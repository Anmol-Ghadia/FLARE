// This fuzz driver is generated for library rapidcsv, aiming to fuzz the following functions:
// rapidcsv::Converter::ToVal at rapidcsv.h:184:10 in rapidcsv.h
// rapidcsv::Converter::ToVal at rapidcsv.h:184:10 in rapidcsv.h
// rapidcsv::Converter::ToVal at rapidcsv.h:184:10 in rapidcsv.h
// rapidcsv::Converter::ToVal at rapidcsv.h:184:10 in rapidcsv.h
// rapidcsv::Converter::ToVal at rapidcsv.h:184:10 in rapidcsv.h
// rapidcsv::Converter<std::basic_string<char>>::ToVal at rapidcsv.h:329:39 in rapidcsv.h
// rapidcsv::Converter::ToVal at rapidcsv.h:184:10 in rapidcsv.h
// rapidcsv::Converter::ToVal at rapidcsv.h:184:10 in rapidcsv.h
// rapidcsv::Converter<std::basic_string<char>>::ToVal at rapidcsv.h:329:39 in rapidcsv.h
// rapidcsv::Converter::ToStr at rapidcsv.h:132:10 in rapidcsv.h
// rapidcsv::Converter::ToStr at rapidcsv.h:132:10 in rapidcsv.h
// rapidcsv::Converter::ToStr at rapidcsv.h:132:10 in rapidcsv.h
// rapidcsv::Converter::ToStr at rapidcsv.h:132:10 in rapidcsv.h
// rapidcsv::Converter::ToStr at rapidcsv.h:132:10 in rapidcsv.h
// rapidcsv::Converter<std::basic_string<char>>::ToStr at rapidcsv.h:318:39 in rapidcsv.h
// rapidcsv::Document::SetRow at rapidcsv.h:1062:10 in rapidcsv.h
// rapidcsv::Document::SetRow at rapidcsv.h:1062:10 in rapidcsv.h
// rapidcsv::Document::SetRow at rapidcsv.h:1062:10 in rapidcsv.h
// rapidcsv::Document::SetRow at rapidcsv.h:1062:10 in rapidcsv.h
// rapidcsv::Document::SetColumn at rapidcsv.h:784:10 in rapidcsv.h
// rapidcsv::Document::SetColumn at rapidcsv.h:784:10 in rapidcsv.h
// rapidcsv::Document::SetColumn at rapidcsv.h:784:10 in rapidcsv.h
// rapidcsv::Document::SetColumn at rapidcsv.h:784:10 in rapidcsv.h
// rapidcsv::Document::SetCell at rapidcsv.h:1367:10 in rapidcsv.h
// rapidcsv::Document::SetCell at rapidcsv.h:1367:10 in rapidcsv.h
// rapidcsv::Document::SetCell at rapidcsv.h:1367:10 in rapidcsv.h
// rapidcsv::Document::SetCell at rapidcsv.h:1367:10 in rapidcsv.h
// rapidcsv::Document::SetCell at rapidcsv.h:1367:10 in rapidcsv.h
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <cstddef>
#include <fstream>
#include <algorithm>
#include <type_traits>
#include "rapidcsv.h"

static uint8_t GetByte(const uint8_t* Data, size_t Size, size_t& Offset)
{
  if (Offset >= Size) return 0;
  return Data[Offset++];
}

static bool GetBool(const uint8_t* Data, size_t Size, size_t& Offset)
{
  return (GetByte(Data, Size, Offset) & 1) != 0;
}

static int GetInt(const uint8_t* Data, size_t Size, size_t& Offset)
{
  int v = 0;
  for (int i = 0; i < 4; ++i)
  {
    v = (v << 8) | GetByte(Data, Size, Offset);
  }
  return v;
}

static long long GetLongLong(const uint8_t* Data, size_t Size, size_t& Offset)
{
  long long v = 0;
  for (int i = 0; i < 8; ++i)
  {
    v = (v << 8) | static_cast<long long>(GetByte(Data, Size, Offset));
  }
  return v;
}

static std::string GetString(const uint8_t* Data, size_t Size, size_t& Offset, size_t MaxLen)
{
  if (Offset >= Size)
  {
    return std::string();
  }

  const size_t available_before_len = Size - Offset;
  if (available_before_len == 0)
  {
    return std::string();
  }

  const size_t rawLen = GetByte(Data, Size, Offset);
  const size_t remaining = (Offset < Size) ? (Size - Offset) : 0;
  const size_t len = std::min(std::min(rawLen, MaxLen), remaining);

  if (len == 0)
  {
    return std::string();
  }

  std::string s(reinterpret_cast<const char*>(Data + Offset), len);
  Offset += len;
  return s;
}

template <typename T>
static std::vector<T> MakeVectorNumeric(const uint8_t* Data, size_t Size, size_t& Offset, size_t MaxCount)
{
  size_t count = std::min<size_t>(GetByte(Data, Size, Offset), MaxCount);
  std::vector<T> out;
  out.reserve(count);
  for (size_t i = 0; i < count; ++i)
  {
    if constexpr (std::is_same<T, int>::value)
    {
      out.push_back(GetInt(Data, Size, Offset));
    }
    else if constexpr (std::is_same<T, long long>::value)
    {
      out.push_back(GetLongLong(Data, Size, Offset));
    }
    else if constexpr (std::is_same<T, double>::value)
    {
      int a = GetInt(Data, Size, Offset);
      int b = GetInt(Data, Size, Offset);
      double d = static_cast<double>(a) / static_cast<double>((b == 0) ? 1 : b);
      out.push_back(d);
    }
  }
  return out;
}

