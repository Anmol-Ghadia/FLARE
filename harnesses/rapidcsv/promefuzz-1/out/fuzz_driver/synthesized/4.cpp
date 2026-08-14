// This fuzz driver is generated for library rapidcsv, aiming to fuzz the following functions:
// rapidcsv::Document::SetRow at rapidcsv.h:1024:10 in rapidcsv.h
// rapidcsv::Document::SetRow at rapidcsv.h:1062:10 in rapidcsv.h
// rapidcsv::Document::GetRow at rapidcsv.h:944:20 in rapidcsv.h
// rapidcsv::Document::GetRow at rapidcsv.h:991:20 in rapidcsv.h
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

static std::vector<std::string> ReadStringVector(const uint8_t* data, size_t size, size_t& offset)
{
  std::vector<std::string> out;
  if (offset >= size) return out;
  size_t count = ReadSizeT(data, size, offset) % 32;
  out.reserve(count);
  for (size_t i = 0; i < count && offset < size; ++i)
  {
    out.push_back(ReadString(data, size, offset));
  }
  return out;
}

extern "C" int LLVMFuzzerTestOneInput_4(const uint8_t *Data, size_t Size)
{
  size_t offset = 0;

  rapidcsv::LabelParams labelParams;
  labelParams.mColumnNameIdx = static_cast<int>(ReadSizeT(Data, Size, offset) % 4);

  rapidcsv::SeparatorParams separatorParams;
  separatorParams.mSeparator = ReadChar(Data, Size, offset, ',');
  if (separatorParams.mSeparator == '\0' || separatorParams.mSeparator == '\n' || separatorParams.mSeparator == '\r')
  {
    separatorParams.mSeparator = ',';
  }
  separatorParams.mTrim = ReadBool(Data, Size, offset);

  rapidcsv::ConverterParams converterParams;
  converterParams.mHasDefaultConverter = ReadBool(Data, Size, offset);
  converterParams.mDefaultFloat = static_cast<long double>(ReadSizeT(Data, Size, offset));
  converterParams.mDefaultInteger = static_cast<long long>(ReadSizeT(Data, Size, offset));

  rapidcsv::LineReaderParams lineReaderParams;
  lineReaderParams.mSkipCommentLines = ReadBool(Data, Size, offset);
  lineReaderParams.mCommentPrefix = ReadChar(Data, Size, offset, '#');

  std::string initialContent;
  if (offset < Size)
  {
    initialContent.assign(reinterpret_cast<const char*>(Data + offset), Size - offset);
  }

  {
    std::ofstream ofs("./dummy_file", std::ios::binary);
    ofs.write(initialContent.data(), static_cast<std::streamsize>(initialContent.size()));
  }

  try
  {
    rapidcsv::Document doc("./dummy_file", labelParams, separatorParams, converterParams, lineReaderParams);

    size_t localOffset = 0;
    const uint8_t* localData = reinterpret_cast<const uint8_t*>(initialContent.data());
    const size_t localSize = initialContent.size();

    size_t rowIdx = ReadSizeT(localData, localSize, localOffset) % 16;
    std::vector<std::string> rowByIdx = ReadStringVector(localData, localSize, localOffset);
    std::string rowName = ReadString(localData, localSize, localOffset);
    std::vector<std::string> rowByName = ReadStringVector(localData, localSize, localOffset);

    try
    {
      doc.SetRow<std::string>(rowIdx, rowByIdx);
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