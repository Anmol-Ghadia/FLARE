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
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>

#include "rapidcsv.h"

static uint32_t ReadU32(const uint8_t* data, size_t size, size_t& offset)
{
  if (offset + 4 > size)
  {
    offset = size;
    return 0;
  }

  uint32_t v = static_cast<uint32_t>(data[offset]) |
               (static_cast<uint32_t>(data[offset + 1]) << 8) |
               (static_cast<uint32_t>(data[offset + 2]) << 16) |
               (static_cast<uint32_t>(data[offset + 3]) << 24);
  offset += 4;
  return v;
}

static bool ReadBool(const uint8_t* data, size_t size, size_t& offset)
{
  if (offset >= size)
  {
    return false;
  }
  return (data[offset++] & 1U) != 0;
}

static char ReadChar(const uint8_t* data, size_t size, size_t& offset, char fallback)
{
  if (offset >= size)
  {
    return fallback;
  }
  return static_cast<char>(data[offset++]);
}

static std::string ReadString(const uint8_t* data, size_t size, size_t& offset, size_t maxLen)
{
  uint32_t len = ReadU32(data, size, offset);
  size_t actualLen = std::min<size_t>(static_cast<size_t>(len), std::min(maxLen, size - offset));
  std::string s(reinterpret_cast<const char*>(data + offset), actualLen);
  offset += actualLen;
  return s;
}

static std::vector<int> ReadIntVector(const uint8_t* data, size_t size, size_t& offset, size_t maxElems)
{
  uint32_t count = ReadU32(data, size, offset);
  size_t actualCount = std::min<size_t>(count, maxElems);
  std::vector<int> out;
  out.reserve(actualCount);

  for (size_t i = 0; i < actualCount; ++i)
  {
    out.push_back(static_cast<int>(ReadU32(data, size, offset)));
  }

  return out;
}

static std::vector<std::string> ReadStringVector(const uint8_t* data, size_t size, size_t& offset, size_t maxElems, size_t maxStrLen)
{
  uint32_t count = ReadU32(data, size, offset);
  size_t actualCount = std::min<size_t>(count, maxElems);
  std::vector<std::string> out;
  out.reserve(actualCount);

  for (size_t i = 0; i < actualCount; ++i)
  {
    out.push_back(ReadString(data, size, offset, maxStrLen));
  }

  return out;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
  size_t offset = 0;

  rapidcsv::LabelParams labelParams;
  labelParams.mColumnNameIdx = static_cast<int>(ReadU32(Data, Size, offset) % 4U) - 1;

  rapidcsv::SeparatorParams separatorParams;
  separatorParams.mSeparator = ReadChar(Data, Size, offset, ',');
  if (separatorParams.mSeparator == '\0' || separatorParams.mSeparator == '\n' || separatorParams.mSeparator == '\r')
  {
    separatorParams.mSeparator = ',';
  }
  separatorParams.mTrim = ReadBool(Data, Size, offset);

  rapidcsv::ConverterParams converterParams;
  converterParams.mHasDefaultConverter = ReadBool(Data, Size, offset);
  converterParams.mDefaultFloat = static_cast<long double>(static_cast<int>(ReadU32(Data, Size, offset)));
  converterParams.mDefaultInteger = static_cast<long long>(static_cast<int>(ReadU32(Data, Size, offset)));

  rapidcsv::LineReaderParams lineReaderParams;
  lineReaderParams.mSkipCommentLines = ReadBool(Data, Size, offset);
  lineReaderParams.mCommentPrefix = ReadChar(Data, Size, offset, '#');
  if (lineReaderParams.mCommentPrefix == '\0' || lineReaderParams.mCommentPrefix == '\n' || lineReaderParams.mCommentPrefix == '\r')
  {
    lineReaderParams.mCommentPrefix = '#';
  }

  std::string csvContent;
  if (offset < Size)
  {
    csvContent.assign(reinterpret_cast<const char*>(Data + offset), Size - offset);
  }

  std::stringstream ss(csvContent);

  try
  {
    rapidcsv::Document doc(ss, labelParams, separatorParams, converterParams, lineReaderParams);

    size_t opOffset = 0;
    const uint8_t* opData = Data;
    size_t opSize = Size;

    size_t colIdx1 = static_cast<size_t>(ReadU32(opData, opSize, opOffset) % 16U);
    std::vector<int> intCol1 = ReadIntVector(opData, opSize, opOffset, 32);

    std::string colName1 = ReadString(opData, opSize, opOffset, 32);
    if (colName1.empty())
    {
      colName1 = "A";
    }
    std::vector<std::string> strCol1 = ReadStringVector(opData, opSize, opOffset, 32, 32);

    size_t colIdx2 = static_cast<size_t>(ReadU32(opData, opSize, opOffset) % 16U);
    std::vector<int> intCol2 = ReadIntVector(opData, opSize, opOffset, 32);

    std::string getName1 = ReadString(opData, opSize, opOffset, 32);
    if (getName1.empty())
    {
      getName1 = colName1;
    }

    std::string savePath = "./dummy_file";

    try
    {
      doc.SetColumn<int>(colIdx1, intCol1);
    }
    catch (...)
    {
    }

    try
    {
      doc.SetColumn<std::string>(colName1, strCol1);
    }
    catch (...)
    {
    }

    try
    {
      doc.SetColumn<int>(colIdx2, intCol2);
    }
    catch (...)
    {
    }

    try
    {
      volatile auto v1 = doc.GetColumn<int>(colIdx1);
      (void)v1;
    }
    catch (...)
    {
    }

    try
    {
      volatile auto v2 = doc.GetColumn<std::string>(getName1);
      (void)v2;
    }
    catch (...)
    {
    }

    try
    {
      volatile auto v3 = doc.GetColumn<int>(colIdx2);
      (void)v3;
    }
    catch (...)
    {
    }

    try
    {
      doc.Save(savePath);
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