static std::vector<std::string> MakeVectorString(const uint8_t* Data, size_t Size, size_t& Offset, size_t MaxCount)
{
  size_t count = std::min<size_t>(GetByte(Data, Size, Offset), MaxCount);
  std::vector<std::string> out;
  out.reserve(count);
  for (size_t i = 0; i < count; ++i)
  {
    out.push_back(GetString(Data, Size, Offset, 32));
  }
  return out;
}

extern "C" int LLVMFuzzerTestOneInput_18(const uint8_t *Data, size_t Size)
{
  size_t Offset = 0;

  rapidcsv::LabelParams labelParams;
  labelParams.mColumnNameIdx = (GetInt(Data, Size, Offset) % 5) - 2;

  rapidcsv::SeparatorParams separatorParams;
  separatorParams.mSeparator = static_cast<char>(GetByte(Data, Size, Offset));
  if (separatorParams.mSeparator == '\0')
    separatorParams.mSeparator = ',';
  separatorParams.mTrim = GetBool(Data, Size, Offset);

  rapidcsv::ConverterParams converterParams;
  converterParams.mHasDefaultConverter = GetBool(Data, Size, Offset);
  converterParams.mDefaultInteger = GetLongLong(Data, Size, Offset);
  converterParams.mDefaultFloat = static_cast<long double>(GetInt(Data, Size, Offset));

  rapidcsv::LineReaderParams lineReaderParams;
  lineReaderParams.mSkipCommentLines = GetBool(Data, Size, Offset);
  lineReaderParams.mCommentPrefix = static_cast<char>(GetByte(Data, Size, Offset));
  if (lineReaderParams.mCommentPrefix == '\0')
    lineReaderParams.mCommentPrefix = '#';

  std::string fileContent;
  if (Offset < Size)
  {
    fileContent.assign(reinterpret_cast<const char*>(Data + Offset), Size - Offset);
  }

  {
    std::ofstream ofs("./dummy_file", std::ios::binary);
    ofs.write(fileContent.data(), static_cast<std::streamsize>(fileContent.size()));
  }

  try
  {
    rapidcsv::Converter<int> convInt(converterParams);
    rapidcsv::Converter<long long> convLL(converterParams);
    rapidcsv::Converter<short> convShort(converterParams);
    rapidcsv::Converter<unsigned char> convUChar(converterParams);
    rapidcsv::Converter<double> convDouble(converterParams);
    rapidcsv::Converter<std::string> convString(converterParams);

    std::string s1 = fileContent.substr(0, std::min<size_t>(fileContent.size(), 32));

    size_t convOffset = 0;
    std::string s2 = GetString(Data, Size, convOffset, 32);

    std::string outStr;
    int vi = 0;
    long long vll = 0;
    short vs = 0;
    unsigned char vuc = 0;
    double vd = 0.0;
    std::string vstr;

    try { convInt.ToVal(s1, vi); } catch (...) {}
    try { convLL.ToVal(s1, vll); } catch (...) {}
    try { convShort.ToVal(s1, vs); } catch (...) {}
    try { convUChar.ToVal(s1, vuc); } catch (...) {}
    try { convDouble.ToVal(s1, vd); } catch (...) {}
    try { convString.ToVal(s1, vstr); } catch (...) {}

    try { convInt.ToVal(s2, vi); } catch (...) {}
    try { convDouble.ToVal(s2, vd); } catch (...) {}
    try { convString.ToVal(s2, vstr); } catch (...) {}

    try { convInt.ToStr(vi, outStr); } catch (...) {}
    try { convLL.ToStr(vll, outStr); } catch (...) {}
    try { convShort.ToStr(vs, outStr); } catch (...) {}
    try { convUChar.ToStr(vuc, outStr); } catch (...) {}
    try { convDouble.ToStr(vd, outStr); } catch (...) {}
    try { convString.ToStr(vstr, outStr); } catch (...) {}

    rapidcsv::Document doc("./dummy_file", labelParams, separatorParams, converterParams, lineReaderParams);

    size_t opOffset = 0;
    std::string rowName1 = GetString(Data, Size, opOffset, 24);
    std::string rowName2 = GetString(Data, Size, opOffset, 24);
    std::string colName1 = GetString(Data, Size, opOffset, 24);
    std::string colName2 = GetString(Data, Size, opOffset, 24);

    auto vecInt = MakeVectorNumeric<int>(Data, Size, opOffset, 16);
    auto vecLL = MakeVectorNumeric<long long>(Data, Size, opOffset, 16);
    auto vecDouble = MakeVectorNumeric<double>(Data, Size, opOffset, 16);
    auto vecString = MakeVectorString(Data, Size, opOffset, 16);

    try { doc.SetRow(rowName1, vecInt); } catch (...) {}
    try { doc.SetRow(rowName2, vecLL); } catch (...) {}
    try { doc.SetRow(rowName1, vecDouble); } catch (...) {}
    try { doc.SetRow(rowName2, vecString); } catch (...) {}

    try { doc.SetColumn(colName1, vecInt); } catch (...) {}
    try { doc.SetColumn(colName2, vecLL); } catch (...) {}
    try { doc.SetColumn(colName1, vecDouble); } catch (...) {}
    try { doc.SetColumn(colName2, vecString); } catch (...) {}

    try { doc.SetCell(colName1, rowName1, vi); } catch (...) {}
    try { doc.SetCell(colName2, rowName1, vll); } catch (...) {}
    try { doc.SetCell(colName1, rowName2, vd); } catch (...) {}
    try { doc.SetCell(colName2, rowName2, vstr); } catch (...) {}
    try { doc.SetCell(std::string(), std::string(), outStr); } catch (...) {}
  }
  catch (...)
  {
  }

  return 0;
}