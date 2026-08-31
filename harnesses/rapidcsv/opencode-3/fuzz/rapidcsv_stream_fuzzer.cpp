#define RAPIDCSV_MAX_COUNT 1024

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <sstream>
#include <string>
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

void ExerciseDocument(rapidcsv::Document& doc,
                      const int columnNameIdx,
                      const int rowNameIdx,
                      const rapidcsv::LabelParams& labelParams,
                      const rapidcsv::SeparatorParams& separatorParams,
                      const rapidcsv::ConverterParams& converterParams,
                      const rapidcsv::LineReaderParams& lineReaderParams,
                      const std::string& fragment)
{
  const size_t rowCount = doc.GetRowCount();
  const size_t columnCount = doc.GetColumnCount();

  std::vector<std::string> columnNames;
  std::vector<std::string> rowNames;
  IgnoreExceptions([&]() { columnNames = doc.GetColumnNames(); });
  IgnoreExceptions([&]() { rowNames = doc.GetRowNames(); });

  IgnoreExceptions([&]() { (void)doc.GetColumnIdx(fragment); });
  IgnoreExceptions([&]() { (void)doc.GetRowIdx(fragment); });

  if (columnCount > 0)
  {
    IgnoreExceptions([&]() { (void)doc.GetColumn<std::string>(0); });
    IgnoreExceptions([&]() { (void)doc.GetColumn<long long>(0); });
    if (columnNameIdx >= 0)
    {
      IgnoreExceptions([&]() { (void)doc.GetColumnName(0); });
    }
  }

  if (rowCount > 0)
  {
    IgnoreExceptions([&]() { (void)doc.GetRow<std::string>(0); });
    IgnoreExceptions([&]() { (void)doc.GetRow<double>(0); });
    if (rowNameIdx >= 0)
    {
      IgnoreExceptions([&]() { (void)doc.GetRowName(0); });
    }
  }

  if ((rowCount > 0) && (columnCount > 0))
  {
    IgnoreExceptions([&]() { (void)doc.GetCell<std::string>(0, 0); });
    IgnoreExceptions([&]() { (void)doc.GetCell<long long>(0, 0); });
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
    IgnoreExceptions([&]() { (void)doc.GetCell<double>(columnNames.front(), rowNames.front()); });
  }

  IgnoreExceptions([&]() { doc.SetCell<std::string>(0, 0, fragment); });

  if (columnNameIdx >= 0)
  {
    IgnoreExceptions([&]() { doc.SetColumnName(0, fragment); });
  }

  if (rowNameIdx >= 0)
  {
    IgnoreExceptions([&]() { doc.SetRowName(0, fragment); });
  }

  IgnoreExceptions([&]() {
    doc.InsertColumn<std::string>(0, std::vector<std::string>(1, fragment));
  });
  IgnoreExceptions([&]() {
    doc.InsertRow<std::string>(0, std::vector<std::string>(1, fragment));
  });

  IgnoreExceptions([&]() {
    if (doc.GetColumnCount() > 0)
    {
      doc.RemoveColumn(0);
    }
  });

  IgnoreExceptions([&]() {
    if (doc.GetRowCount() > 0)
    {
      doc.RemoveRow(0);
    }
  });

  std::ostringstream output;
  IgnoreExceptions([&]() { doc.Save(output); });

  IgnoreExceptions([&]() {
    std::istringstream roundTrip(output.str());
    doc.Load(roundTrip, labelParams, separatorParams, converterParams, lineReaderParams);
  });
}
} // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
  if ((data == nullptr) || (size == 0) || (size > (1U << 20)))
  {
    return 0;
  }

  const int columnNameIdx = PickLabelIndex(ByteAt(data, size, 0));
  const int rowNameIdx = PickLabelIndex(ByteAt(data, size, 1));
  const rapidcsv::LabelParams labelParams(columnNameIdx, rowNameIdx);

  // Bound row/column growth so separator-heavy inputs stay cheap to fuzz.
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

  const std::string csv(reinterpret_cast<const char*>(data), size);
  const std::string fragment = MakeFragment(data, size);

  try
  {
    std::istringstream input(csv);
    rapidcsv::Document doc(input, labelParams, separatorParams, converterParams, lineReaderParams);
    ExerciseDocument(doc,
                     columnNameIdx,
                     rowNameIdx,
                     labelParams,
                     separatorParams,
                     converterParams,
                     lineReaderParams,
                     fragment);
  }
  catch (const std::exception&)
  {
  }
  catch (...)
  {
  }

  return 0;
}
