// This fuzz driver is generated for library rapidcsv, aiming to fuzz the following functions:
// rapidcsv::Converter::ToVal at rapidcsv.h:184:10 in rapidcsv.h
// rapidcsv::Converter::ToStr at rapidcsv.h:132:10 in rapidcsv.h
// rapidcsv::Converter::ToVal at rapidcsv.h:184:10 in rapidcsv.h
// rapidcsv::Converter::ToStr at rapidcsv.h:132:10 in rapidcsv.h
// rapidcsv::Converter::ToVal at rapidcsv.h:184:10 in rapidcsv.h
// rapidcsv::Converter::ToStr at rapidcsv.h:132:10 in rapidcsv.h
// rapidcsv::Converter::ToVal at rapidcsv.h:184:10 in rapidcsv.h
// rapidcsv::Converter::ToStr at rapidcsv.h:132:10 in rapidcsv.h
// rapidcsv::Converter<std::basic_string<char>>::ToStr at rapidcsv.h:318:39 in rapidcsv.h
// rapidcsv::Document::SetRow at rapidcsv.h:1062:10 in rapidcsv.h
// rapidcsv::Document::SetRow at rapidcsv.h:1062:10 in rapidcsv.h
// rapidcsv::Document::SetCell at rapidcsv.h:1367:10 in rapidcsv.h
// rapidcsv::Document::SetCell at rapidcsv.h:1367:10 in rapidcsv.h
// rapidcsv::Document::SetCell at rapidcsv.h:1367:10 in rapidcsv.h
// rapidcsv::Document::SetColumn at rapidcsv.h:784:10 in rapidcsv.h
// rapidcsv::Document::SetColumn at rapidcsv.h:784:10 in rapidcsv.h
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
#include <sstream>
#include <fstream>
#include <limits>
#include "rapidcsv.h"

static uint32_t ReadU32(const uint8_t*& p, size_t& remaining)
{
  uint32_t v = 0;
  for (int i = 0; i < 4 && remaining > 0; ++i)
  {
    v = (v << 8) | *p++;
    --remaining;
  }
  return v;
}

static bool ReadBool(const uint8_t*& p, size_t& remaining)
{
  if (remaining == 0) return false;
  bool v = ((*p++) & 1) != 0;
  --remaining;
  return v;
}

static char ReadChar(const uint8_t*& p, size_t& remaining, char fallback)
{
  if (remaining == 0) return fallback;
  char c = static_cast<char>(*p++);
  --remaining;
  return c;
}

static std::string ReadString(const uint8_t*& p, size_t& remaining)
{
  if (remaining == 0) return std::string();
  uint32_t len = ReadU32(p, remaining);
  if (remaining == 0) return std::string();
  len %= static_cast<uint32_t>(remaining + 1);
  size_t actual = static_cast<size_t>(len);
  std::string s(reinterpret_cast<const char*>(p), actual);
  p += actual;
  remaining -= actual;
  return s;
}

static std::vector<int> ReadIntVector(const uint8_t*& p, size_t& remaining)
{
  std::vector<int> out;
  uint32_t count = ReadU32(p, remaining) % 32;
  out.reserve(count);
  for (uint32_t i = 0; i < count && remaining > 0; ++i)
  {
    uint32_t raw = ReadU32(p, remaining);
    out.push_back(static_cast<int>(raw));
  }
  return out;
}

