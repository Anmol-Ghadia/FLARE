// This fuzz driver is generated for library rapidcsv, aiming to fuzz the following functions:
// rapidcsv::Document::RemoveRow at rapidcsv.h:1076:10 in rapidcsv.h
// rapidcsv::Document::RemoveRow at rapidcsv.h:1097:10 in rapidcsv.h
// rapidcsv::Document::Save at rapidcsv.h:597:10 in rapidcsv.h
// rapidcsv::Document::RemoveRow at rapidcsv.h:1076:10 in rapidcsv.h
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

static size_t GetSizeT(const uint8_t* data, size_t size, size_t offset)
{
  size_t value = 0;
  for (size_t i = 0; i < sizeof(size_t); ++i)
  {
    value <<= 8;
    if ((offset + i) < size)
    {
      value |= static_cast<size_t>(data[offset + i]);
    }
  }
  return value;
}

static std::string GetStringSlice(const uint8_t* data, size_t size, size_t offset, size_t length)
{
  if (offset >= size)
  {
    return std::string();
  }

  const size_t maxLen = size - offset;
  if (length > maxLen)
  {
    length = maxLen;
  }

  return std::string(reinterpret_cast<const char*>(data + offset), length);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
  if (Data == nullptr)
  {
    return 0;
  }

  rapidcsv::LabelParams labelParams;
  rapidcsv::SeparatorParams separatorParams;
  rapidcsv::ConverterParams converterParams;
  rapidcsv::LineReaderParams lineReaderParams;

  if (Size > 0)
  {
    labelParams.mColumnNameIdx = static_cast<int>(Data[0] % 4) - 1;
  }

  if (Size > 1)
  {
    separatorParams.mSeparator = static_cast<char>(Data[1]);
    if (separatorParams.mSeparator == '\0' || separatorParams.mSeparator == '\n' || separatorParams.mSeparator == '\r')
    {
      separatorParams.mSeparator = ',';
    }
  }

  if (Size > 2)
  {
    separatorParams.mTrim = (Data[2] & 1) != 0;
  }

  if (Size > 3)
  {
    converterParams.mHasDefaultConverter = (Data[3] & 1) != 0;
  }

  if (Size > 4)
  {
    converterParams.mDefaultFloat = static_cast<long double>(static_cast<int8_t>(Data[4]));
  }

  if (Size > 5)
  {
    converterParams.mDefaultInteger = static_cast<long long>(static_cast<int8_t>(Data[5]));
  }

  if (Size > 6)
  {
    lineReaderParams.mSkipCommentLines = (Data[6] & 1) != 0;
  }

  if (Size > 7)
  {
    lineReaderParams.mCommentPrefix = static_cast<char>(Data[7]);
    if (lineReaderParams.mCommentPrefix == '\0')
    {
      lineReaderParams.mCommentPrefix = '#';
    }
  }

  std::string csv(reinterpret_cast<const char*>(Data), Size);
  std::stringstream ss(csv, std::ios::in | std::ios::binary);

  try
  {
    rapidcsv::Document doc(ss, labelParams, separatorParams, converterParams, lineReaderParams);

    size_t idx1 = 0;
    size_t idx2 = 0;
    if (Size > 8)
    {
      idx1 = GetSizeT(Data, Size, 8);
    }
    if (Size > 8 + sizeof(size_t))
    {
      idx2 = GetSizeT(Data, Size, 8 + sizeof(size_t));
    }

    size_t nameOffset = 8 + 2 * sizeof(size_t);
    size_t nameLen = 0;
    if (Size > nameOffset)
    {
      nameLen = static_cast<size_t>(Data[nameOffset]) % (Size - nameOffset);
    }
    std::string rowName = GetStringSlice(Data, Size, nameOffset + 1, nameLen);

    try
    {
      doc.RemoveRow(idx1);
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
      std::ofstream out("./dummy_file", std::ios::binary);
      out.write(csv.data(), static_cast<std::streamsize>(csv.size()));
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

    try
    {
      doc.RemoveRow(idx2);
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