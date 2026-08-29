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

static uint32_t ReadU32(const uint8_t*& data, size_t& size)
{
  if (size < 4) {
    uint32_t v = 0;
    while (size > 0) {
      v = (v << 8) | *data++;
      --size;
    }
    return v;
  }
  uint32_t v = (static_cast<uint32_t>(data[0]) << 24) |
               (static_cast<uint32_t>(data[1]) << 16) |
               (static_cast<uint32_t>(data[2]) << 8) |
               static_cast<uint32_t>(data[3]);
  data += 4;
  size -= 4;
  return v;
}

static bool ReadBool(const uint8_t*& data, size_t& size)
{
  if (size == 0) {
    return false;
  }
  bool v = ((*data) & 1) != 0;
  ++data;
  --size;
  return v;
}

static char ReadChar(const uint8_t*& data, size_t& size, char fallback)
{
  if (size == 0) {
    return fallback;
  }
  char c = static_cast<char>(*data++);
  --size;
  return c;
}

static std::string ReadString(const uint8_t*& data, size_t& size, size_t maxLen)
{
  if (size == 0) {
    return std::string();
  }
  size_t len = ReadU32(data, size) % (maxLen + 1);
  len = std::min(len, size);
  std::string out(reinterpret_cast<const char*>(data), len);
  data += len;
  size -= len;
  return out;
}

static std::vector<int> ReadIntVector(const uint8_t*& data, size_t& size, size_t maxCount)
{
  std::vector<int> out;
  size_t count = 0;
  if (size > 0) {
    count = ReadU32(data, size) % (maxCount + 1);
  }
  out.reserve(count);
  for (size_t i = 0; i < count; ++i) {
    out.push_back(static_cast<int>(ReadU32(data, size)));
  }
  return out;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* Data, size_t Size)
{
  const uint8_t* data = Data;
  size_t size = Size;

  std::string initialCsv = ReadString(data, size, 4096);
  {
    std::ofstream ofs("./dummy_file", std::ios::binary);
    ofs.write(initialCsv.data(), static_cast<std::streamsize>(initialCsv.size()));
  }

  rapidcsv::LabelParams labelParams;
  labelParams.mColumnNameIdx = static_cast<int>(ReadU32(data, size) % 4) - 1;

  rapidcsv::SeparatorParams separatorParams;
  separatorParams.mSeparator = ReadChar(data, size, ',');
  if (separatorParams.mSeparator == '\0' || separatorParams.mSeparator == '\n' ||
      separatorParams.mSeparator == '\r' || separatorParams.mSeparator == '"') {
    separatorParams.mSeparator = ',';
  }
  separatorParams.mTrim = ReadBool(data, size);

  rapidcsv::ConverterParams converterParams;
  converterParams.mHasDefaultConverter = ReadBool(data, size);
  converterParams.mDefaultFloat = static_cast<long double>(static_cast<int>(ReadU32(data, size)));
  converterParams.mDefaultInteger = static_cast<long long>(static_cast<int>(ReadU32(data, size)));

  rapidcsv::LineReaderParams lineReaderParams;
  lineReaderParams.mSkipCommentLines = ReadBool(data, size);
  lineReaderParams.mCommentPrefix = ReadChar(data, size, '#');
  if (lineReaderParams.mCommentPrefix == '\0') {
    lineReaderParams.mCommentPrefix = '#';
  }

  try {
    rapidcsv::Document doc("./dummy_file", labelParams, separatorParams, converterParams, lineReaderParams);

    std::string colName1 = ReadString(data, size, 64);
    std::string colName2 = ReadString(data, size, 64);
    size_t colIdx1 = static_cast<size_t>(ReadU32(data, size) % 16);
    size_t colIdx2 = static_cast<size_t>(ReadU32(data, size) % 16);

    std::vector<int> vec1 = ReadIntVector(data, size, 32);
    std::vector<int> vec2 = ReadIntVector(data, size, 32);
    std::vector<int> vec3 = ReadIntVector(data, size, 32);

    try {
      doc.SetColumn<int>(colName1, vec1);
    } catch (...) {
    }

    try {
      doc.SetColumn<int>(colIdx1, vec2);
    } catch (...) {
    }

    try {
      doc.SetColumn<int>(colName2, vec3);
    } catch (...) {
    }

    try {
      (void)doc.GetColumn<int>(colName1);
    } catch (...) {
    }

    try {
      (void)doc.GetColumn<int>(colIdx2);
    } catch (...) {
    }

    try {
      (void)doc.GetColumn<int>(colName2);
    } catch (...) {
    }

    try {
      doc.Save("./dummy_file");
    } catch (...) {
    }
  } catch (...) {
  }

  return 0;
}