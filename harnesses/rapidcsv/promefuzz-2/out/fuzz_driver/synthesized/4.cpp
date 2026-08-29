// This fuzz driver is generated for library rapidcsv, aiming to fuzz the following functions:
// rapidcsv::Document::SetRow at rapidcsv.h:1024:10 in rapidcsv.h
// rapidcsv::Document::SetRow at rapidcsv.h:1062:10 in rapidcsv.h
// rapidcsv::Document::GetRow at rapidcsv.h:944:20 in rapidcsv.h
// rapidcsv::Document::GetRow at rapidcsv.h:991:20 in rapidcsv.h
// rapidcsv::Document::Save at rapidcsv.h:597:10 in rapidcsv.h
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
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>

#include "rapidcsv.h"

static size_t ReadSizeT(const uint8_t* Data, size_t Size, size_t& Offset)
{
  size_t value = 0;
  const size_t n = std::min(sizeof(size_t), Size - Offset);
  for (size_t i = 0; i < n; ++i)
  {
    value = (value << 8) | Data[Offset++];
  }
  return value;
}

static std::string ReadString(const uint8_t* Data, size_t Size, size_t& Offset)
{
  if (Offset >= Size)
  {
    return std::string();
  }

  size_t len = Data[Offset++];
  if (Offset + len > Size)
  {
    len = Size - Offset;
  }

  std::string s(reinterpret_cast<const char*>(Data + Offset), len);
  Offset += len;
  return s;
}

static std::vector<std::string> ReadStringVector(const uint8_t* Data, size_t Size, size_t& Offset)
{
  std::vector<std::string> out;
  if (Offset >= Size)
  {
    return out;
  }

  size_t count = Data[Offset++] % 16;
  out.reserve(count);
  for (size_t i = 0; i < count && Offset < Size; ++i)
  {
    out.push_back(ReadString(Data, Size, Offset));
  }
  return out;
}

extern "C" int LLVMFuzzerTestOneInput_4(const uint8_t *Data, size_t Size)
{
  std::ofstream ofs("./dummy_file", std::ios::binary);
  if (ofs)
  {
    ofs.write(reinterpret_cast<const char*>(Data), static_cast<std::streamsize>(Size));
  }
  ofs.close();

  size_t offset = 0;

  rapidcsv::LabelParams labelParams;
  labelParams.mColumnNameIdx = (Size > 0) ? static_cast<int>(static_cast<int8_t>(Data[offset++])) : 0;

  rapidcsv::SeparatorParams separatorParams;
  separatorParams.mSeparator = (offset < Size) ? static_cast<char>(Data[offset++]) : ',';
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

    size_t rowIdx = (offset < Size) ? (ReadSizeT(Data, Size, offset) % 32) : 0;
    std::vector<std::string> rowByIndex = ReadStringVector(Data, Size, offset);
    std::string rowName = ReadString(Data, Size, offset);
    std::vector<std::string> rowByName = ReadStringVector(Data, Size, offset);

    try
    {
      doc.SetRow<std::string>(rowIdx, rowByIndex);
    }
    catch (...)
    {
    }

    try
    {
      doc.SetRow<std::string>(rowName, rowByName);
    }
    catch (...)
    {
    }

    try
    {
      volatile auto r1 = doc.GetRow<std::string>(rowIdx);
      (void)r1;
    }
    catch (...)
    {
    }

    try
    {
      volatile auto r2 = doc.GetRow<std::string>(rowName);
      (void)r2;
    }
    catch (...)
    {
    }

    try
    {
      if (offset < Size && (Data[offset] & 1))
      {
        doc.Save("./dummy_file");
      }
      else
      {
        doc.Save();
      }
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