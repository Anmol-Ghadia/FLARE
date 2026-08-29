#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <cstddef>
#include <cstdint>
#include <cstddef>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <limits>

#include "rapidcsv.h"

static rapidcsv::LabelParams MakeLabelParams(const uint8_t* data, size_t size, size_t& idx)
{
  int columnNameIdx = 0;
  int rowNameIdx = -1;

  if (idx < size)
  {
    columnNameIdx = static_cast<int>(static_cast<int8_t>(data[idx++]));
  }
  if (idx < size)
  {
    rowNameIdx = static_cast<int>(static_cast<int8_t>(data[idx++]));
  }

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

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
  std::string content(reinterpret_cast<const char*>(Data), Size);

  {
    std::ofstream ofs("./dummy_file", std::ios::binary);
    ofs.write(content.data(), static_cast<std::streamsize>(content.size()));
  }

  size_t idx = 0;
  rapidcsv::LabelParams labelParams1 = MakeLabelParams(Data, Size, idx);
  rapidcsv::SeparatorParams separatorParams1 = MakeSeparatorParams(Data, Size, idx);
  rapidcsv::ConverterParams converterParams1 = MakeConverterParams(Data, Size, idx);
  rapidcsv::LineReaderParams lineReaderParams1 = MakeLineReaderParams(Data, Size, idx);

  rapidcsv::LabelParams labelParams2 = MakeLabelParams(Data, Size, idx);
  rapidcsv::SeparatorParams separatorParams2 = MakeSeparatorParams(Data, Size, idx);
  rapidcsv::ConverterParams converterParams2 = MakeConverterParams(Data, Size, idx);
  rapidcsv::LineReaderParams lineReaderParams2 = MakeLineReaderParams(Data, Size, idx);

  std::istringstream iss(content, std::ios::binary);

  try
  {
    rapidcsv::Document doc(iss, labelParams1, separatorParams1, converterParams1, lineReaderParams1);

    (void)doc.GetRowCount();
    doc.Load(iss, labelParams1, separatorParams1, converterParams1, lineReaderParams1);
    (void)doc.GetRowCount();
    doc.Load(std::string("./dummy_file"), labelParams2, separatorParams2, converterParams2, lineReaderParams2);
    (void)doc.GetRowCount();
  }
  catch (...)
  {
  }

  return 0;
}