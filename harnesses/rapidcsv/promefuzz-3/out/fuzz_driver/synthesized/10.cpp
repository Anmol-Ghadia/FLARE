// This fuzz driver is generated for library rapidcsv, aiming to fuzz the following functions:
// rapidcsv::Document::RemoveRow at rapidcsv.h:1076:10 in rapidcsv.h
// rapidcsv::Document::RemoveRow at rapidcsv.h:1097:10 in rapidcsv.h
// rapidcsv::Document::Save at rapidcsv.h:597:10 in rapidcsv.h
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
#include <sstream>
#include <fstream>
#include <vector>
#include "rapidcsv.h"

static size_t ReadSizeT(const uint8_t* data, size_t size, size_t& offset)
{
  size_t value = 0;
  const size_t n = sizeof(size_t);
  for (size_t i = 0; i < n; ++i)
  {
    value <<= 8;
    if (offset < size)
    {
      value |= static_cast<size_t>(data[offset]);
      ++offset;
    }
  }
  return value;
}

static int ReadInt(const uint8_t* data, size_t size, size_t& offset)
{
  int value = 0;
  const size_t n = sizeof(int);
  for (size_t i = 0; i < n; ++i)
  {
    value <<= 8;
    if (offset < size)
    {
      value |= static_cast<int>(data[offset]);
      ++offset;
    }
  }
  return value;
}

static long long ReadLongLong(const uint8_t* data, size_t size, size_t& offset)
{
  long long value = 0;
  const size_t n = sizeof(long long);
  for (size_t i = 0; i < n; ++i)
  {
    value <<= 8;
    if (offset < size)
    {
      value |= static_cast<long long>(data[offset]);
      ++offset;
    }
  }
  return value;
}

static bool ReadBool(const uint8_t* data, size_t size, size_t& offset)
{
  if (offset < size)
  {
    return (data[offset++] & 1U) != 0;
  }
  return false;
}

static char ReadChar(const uint8_t* data, size_t size, size_t& offset, char fallback)
{
  if (offset < size)
  {
    return static_cast<char>(data[offset++]);
  }
  return fallback;
}

static std::string ReadString(const uint8_t* data, size_t size, size_t& offset)
{
  size_t len = 0;
  if (offset < size)
  {
    len = static_cast<size_t>(data[offset++]);
  }
  if (offset + len > size)
  {
    len = size - offset;
  }
  std::string s(reinterpret_cast<const char*>(data + offset), len);
  offset += len;
  return s;
}

extern "C" int LLVMFuzzerTestOneInput_10(const uint8_t *Data, size_t Size)
{
  size_t offset = 0;

  rapidcsv::LabelParams labelParams;
  labelParams.mColumnNameIdx = ReadInt(Data, Size, offset);

  rapidcsv::SeparatorParams separatorParams;
  separatorParams.mSeparator = ReadChar(Data, Size, offset, ',');
  separatorParams.mTrim = ReadBool(Data, Size, offset);

  rapidcsv::ConverterParams converterParams;
  converterParams.mHasDefaultConverter = ReadBool(Data, Size, offset);
  converterParams.mDefaultInteger = ReadLongLong(Data, Size, offset);
  converterParams.mDefaultFloat = static_cast<long double>(ReadLongLong(Data, Size, offset));

  rapidcsv::LineReaderParams lineReaderParams;
  lineReaderParams.mSkipCommentLines = ReadBool(Data, Size, offset);
  lineReaderParams.mCommentPrefix = ReadChar(Data, Size, offset, '#');

  std::string rowName = ReadString(Data, Size, offset);
  size_t rowIdx = ReadSizeT(Data, Size, offset);

  std::string csvContent;
  if (offset < Size)
  {
    csvContent.assign(reinterpret_cast<const char*>(Data + offset), Size - offset);
  }

  std::stringstream ss(csvContent);

  try
  {
    rapidcsv::Document doc(ss, labelParams, separatorParams, converterParams, lineReaderParams);

    try
    {
      doc.RemoveRow(rowIdx);
    }
    catch (...)
    {
    }

    try
    {
      doc.RemoveRow(rowName);
    }
    catch (...)
    {
    }

    try
    {
      std::ofstream ofs("./dummy_file", std::ios::binary);
      ofs.write(csvContent.data(), static_cast<std::streamsize>(csvContent.size()));
      ofs.close();
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
  }
  catch (...)
  {
  }

  return 0;
}