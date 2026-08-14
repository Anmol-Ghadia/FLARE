// This fuzz driver is generated for library rapidcsv, aiming to fuzz the following functions:
// rapidcsv::Document::SetColumn at rapidcsv.h:746:10 in rapidcsv.h
// rapidcsv::Document::SetColumn at rapidcsv.h:784:10 in rapidcsv.h
// rapidcsv::Document::SetColumn at rapidcsv.h:746:10 in rapidcsv.h
// rapidcsv::Document::GetColumn at rapidcsv.h:654:20 in rapidcsv.h
// rapidcsv::Document::GetColumn at rapidcsv.h:713:20 in rapidcsv.h
// rapidcsv::Document::GetColumn at rapidcsv.h:654:20 in rapidcsv.h
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
#include <vector>
#include <fstream>
#include <algorithm>

#include "rapidcsv.h"

static uint32_t ReadU32(const uint8_t* data, size_t size, size_t& offset)
{
  uint32_t v = 0;
  for (int i = 0; i < 4; ++i)
  {
    v <<= 8;
    if (offset < size)
    {
      v |= data[offset++];
    }
  }
  return v;
}

static std::string ReadString(const uint8_t* data, size_t size, size_t& offset, size_t maxLen)
{
  if (offset >= size)
  {
    return std::string();
  }

  size_t remaining = size - offset;
  size_t len = ReadU32(data, size, offset);
  len = std::min(len, maxLen);
  len = std::min(len, size - offset);

  std::string s(reinterpret_cast<const char*>(data + offset), len);
  offset += len;
  return s;
}

static std::vector<std::string> ReadStringVector(const uint8_t* data, size_t size, size_t& offset, size_t maxCount, size_t maxStrLen)
{
  std::vector<std::string> out;
  size_t count = ReadU32(data, size, offset) % (maxCount + 1);
  out.reserve(count);
  for (size_t i = 0; i < count && offset < size; ++i)
  {
    out.push_back(ReadString(data, size, offset, maxStrLen));
  }
  return out;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
  {
    std::ofstream seed("./dummy_file", std::ios::binary);
    if (seed)
    {
      seed.write(reinterpret_cast<const char*>(Data), static_cast<std::streamsize>(Size));
    }
  }

  size_t offset = 0;

  rapidcsv::LabelParams labelParams;
  labelParams.mColumnNameIdx = (offset < Size) ? static_cast<int>(static_cast<int8_t>(Data[offset++])) : 0;

  rapidcsv::SeparatorParams separatorParams;
  separatorParams.mSeparator = (offset < Size) ? static_cast<char>(Data[offset++]) : ',';
  if (separatorParams.mSeparator == '\0' || separatorParams.mSeparator == '\n' || separatorParams.mSeparator == '\r')
  {
    separatorParams.mSeparator = ',';
  }
  separatorParams.mTrim = (offset < Size) ? ((Data[offset++] & 1) != 0) : false;

  rapidcsv::ConverterParams converterParams;
  converterParams.mHasDefaultConverter = (offset < Size) ? ((Data[offset++] & 1) != 0) : false;
  converterParams.mDefaultFloat = (offset < Size) ? static_cast<long double>(static_cast<int8_t>(Data[offset++])) : 0.0L;
  converterParams.mDefaultInteger = (offset < Size) ? static_cast<long long>(static_cast<int8_t>(Data[offset++])) : 0LL;

  rapidcsv::LineReaderParams lineReaderParams;
  lineReaderParams.mSkipCommentLines = (offset < Size) ? ((Data[offset++] & 1) != 0) : false;
  lineReaderParams.mCommentPrefix = (offset < Size) ? static_cast<char>(Data[offset++]) : '#';

  try
  {
    rapidcsv::Document doc("./dummy_file", labelParams, separatorParams, converterParams, lineReaderParams);

    std::vector<std::string> colByIndex1 = ReadStringVector(Data, Size, offset, 16, 64);
    std::vector<std::string> colByName = ReadStringVector(Data, Size, offset, 16, 64);
    std::vector<std::string> colByIndex2 = ReadStringVector(Data, Size, offset, 16, 64);

    size_t idx1 = (offset < Size) ? static_cast<size_t>(Data[offset++]) : 0;
    std::string name = ReadString(Data, Size, offset, 64);
    if (name.empty())
    {
      name = "col";
    }
    size_t idx2 = (offset < Size) ? static_cast<size_t>(Data[offset++]) : 1;

    try
    {
      doc.SetColumn<std::string>(idx1, colByIndex1);
    }
    catch (...)
    {
    }

    try
    {
      doc.SetColumn<std::string>(name, colByName);
    }
    catch (...)
    {
    }

    try
    {
      doc.SetColumn<std::string>(idx2, colByIndex2);
    }
    catch (...)
    {
    }

    try
    {
      (void)doc.GetColumn<std::string>(idx1);
    }
    catch (...)
    {
    }

    try
    {
      (void)doc.GetColumn<std::string>(name);
    }
    catch (...)
    {
    }

    try
    {
      (void)doc.GetColumn<std::string>(idx2);
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