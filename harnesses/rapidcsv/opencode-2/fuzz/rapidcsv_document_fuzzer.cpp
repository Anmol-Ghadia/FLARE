#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>

#include <rapidcsv.h>

namespace {

class InputReader
{
public:
  InputReader(const uint8_t* pData, size_t pSize)
    : mData(pData)
    , mSize(pSize)
  {
  }

  uint8_t ConsumeByte()
  {
    if (mSize == 0)
    {
      return 0;
    }

    const uint8_t value = *mData;
    ++mData;
    --mSize;
    return value;
  }

  bool ConsumeBool()
  {
    return ((ConsumeByte() & 1U) != 0U);
  }

  int ConsumeInt(const int pMin, const int pMax)
  {
    const unsigned int range = static_cast<unsigned int>(pMax - pMin + 1);
    return pMin + static_cast<int>(ConsumeByte() % range);
  }

  template<typename T, size_t N>
  T ConsumeChoice(const T (&pChoices)[N])
  {
    return pChoices[ConsumeByte() % N];
  }

  std::string ConsumeRemainingString() const
  {
    return std::string(reinterpret_cast<const char*>(mData), mSize);
  }

private:
  const uint8_t* mData;
  size_t mSize;
};

template<typename F>
void IgnoreExpectedExceptions(F&& pFunc)
{
  try
  {
    pFunc();
  }
  catch (const rapidcsv::no_converter&)
  {
  }
  catch (const std::invalid_argument&)
  {
  }
  catch (const std::out_of_range&)
  {
  }
  catch (const std::ios_base::failure&)
  {
  }
}

size_t PickIndex(const uint8_t pSelector, const size_t pCount)
{
  if (pCount == 0)
  {
    return 0;
  }

  return static_cast<size_t>(pSelector) % pCount;
}

size_t PickInsertPos(const uint8_t pSelector, const size_t pCount)
{
  return static_cast<size_t>(pSelector) % (pCount + 1);
}

std::string SampleString(const std::string& pData, const size_t pOffset, const char* pFallback)
{
  if (pData.empty())
  {
    return std::string(pFallback);
  }

  const size_t start = std::min(pOffset, pData.size() - 1);
  const size_t length = std::min<size_t>(16, pData.size() - start);
  const std::string sample = pData.substr(start, length);
  return sample.empty() ? std::string(pFallback) : sample;
}

std::string TempPath(const char* pSuffix)
{
  return std::string("/tmp/rapidcsv_fuzzer_") + std::to_string(static_cast<long long>(::getpid())) + pSuffix;
}

void ExerciseLookups(rapidcsv::Document& pDoc, const uint8_t pRowSelector, const uint8_t pColumnSelector)
{
  const size_t rowCount = pDoc.GetRowCount();
  const size_t columnCount = pDoc.GetColumnCount();
  const size_t rowIdx = PickIndex(pRowSelector, rowCount);
  const size_t columnIdx = PickIndex(pColumnSelector, columnCount);

  IgnoreExpectedExceptions([&]() { static_cast<void>(pDoc.GetColumnNames()); });
  IgnoreExpectedExceptions([&]() { static_cast<void>(pDoc.GetRowNames()); });

  if (columnCount > 0)
  {
    IgnoreExpectedExceptions([&]() { static_cast<void>(pDoc.GetColumn<std::string>(columnIdx)); });
    IgnoreExpectedExceptions([&]() { static_cast<void>(pDoc.GetColumn<char>(columnIdx)); });
    IgnoreExpectedExceptions([&]() { static_cast<void>(pDoc.GetColumn<int>(columnIdx)); });
    IgnoreExpectedExceptions([&]() { static_cast<void>(pDoc.GetColumnName(columnIdx)); });
  }

  if (rowCount > 0)
  {
    IgnoreExpectedExceptions([&]() { static_cast<void>(pDoc.GetRow<std::string>(rowIdx)); });
    IgnoreExpectedExceptions([&]() { static_cast<void>(pDoc.GetRow<char>(rowIdx)); });
    IgnoreExpectedExceptions([&]() { static_cast<void>(pDoc.GetRow<double>(rowIdx)); });
    IgnoreExpectedExceptions([&]() { static_cast<void>(pDoc.GetRowName(rowIdx)); });
  }

  if ((columnCount > 0) && (rowCount > 0))
  {
    IgnoreExpectedExceptions([&]() { static_cast<void>(pDoc.GetCell<std::string>(columnIdx, rowIdx)); });
    IgnoreExpectedExceptions([&]() { static_cast<void>(pDoc.GetCell<char>(columnIdx, rowIdx)); });
    IgnoreExpectedExceptions([&]() { static_cast<void>(pDoc.GetCell<double>(columnIdx, rowIdx)); });
  }

  std::vector<std::string> columnNames;
  std::vector<std::string> rowNames;
  IgnoreExpectedExceptions([&]() { columnNames = pDoc.GetColumnNames(); });
  IgnoreExpectedExceptions([&]() { rowNames = pDoc.GetRowNames(); });

  if (!columnNames.empty())
  {
    const std::string& columnName = columnNames[PickIndex(pColumnSelector, columnNames.size())];
    IgnoreExpectedExceptions([&]() { static_cast<void>(pDoc.GetColumn<std::string>(columnName)); });
    if (rowCount > 0)
    {
      IgnoreExpectedExceptions([&]() { static_cast<void>(pDoc.GetCell<std::string>(columnName, rowIdx)); });
    }
  }

  if (!rowNames.empty())
  {
    const std::string& rowName = rowNames[PickIndex(pRowSelector, rowNames.size())];
    IgnoreExpectedExceptions([&]() { static_cast<void>(pDoc.GetRow<std::string>(rowName)); });
    if (columnCount > 0)
    {
      IgnoreExpectedExceptions([&]() { static_cast<void>(pDoc.GetCell<std::string>(columnIdx, rowName)); });
    }
  }

  if (!columnNames.empty() && !rowNames.empty())
  {
    const std::string& columnName = columnNames[PickIndex(pColumnSelector, columnNames.size())];
    const std::string& rowName = rowNames[PickIndex(pRowSelector, rowNames.size())];
    IgnoreExpectedExceptions([&]() { static_cast<void>(pDoc.GetCell<std::string>(columnName, rowName)); });
  }
}

void ExerciseMutations(rapidcsv::Document& pDoc,
                       const std::string& pSampleA,
                       const std::string& pSampleB,
                       const uint8_t pRowSelector,
                       const uint8_t pColumnSelector)
{
  const size_t rowPos = PickInsertPos(pRowSelector, pDoc.GetRowCount());
  const size_t columnPos = PickInsertPos(pColumnSelector, pDoc.GetColumnCount());

  IgnoreExpectedExceptions([&]() { pDoc.SetCell<std::string>(columnPos, rowPos, pSampleA); });
  IgnoreExpectedExceptions([&]() { pDoc.SetCell<int>(0, 0, static_cast<int>(pColumnSelector)); });
  IgnoreExpectedExceptions([&]() { pDoc.SetColumnName(0, pSampleA); });
  IgnoreExpectedExceptions([&]() { pDoc.SetRowName(0, pSampleB); });
  IgnoreExpectedExceptions([&]() {
    pDoc.InsertRow<std::string>(rowPos, std::vector<std::string>({ pSampleA, pSampleB }), pSampleB);
  });
  IgnoreExpectedExceptions([&]() {
    pDoc.InsertColumn<std::string>(columnPos, std::vector<std::string>({ pSampleB, pSampleA }), pSampleA);
  });

  if (pDoc.GetRowCount() > 0)
  {
    IgnoreExpectedExceptions([&]() { pDoc.RemoveRow(PickIndex(pRowSelector, pDoc.GetRowCount())); });
  }

  if (pDoc.GetColumnCount() > 0)
  {
    IgnoreExpectedExceptions([&]() { pDoc.RemoveColumn(PickIndex(pColumnSelector, pDoc.GetColumnCount())); });
  }
}

void ExerciseDocument(rapidcsv::Document& pDoc,
                      const std::string& pSampleA,
                      const std::string& pSampleB,
                      const uint8_t pRowSelector,
                      const uint8_t pColumnSelector)
{
  ExerciseLookups(pDoc, pRowSelector, pColumnSelector);
  ExerciseMutations(pDoc, pSampleA, pSampleB, pRowSelector, pColumnSelector);
  ExerciseLookups(pDoc, static_cast<uint8_t>(pRowSelector ^ 0x5aU), static_cast<uint8_t>(pColumnSelector ^ 0xa5U));
}

} // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* pData, size_t pSize)
{
  static const size_t sMaxInputSize = 256 * 1024;
  if (pSize > sMaxInputSize)
  {
    return 0;
  }

  InputReader input(pData, pSize);

  const uint8_t rowSelector = input.ConsumeByte();
  const uint8_t columnSelector = input.ConsumeByte();
  const rapidcsv::LabelParams labelParams(input.ConsumeInt(-1, 2), input.ConsumeInt(-1, 2));

  static const char separators[] = { ',', ';', '\t', '|', ':' };
  static const char quoteChars[] = { '"', '\'', '`' };
  static const char commentPrefixes[] = { '#', ';', '/', '%', '!' };

  const rapidcsv::SeparatorParams separatorParams(input.ConsumeChoice(separators),
                                                  input.ConsumeBool(),
                                                  input.ConsumeBool(),
                                                  input.ConsumeBool(),
                                                  input.ConsumeBool(),
                                                  input.ConsumeChoice(quoteChars));

  const rapidcsv::ConverterParams converterParams(input.ConsumeBool(),
                                                  static_cast<long double>(static_cast<int8_t>(input.ConsumeByte())) / 8.0L,
                                                  static_cast<long long>(static_cast<int8_t>(input.ConsumeByte())),
                                                  input.ConsumeBool());

  const rapidcsv::LineReaderParams lineReaderParams(input.ConsumeBool(),
                                                    input.ConsumeChoice(commentPrefixes),
                                                    input.ConsumeBool());

  const std::string csv = input.ConsumeRemainingString();
  const std::string sampleA = SampleString(csv, 0, "col");
  const std::string sampleB = SampleString(csv, csv.size() / 2, "row");
  const std::string tempPath = TempPath(".csv");

  IgnoreExpectedExceptions([&]() {
    std::istringstream stream(csv);
    rapidcsv::Document doc(stream, labelParams, separatorParams, converterParams, lineReaderParams);
    ExerciseDocument(doc, sampleA, sampleB, rowSelector, columnSelector);

    std::ostringstream output;
    doc.Save(output);

    std::istringstream reloadStream(output.str());
    rapidcsv::Document reloaded;
    reloaded.Load(reloadStream, labelParams, separatorParams, converterParams, lineReaderParams);
    ExerciseDocument(reloaded, sampleB, sampleA,
                     static_cast<uint8_t>(rowSelector ^ 0x33U),
                     static_cast<uint8_t>(columnSelector ^ 0xccU));

    doc.Save(tempPath);
    rapidcsv::Document roundTrip(tempPath, labelParams, separatorParams, converterParams, lineReaderParams);
    ExerciseDocument(roundTrip, sampleA, sampleB,
                     static_cast<uint8_t>(rowSelector ^ 0x0fU),
                     static_cast<uint8_t>(columnSelector ^ 0xf0U));
  });

  std::remove(tempPath.c_str());
  return 0;
}
