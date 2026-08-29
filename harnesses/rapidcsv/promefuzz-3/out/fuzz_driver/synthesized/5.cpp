// This fuzz driver is generated for library rapidcsv, aiming to fuzz the following functions:
// rapidcsv::Document::GetRowCount at rapidcsv.h:1158:12 in rapidcsv.h
// rapidcsv::Document::Load at rapidcsv.h:577:10 in rapidcsv.h
// rapidcsv::Document::GetRowCount at rapidcsv.h:1158:12 in rapidcsv.h
// rapidcsv::Document::Load at rapidcsv.h:554:10 in rapidcsv.h
// rapidcsv::Document::GetRowCount at rapidcsv.h:1158:12 in rapidcsv.h
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <limits>
#include <sstream>
#include <string>

#include "rapidcsv.h"

static int MakeSafeIndex(const uint8_t* data, size_t size, size_t& idx, int minVal, int maxVal, int defaultVal)
{
  if (idx >= size)
  {
    return defaultVal;
  }

  const int range = maxVal - minVal + 1;
  const int value = minVal + static_cast<int>(data[idx++] % static_cast<uint8_t>(range));
  return value;
}

static rapidcsv::LabelParams MakeLabelParams(const uint8_t* data, size_t size, size_t& idx)
{
  const int columnNameIdx = MakeSafeIndex(data, size, idx, -1, 4, 0);
  const int rowNameIdx = MakeSafeIndex(data, size, idx, -1, 4, -1);
  return rapidcsv::LabelParams(columnNameIdx, rowNameIdx);
}

static rapidcsv::SeparatorParams MakeSeparatorParams(const uint8_t* data, size_t size, size_t& idx)
{
  char separator = ',';
  bool trim = false;
  bool hasCR = false;
  bool quotedLinebreaks = false;
  bool autoQuote = true;
  char quoteChar = '"';

  if (idx < size) separator = static_cast<char>(data[idx++]);
  if (idx < size) trim = (data[idx++] & 1) != 0;
  if (idx < size) hasCR = (data[idx++] & 1) != 0;
  if (idx < size) quotedLinebreaks = (data[idx++] & 1) != 0;
  if (idx < size) autoQuote = (data[idx++] & 1) != 0;
  if (idx < size) quoteChar = static_cast<char>(data[idx++]);

  return rapidcsv::SeparatorParams(separator, trim, hasCR, quotedLinebreaks, autoQuote, quoteChar);
}

static rapidcsv::ConverterParams MakeConverterParams(const uint8_t* data, size_t size, size_t& idx)
{
  bool hasDefaultConverter = false;
  long double defaultFloat = std::numeric_limits<long double>::signaling_NaN();
  long long defaultInteger = 0;
  bool numericLocale = true;

  if (idx < size) hasDefaultConverter = (data[idx++] & 1) != 0;
  if (idx < size) defaultFloat = static_cast<long double>(static_cast<int8_t>(data[idx++]));
  if (idx < size) defaultInteger = static_cast<long long>(static_cast<int8_t>(data[idx++]));
  if (idx < size) numericLocale = (data[idx++] & 1) != 0;

  return rapidcsv::ConverterParams(hasDefaultConverter, defaultFloat, defaultInteger, numericLocale);
}

static rapidcsv::LineReaderParams MakeLineReaderParams(const uint8_t* data, size_t size, size_t& idx)
{
  bool skipCommentLines = false;
  char commentPrefix = '#';
  bool skipEmptyLines = false;

  if (idx < size) skipCommentLines = (data[idx++] & 1) != 0;
  if (idx < size) commentPrefix = static_cast<char>(data[idx++]);
  if (idx < size) skipEmptyLines = (data[idx++] & 1) != 0;

  return rapidcsv::LineReaderParams(skipCommentLines, commentPrefix, skipEmptyLines);
}

extern "C" int LLVMFuzzerTestOneInput_5(const uint8_t* Data, size_t Size)
{
  const std::string content(reinterpret_cast<const char*>(Data), Size);

  {
    std::ofstream ofs("./dummy_file", std::ios::binary);
    if (ofs)
    {
      ofs.write(content.data(), static_cast<std::streamsize>(content.size()));
    }
  }

  size_t idx = 0;
  rapidcsv::LabelParams labelParams1(0, -1);
  rapidcsv::SeparatorParams separatorParams1;
  rapidcsv::ConverterParams converterParams1;
  rapidcsv::LineReaderParams lineReaderParams1;

  rapidcsv::LabelParams labelParams2(0, -1);
  rapidcsv::SeparatorParams separatorParams2;
  rapidcsv::ConverterParams converterParams2;
  rapidcsv::LineReaderParams lineReaderParams2;

  try
  {
    labelParams1 = MakeLabelParams(Data, Size, idx);
    separatorParams1 = MakeSeparatorParams(Data, Size, idx);
    converterParams1 = MakeConverterParams(Data, Size, idx);
    lineReaderParams1 = MakeLineReaderParams(Data, Size, idx);

    labelParams2 = MakeLabelParams(Data, Size, idx);
    separatorParams2 = MakeSeparatorParams(Data, Size, idx);
    converterParams2 = MakeConverterParams(Data, Size, idx);
    lineReaderParams2 = MakeLineReaderParams(Data, Size, idx);
  }
  catch (...)
  {
  }

  try
  {
    std::istringstream initialStream(content, std::ios::binary);
    rapidcsv::Document doc(initialStream, labelParams1, separatorParams1, converterParams1, lineReaderParams1);

    (void)doc.GetRowCount();

    std::istringstream loadStream(content, std::ios::binary);
    doc.Load(loadStream, labelParams1, separatorParams1, converterParams1, lineReaderParams1);

    (void)doc.GetRowCount();

    doc.Load(std::string("./dummy_file"), labelParams2, separatorParams2, converterParams2, lineReaderParams2);

    (void)doc.GetRowCount();
  }
  catch (...)
  {
  }

  return 0;
}