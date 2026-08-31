#define RAPIDCSV_MAX_COUNT 1024

#include <algorithm>
#include <cstdio>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <string>
#include <unistd.h>
#include <vector>

#include <rapidcsv.h>

namespace
{
uint8_t ByteAt(const uint8_t* data, size_t size, size_t index)
{
  return data[index % size];
}

int PickLabelIndex(const uint8_t byte)
{
  switch (byte % 3)
  {
    case 0:
      return -1;

    case 1:
      return 0;

    default:
      return 1;
  }
}

char PickSeparator(const uint8_t byte)
{
  static const char kSeparators[] = { ',', ';', '\t', '|', ' ' };
  return kSeparators[byte % (sizeof(kSeparators) / sizeof(kSeparators[0]))];
}

char PickQuoteChar(const uint8_t byte)
{
  static const char kQuoteChars[] = { '"', '\'', '`' };
  return kQuoteChars[byte % (sizeof(kQuoteChars) / sizeof(kQuoteChars[0]))];
}

std::string MakeFragment(const uint8_t* data, size_t size)
{
  const size_t offset = ByteAt(data, size, 12) % size;
  size_t length = std::min<size_t>(size - offset, 32);
  if (length == 0)
  {
    length = std::min<size_t>(size, 32);
    return std::string(reinterpret_cast<const char*>(data), length);
  }

  return std::string(reinterpret_cast<const char*>(data + offset), length);
}

std::string MakeTempPath(const char* suffix)
{
  return std::string("/tmp/rapidcsv-") + suffix + "-" +
         std::to_string(static_cast<long long>(getpid())) + ".csv";
}

bool WriteInputFile(const std::string& path, const uint8_t* data, size_t size)
{
  std::ofstream output(path.c_str(), std::ios::binary | std::ios::trunc);
  if (!output)
  {
    return false;
  }

  output.write(reinterpret_cast<const char*>(data), static_cast<std::streamsize>(size));
  return output.good();
}

template<typename Callable>
void IgnoreExceptions(const Callable& callable)
{
  try
  {
    callable();
  }
  catch (const std::exception&)
  {
  }
  catch (...)
  {
  }
}

void ExerciseDocument(rapidcsv::Document& doc, const std::string& fragment)
{
  const size_t rowCount = doc.GetRowCount();
  const size_t columnCount = doc.GetColumnCount();

  std::vector<std::string> columnNames;
  std::vector<std::string> rowNames;
  IgnoreExceptions([&]() { columnNames = doc.GetColumnNames(); });
  IgnoreExceptions([&]() { rowNames = doc.GetRowNames(); });

  IgnoreExceptions([&]() { (void)doc.GetColumnIdx(fragment); });
  IgnoreExceptions([&]() { (void)doc.GetRowIdx(fragment); });

  if ((rowCount > 0) && (columnCount > 0))
  {
    IgnoreExceptions([&]() { (void)doc.GetCell<std::string>(0, 0); });
    IgnoreExceptions([&]() { (void)doc.GetCell<double>(0, 0); });
    IgnoreExceptions([&]() { (void)doc.GetCell<char>(0, 0); });
  }

  if (!columnNames.empty())
  {
    IgnoreExceptions([&]() { (void)doc.GetColumn<std::string>(columnNames.front()); });
  }

  if (!rowNames.empty())
  {
    IgnoreExceptions([&]() { (void)doc.GetRow<std::string>(rowNames.front()); });
  }

  if (!columnNames.empty() && !rowNames.empty())
  {
    IgnoreExceptions([&]() { (void)doc.GetCell<std::string>(columnNames.front(), rowNames.front()); });
  }
}
} // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
  if ((data == nullptr) || (size == 0) || (size > (1U << 20)))
  {
    return 0;
  }

  const std::string inputPath = MakeTempPath("in");
  const std::string outputPath = MakeTempPath("out");
  if (!WriteInputFile(inputPath, data, size))
  {
    std::remove(inputPath.c_str());
    return 0;
  }

  const int columnNameIdx = PickLabelIndex(ByteAt(data, size, 0));
  const int rowNameIdx = PickLabelIndex(ByteAt(data, size, 1));
  const rapidcsv::LabelParams labelParams(columnNameIdx, rowNameIdx);
  const rapidcsv::SeparatorParams separatorParams(PickSeparator(ByteAt(data, size, 2)),
                                                  ((ByteAt(data, size, 3) & 1U) != 0),
                                                  ((ByteAt(data, size, 4) & 1U) != 0),
                                                  ((ByteAt(data, size, 5) & 1U) != 0),
                                                  ((ByteAt(data, size, 6) & 1U) != 0),
                                                  PickQuoteChar(ByteAt(data, size, 7)));
  const rapidcsv::ConverterParams converterParams(((ByteAt(data, size, 8) & 1U) != 0),
                                                  static_cast<long double>(static_cast<int>(ByteAt(data, size, 9)) - 128),
                                                  static_cast<long long>(static_cast<int>(ByteAt(data, size, 10)) - 128),
                                                  ((ByteAt(data, size, 11) & 1U) != 0));
  const rapidcsv::LineReaderParams lineReaderParams(((ByteAt(data, size, 12) & 1U) != 0),
                                                    static_cast<char>(ByteAt(data, size, 13)),
                                                    ((ByteAt(data, size, 14) & 1U) != 0));
  const std::string fragment = MakeFragment(data, size);

  try
  {
    rapidcsv::Document doc(inputPath, labelParams, separatorParams, converterParams, lineReaderParams);
    ExerciseDocument(doc, fragment);

    IgnoreExceptions([&]() { doc.SetCell<std::string>(0, 0, fragment); });
    if (columnNameIdx >= 0)
    {
      IgnoreExceptions([&]() { doc.SetColumnName(0, fragment); });
    }
    if (rowNameIdx >= 0)
    {
      IgnoreExceptions([&]() { doc.SetRowName(0, fragment); });
    }

    IgnoreExceptions([&]() { doc.Save(outputPath); });

    IgnoreExceptions([&]() {
      rapidcsv::Document reloaded(outputPath, labelParams, separatorParams, converterParams, lineReaderParams);
      ExerciseDocument(reloaded, fragment);
      reloaded.Load(outputPath, labelParams, separatorParams, converterParams, lineReaderParams);
      ExerciseDocument(reloaded, fragment);
    });
  }
  catch (const std::exception&)
  {
  }
  catch (...)
  {
  }

  std::remove(inputPath.c_str());
  std::remove(outputPath.c_str());
  return 0;
}
