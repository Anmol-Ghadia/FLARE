// This fuzz driver is generated for library rapidcsv, aiming to fuzz the following functions:
// rapidcsv::Document::RemoveColumn at rapidcsv.h:798:10 in rapidcsv.h
// rapidcsv::Document::GetColumnName at rapidcsv.h:1425:17 in rapidcsv.h
// rapidcsv::Document::RemoveColumn at rapidcsv.h:827:10 in rapidcsv.h
// rapidcsv::Document::GetColumnName at rapidcsv.h:1425:17 in rapidcsv.h
// rapidcsv::Document::RemoveColumn at rapidcsv.h:798:10 in rapidcsv.h
// rapidcsv::Document::GetColumnName at rapidcsv.h:1425:17 in rapidcsv.h
// rapidcsv::Document::RemoveColumn at rapidcsv.h:827:10 in rapidcsv.h
// rapidcsv::Document::GetColumnName at rapidcsv.h:1425:17 in rapidcsv.h
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
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>

#include "rapidcsv.h"

static size_t ReadSizeT(const uint8_t* data, size_t size, size_t& offset)
{
  size_t value = 0;
  const size_t n = std::min(sizeof(size_t), size - offset);
  for (size_t i = 0; i < n; ++i)
  {
    value = (value << 8) | data[offset++];
  }
  return value;
}

static int ReadInt(const uint8_t* data, size_t size, size_t& offset)
{
  int value = 0;
  const size_t n = std::min(sizeof(int), size - offset);
  for (size_t i = 0; i < n; ++i)
  {
    value = (value << 8) | data[offset++];
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
  char c = static_cast<char>(data[offset++]);
  if (c == '\0' || c == '\n' || c == '\r')
  {
    return fallback;
  }
  return c;
}

static long long ReadLongLong(const uint8_t* data, size_t size, size_t& offset)
{
  long long value = 0;
  const size_t n = std::min(sizeof(long long), size - offset);
  for (size_t i = 0; i < n; ++i)
  {
    value = (value << 8) | data[offset++];
  }
  return value;
}

static long double ReadLongDouble(const uint8_t* data, size_t size, size_t& offset)
{
  long double out = 0.0L;
  if (offset >= size) return out;
  uint64_t bits = 0;
  const size_t n = std::min(sizeof(uint64_t), size - offset);
  for (size_t i = 0; i < n; ++i)
  {
    bits = (bits << 8) | data[offset++];
  }
  out = static_cast<long double>(bits);
  return out;
}

static std::string ReadString(const uint8_t* data, size_t size, size_t& offset)
{
  if (offset >= size) return std::string();
  size_t len = ReadSizeT(data, size, offset);
  if (offset > size) return std::string();
  len = std::min(len, size - offset);
  std::string s(reinterpret_cast<const char*>(data + offset), len);
  offset += len;
  return s;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
  size_t offset = 0;

  rapidcsv::LabelParams labelParams;
  labelParams.mColumnNameIdx = (Size > 0) ? (ReadInt(Data, Size, offset) % 7 - 3) : 0;

  rapidcsv::SeparatorParams separatorParams;
  separatorParams.mSeparator = ReadChar(Data, Size, offset, ',');
  separatorParams.mTrim = ReadBool(Data, Size, offset);

  rapidcsv::ConverterParams converterParams;
  converterParams.mHasDefaultConverter = ReadBool(Data, Size, offset);
  converterParams.mDefaultFloat = ReadLongDouble(Data, Size, offset);
  converterParams.mDefaultInteger = ReadLongLong(Data, Size, offset);

  rapidcsv::LineReaderParams lineReaderParams;
  lineReaderParams.mSkipCommentLines = ReadBool(Data, Size, offset);
  lineReaderParams.mCommentPrefix = ReadChar(Data, Size, offset, '#');

  std::string extraName = ReadString(Data, Size, offset);
  size_t idx1 = ReadSizeT(Data, Size, offset);
  size_t idx2 = ReadSizeT(Data, Size, offset);

  std::ofstream ofs("./dummy_file", std::ios::binary);
  if (ofs)
  {
    if (offset < Size)
    {
      ofs.write(reinterpret_cast<const char*>(Data + offset), static_cast<std::streamsize>(Size - offset));
    }
    ofs.close();
  }

  try
  {
    rapidcsv::Document doc("./dummy_file", labelParams, separatorParams, converterParams, lineReaderParams);

    try
    {
      doc.RemoveColumn(idx1);
    }
    catch (...)
    {
    }

    try
    {
      (void)doc.GetColumnName(idx1);
    }
    catch (...)
    {
    }

    try
    {
      doc.RemoveColumn(extraName);
    }
    catch (...)
    {
    }

    try
    {
      (void)doc.GetColumnName(idx2);
    }
    catch (...)
    {
    }

    try
    {
      doc.RemoveColumn(idx2);
    }
    catch (...)
    {
    }

    try
    {
      (void)doc.GetColumnName(0);
    }
    catch (...)
    {
    }

    try
    {
      doc.RemoveColumn(std::string());
    }
    catch (...)
    {
    }

    try
    {
      (void)doc.GetColumnName(static_cast<size_t>(-1));
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