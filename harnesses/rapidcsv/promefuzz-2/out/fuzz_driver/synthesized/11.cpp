// This fuzz driver is generated for library rapidcsv, aiming to fuzz the following functions:
// rapidcsv::Document::SetCell at rapidcsv.h:1331:10 in rapidcsv.h
// rapidcsv::Document::SetCell at rapidcsv.h:1331:10 in rapidcsv.h
// rapidcsv::Document::Save at rapidcsv.h:597:10 in rapidcsv.h
// rapidcsv::Document::GetRowCount at rapidcsv.h:1158:12 in rapidcsv.h
// rapidcsv::Document::GetColumnCount at rapidcsv.h:910:12 in rapidcsv.h
// rapidcsv::Document::GetCell at rapidcsv.h:1172:7 in rapidcsv.h
// rapidcsv::Document::GetCell at rapidcsv.h:1172:7 in rapidcsv.h
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

#include "rapidcsv.h"

static size_t ReadSizeT(const uint8_t* data, size_t size, size_t& offset)
{
  size_t value = 0;
  const size_t n = sizeof(size_t);
  for (size_t i = 0; i < n && offset < size; ++i)
  {
    value = (value << 8) | static_cast<size_t>(data[offset++]);
  }
  return value;
}

static bool ReadBool(const uint8_t* data, size_t size, size_t& offset)
{
  if (offset >= size) return false;
  return (data[offset++] & 1U) != 0;
}

static char ReadChar(const uint8_t* data, size_t size, size_t& offset, char fallback)
{
  if (offset >= size) return fallback;
  return static_cast<char>(data[offset++]);
}

static long long ReadLongLong(const uint8_t* data, size_t size, size_t& offset)
{
  long long value = 0;
  const size_t n = sizeof(long long);
  for (size_t i = 0; i < n && offset < size; ++i)
  {
    value = (value << 8) | static_cast<long long>(data[offset++]);
  }
  return value;
}

static long double ReadLongDouble(const uint8_t* data, size_t size, size_t& offset)
{
  long double result = 0.0L;
  if (offset >= size) return result;

  uint64_t bits = 0;
  const size_t n = sizeof(uint64_t);
  for (size_t i = 0; i < n && offset < size; ++i)
  {
    bits = (bits << 8) | static_cast<uint64_t>(data[offset++]);
  }
  result = static_cast<long double>(bits) / 17.0L;
  return result;
}

static std::string ReadString(const uint8_t* data, size_t size, size_t& offset)
{
  if (offset >= size) return std::string();

  size_t len = static_cast<size_t>(data[offset++]);
  if (len > size - offset) len = size - offset;

  std::string out(reinterpret_cast<const char*>(data + offset), len);
  offset += len;
  return out;
}

extern "C" int LLVMFuzzerTestOneInput_11(const uint8_t *Data, size_t Size)
{
  size_t offset = 0;

  rapidcsv::LabelParams labelParams;
  labelParams.mColumnNameIdx = static_cast<int>(ReadLongLong(Data, Size, offset) % 8LL) - 4;

  rapidcsv::SeparatorParams separatorParams;
  separatorParams.mSeparator = ReadChar(Data, Size, offset, ',');
  if (separatorParams.mSeparator == '\0' || separatorParams.mSeparator == '\n' || separatorParams.mSeparator == '\r')
  {
    separatorParams.mSeparator = ',';
  }
  separatorParams.mTrim = ReadBool(Data, Size, offset);

  rapidcsv::ConverterParams converterParams;
  converterParams.mHasDefaultConverter = ReadBool(Data, Size, offset);
  converterParams.mDefaultFloat = ReadLongDouble(Data, Size, offset);
  converterParams.mDefaultInteger = ReadLongLong(Data, Size, offset);

  rapidcsv::LineReaderParams lineReaderParams;
  lineReaderParams.mSkipCommentLines = ReadBool(Data, Size, offset);
  lineReaderParams.mCommentPrefix = ReadChar(Data, Size, offset, '#');
  if (lineReaderParams.mCommentPrefix == '\0')
  {
    lineReaderParams.mCommentPrefix = '#';
  }

  std::string initialCsv = ReadString(Data, Size, offset);
  std::stringstream ss(initialCsv);

  try
  {
    rapidcsv::Document doc(ss, labelParams, separatorParams, converterParams, lineReaderParams);

    size_t col1 = ReadSizeT(Data, Size, offset) % 16;
    size_t row1 = ReadSizeT(Data, Size, offset) % 16;
    std::string cell1 = ReadString(Data, Size, offset);

    size_t col2 = ReadSizeT(Data, Size, offset) % 16;
    size_t row2 = ReadSizeT(Data, Size, offset) % 16;
    long long cell2 = ReadLongLong(Data, Size, offset);

    try
    {
      doc.SetCell<std::string>(col1, row1, cell1);
    }
    catch (...)
    {
    }

    try
    {
      doc.SetCell<long long>(col2, row2, cell2);
    }
    catch (...)
    {
    }

    try
    {
      std::ofstream out("./dummy_file", std::ios::binary);
      out.write(initialCsv.data(), static_cast<std::streamsize>(initialCsv.size()));
    }
    catch (...)
    {
    }

    try
    {
      doc.Save("./dummy_file");
    }
    catch (...)
    {
    }

    size_t rowCount = 0;
    size_t colCount = 0;

    try
    {
      rowCount = doc.GetRowCount();
    }
    catch (...)
    {
    }

    try
    {
      colCount = doc.GetColumnCount();
    }
    catch (...)
    {
    }

    size_t getCol1 = colCount ? (ReadSizeT(Data, Size, offset) % (colCount + 4)) : (ReadSizeT(Data, Size, offset) % 4);
    size_t getRow1 = rowCount ? (ReadSizeT(Data, Size, offset) % (rowCount + 4)) : (ReadSizeT(Data, Size, offset) % 4);
    size_t getCol2 = colCount ? (ReadSizeT(Data, Size, offset) % (colCount + 4)) : (ReadSizeT(Data, Size, offset) % 4);
    size_t getRow2 = rowCount ? (ReadSizeT(Data, Size, offset) % (rowCount + 4)) : (ReadSizeT(Data, Size, offset) % 4);

    try
    {
      volatile std::string s = doc.GetCell<std::string>(getCol1, getRow1);
      (void)s;
    }
    catch (...)
    {
    }

    try
    {
      volatile long long v = doc.GetCell<long long>(getCol2, getRow2);
      (void)v;
    }
    catch (...)
    {
    }
  }
  catch (...)
  {
  }

  return 0;
}