static std::vector<std::string> ReadStringVector(const uint8_t*& p, size_t& remaining)
{
  std::vector<std::string> out;
  uint32_t count = ReadU32(p, remaining) % 32;
  out.reserve(count);
  for (uint32_t i = 0; i < count && remaining > 0; ++i)
  {
    out.push_back(ReadString(p, remaining));
  }
  return out;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
  const uint8_t* p = Data;
  size_t remaining = Size;

  rapidcsv::LabelParams labelParams;
  labelParams.mColumnNameIdx = static_cast<int>(ReadU32(p, remaining) % 4) - 1;

  rapidcsv::SeparatorParams separatorParams;
  separatorParams.mSeparator = ReadChar(p, remaining, ',');
  if (separatorParams.mSeparator == '\0' || separatorParams.mSeparator == '\n' || separatorParams.mSeparator == '\r')
  {
    separatorParams.mSeparator = ',';
  }
  separatorParams.mTrim = ReadBool(p, remaining);

  rapidcsv::ConverterParams converterParams;
  converterParams.mHasDefaultConverter = ReadBool(p, remaining);
  converterParams.mDefaultFloat = static_cast<long double>(static_cast<int64_t>(ReadU32(p, remaining)));
  converterParams.mDefaultInteger = static_cast<long long>(static_cast<int64_t>(ReadU32(p, remaining)));

  rapidcsv::LineReaderParams lineReaderParams;
  lineReaderParams.mSkipCommentLines = ReadBool(p, remaining);
  lineReaderParams.mCommentPrefix = ReadChar(p, remaining, '#');

  std::string csvData = ReadString(p, remaining);
  std::ofstream ofs("./dummy_file", std::ios::binary);
  ofs.write(csvData.data(), static_cast<std::streamsize>(csvData.size()));
  ofs.close();

  std::stringstream ss;
  ss.write(csvData.data(), static_cast<std::streamsize>(csvData.size()));

  try
  {
    rapidcsv::Document doc(ss, labelParams, separatorParams, converterParams, lineReaderParams);

    rapidcsv::Converter<int> convInt(converterParams);
    rapidcsv::Converter<long long> convLL(converterParams);
    rapidcsv::Converter<unsigned char> convUChar(converterParams);
    rapidcsv::Converter<double> convDouble(converterParams);
    rapidcsv::Converter<std::string> convString(converterParams);

    std::string s1 = ReadString(p, remaining);
    std::string s2 = ReadString(p, remaining);
    std::string s3 = ReadString(p, remaining);

    try
    {
      int v = 0;
      convInt.ToVal(s1, v);
      std::string out;
      convInt.ToStr(v, out);
    }
    catch (...) {}

    try
    {
      long long v = 0;
      convLL.ToVal(s2, v);
      std::string out;
      convLL.ToStr(v, out);
    }
    catch (...) {}

    try
    {
      unsigned char v = 0;
      convUChar.ToVal(s3, v);
      std::string out;
      convUChar.ToStr(v, out);
    }
    catch (...) {}

    try
    {
      double v = 0.0;
      convDouble.ToVal(s1, v);
      std::string out;
      convDouble.ToStr(v, out);
    }
    catch (...) {}

    try
    {
      std::string out;
      std::string in = ReadString(p, remaining);
      convString.ToStr(in, out);
    }
    catch (...) {}

    std::string rowName = ReadString(p, remaining);
    std::string colName = ReadString(p, remaining);
    std::string otherRowName = ReadString(p, remaining);
    std::string otherColName = ReadString(p, remaining);

    std::vector<int> intRow = ReadIntVector(p, remaining);
    std::vector<std::string> strRow = ReadStringVector(p, remaining);
    std::vector<int> intCol = ReadIntVector(p, remaining);
    std::vector<std::string> strCol = ReadStringVector(p, remaining);

    try
    {
      doc.SetRow(rowName, intRow);
    }
    catch (...) {}

    try
    {
      doc.SetRow(otherRowName, strRow);
    }
    catch (...) {}

    try
    {
      int cellVal = static_cast<int>(ReadU32(p, remaining));
      doc.SetCell(colName, rowName, cellVal);
    }
    catch (...) {}

    try
    {
      std::string cellStr = ReadString(p, remaining);
      doc.SetCell(otherColName, otherRowName, cellStr);
    }
    catch (...) {}

    try
    {
      double cellDouble = static_cast<double>(static_cast<int32_t>(ReadU32(p, remaining))) / 3.0;
      doc.SetCell(colName, otherRowName, cellDouble);
    }
    catch (...) {}

    try
    {
      doc.SetColumn(colName, intCol);
    }
    catch (...) {}

    try
    {
      doc.SetColumn(otherColName, strCol);
    }
    catch (...) {}
  }
  catch (...) {}

  return 0;
}