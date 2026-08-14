// This fuzz driver is generated for library rapidcsv, aiming to fuzz the following functions:
// rapidcsv::Converter::ToStr at rapidcsv.h:132:10 in rapidcsv.h
// rapidcsv::Converter::ToStr at rapidcsv.h:132:10 in rapidcsv.h
// rapidcsv::Converter::ToStr at rapidcsv.h:132:10 in rapidcsv.h
// rapidcsv::Converter::ToStr at rapidcsv.h:132:10 in rapidcsv.h
// rapidcsv::Converter<std::basic_string<char>>::ToStr at rapidcsv.h:318:39 in rapidcsv.h
// rapidcsv::Document::SetColumn at rapidcsv.h:784:10 in rapidcsv.h
// rapidcsv::Document::SetColumn at rapidcsv.h:784:10 in rapidcsv.h
// rapidcsv::Document::SetColumn at rapidcsv.h:784:10 in rapidcsv.h
// rapidcsv::Document::SetColumn at rapidcsv.h:784:10 in rapidcsv.h
// rapidcsv::Document::SetCell at rapidcsv.h:1367:10 in rapidcsv.h
// rapidcsv::Document::SetCell at rapidcsv.h:1367:10 in rapidcsv.h
// rapidcsv::Document::SetCell at rapidcsv.h:1367:10 in rapidcsv.h
// rapidcsv::Document::InsertColumn at rapidcsv.h:845:10 in rapidcsv.h
// rapidcsv::Document::InsertColumn at rapidcsv.h:845:10 in rapidcsv.h
// rapidcsv::Document::SetRow at rapidcsv.h:1062:10 in rapidcsv.h
// rapidcsv::Document::SetRow at rapidcsv.h:1062:10 in rapidcsv.h
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
#include <algorithm>
#include <limits>

#include "rapidcsv.h"

static uint8_t GetByte(const uint8_t* Data, size_t Size, size_t& Off)
{
  if (Off >= Size) return 0;
  return Data[Off++];
}

static bool GetBool(const uint8_t* Data, size_t Size, size_t& Off)
{
  return (GetByte(Data, Size, Off) & 1u) != 0;
}

static int GetInt(const uint8_t* Data, size_t Size, size_t& Off)
{
  int v = 0;
  for (int i = 0; i < 4; ++i)
  {
    v = (v << 8) | GetByte(Data, Size, Off);
  }
  return v;
}

static long long GetLongLong(const uint8_t* Data, size_t Size, size_t& Off)
{
  long long v = 0;
  for (int i = 0; i < 8; ++i)
  {
    v = (v << 8) | static_cast<long long>(GetByte(Data, Size, Off));
  }
  return v;
}

static size_t GetSizeT(const uint8_t* Data, size_t Size, size_t& Off, size_t Mod)
{
  if (Mod == 0) return 0;
  return static_cast<size_t>(GetLongLong(Data, Size, Off)) % Mod;
}

static std::string GetString(const uint8_t* Data, size_t Size, size_t& Off, size_t MaxLen)
{
  size_t remaining = (Off < Size) ? (Size - Off) : 0;
  size_t len = std::min(remaining, GetSizeT(Data, Size, Off, MaxLen + 1));
  std::string s(reinterpret_cast<const char*>(Data + Off), len);
  Off += len;
  return s;
}

template <typename T>
static std::vector<T> GetVectorNumeric(const uint8_t* Data, size_t Size, size_t& Off, size_t MaxCount)
{
  std::vector<T> out;
  size_t count = GetSizeT(Data, Size, Off, MaxCount + 1);
  out.reserve(count);
  for (size_t i = 0; i < count; ++i)
  {
    if constexpr (std::is_same<T, int>::value)
    {
      out.push_back(GetInt(Data, Size, Off));
    }
    else if constexpr (std::is_same<T, long long>::value)
    {
      out.push_back(GetLongLong(Data, Size, Off));
    }
    else if constexpr (std::is_same<T, double>::value)
    {
      long long bits = GetLongLong(Data, Size, Off);
      double d;
      static_assert(sizeof(double) == sizeof(long long), "unexpected double size");
      std::memcpy(&d, &bits, sizeof(double));
      out.push_back(d);
    }
    else if constexpr (std::is_same<T, float>::value)
    {
      int bits = GetInt(Data, Size, Off);
      float f;
      static_assert(sizeof(float) == sizeof(int), "unexpected float size");
      std::memcpy(&f, &bits, sizeof(float));
      out.push_back(f);
    }
    else if constexpr (std::is_same<T, char>::value)
    {
      out.push_back(static_cast<char>(GetByte(Data, Size, Off)));
    }
  }
  return out;
}

static std::vector<std::string> GetVectorString(const uint8_t* Data, size_t Size, size_t& Off, size_t MaxCount, size_t MaxStrLen)
{
  std::vector<std::string> out;
  size_t count = GetSizeT(Data, Size, Off, MaxCount + 1);
  out.reserve(count);
  for (size_t i = 0; i < count; ++i)
  {
    out.push_back(GetString(Data, Size, Off, MaxStrLen));
  }
  return out;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
  size_t Off = 0;

  rapidcsv::LabelParams labelParams;
  labelParams.mColumnNameIdx = (GetBool(Data, Size, Off) ? 0 : -1);

  rapidcsv::SeparatorParams separatorParams;
  separatorParams.mSeparator = static_cast<char>(GetByte(Data, Size, Off));
  if (separatorParams.mSeparator == '\0' || separatorParams.mSeparator == '\n' || separatorParams.mSeparator == '\r')
  {
    separatorParams.mSeparator = ',';
  }
  separatorParams.mTrim = GetBool(Data, Size, Off);

  rapidcsv::ConverterParams converterParams;
  converterParams.mHasDefaultConverter = GetBool(Data, Size, Off);
  converterParams.mDefaultFloat = static_cast<long double>(GetLongLong(Data, Size, Off));
  converterParams.mDefaultInteger = GetLongLong(Data, Size, Off);

  rapidcsv::LineReaderParams lineReaderParams;
  lineReaderParams.mSkipCommentLines = GetBool(Data, Size, Off);
  lineReaderParams.mCommentPrefix = static_cast<char>(GetByte(Data, Size, Off));
  if (lineReaderParams.mCommentPrefix == '\0')
  {
    lineReaderParams.mCommentPrefix = '#';
  }

  std::string csvData = GetString(Data, Size, Off, 4096);

  {
    std::ofstream ofs("./dummy_file", std::ios::binary);
    ofs.write(csvData.data(), static_cast<std::streamsize>(csvData.size()));
  }

  std::stringstream ss(csvData);

  try
  {
    rapidcsv::Document doc(ss, labelParams, separatorParams, converterParams, lineReaderParams);

    try
    {
      rapidcsv::Converter<int> convInt(converterParams);
      std::string out;
      convInt.ToStr(GetInt(Data, Size, Off), out);
    }
    catch (...)
    {
    }

    try
    {
      rapidcsv::Converter<long long> convLL(converterParams);
      std::string out;
      convLL.ToStr(GetLongLong(Data, Size, Off), out);
    }
    catch (...)
    {
    }

    try
    {
      rapidcsv::Converter<float> convFloat(converterParams);
      std::string out;
      int bits = GetInt(Data, Size, Off);
      float f;
      std::memcpy(&f, &bits, sizeof(float));
      convFloat.ToStr(f, out);
    }
    catch (...)
    {
    }

    try
    {
      rapidcsv::Converter<double> convDouble(converterParams);
      std::string out;
      long long bits = GetLongLong(Data, Size, Off);
      double d;
      std::memcpy(&d, &bits, sizeof(double));
      convDouble.ToStr(d, out);
    }
    catch (...)
    {
    }

    try
    {
      rapidcsv::Converter<std::string> convStr(converterParams);
      std::string out;
      std::string in = GetString(Data, Size, Off, 128);
      convStr.ToStr(in, out);
    }
    catch (...)
    {
    }

    for (int iter = 0; iter < 12; ++iter)
    {
      std::string colName = GetString(Data, Size, Off, 64);
      std::string rowName = GetString(Data, Size, Off, 64);
      size_t colIdx = GetSizeT(Data, Size, Off, 32);

      switch (GetByte(Data, Size, Off) % 10)
      {
        case 0:
        {
          try
          {
            auto v = GetVectorNumeric<int>(Data, Size, Off, 32);
            doc.SetColumn(colName, v);
          }
          catch (...)
          {
          }
          break;
        }
        case 1:
        {
          try
          {
            auto v = GetVectorNumeric<long long>(Data, Size, Off, 32);
            doc.SetColumn(colName, v);
          }
          catch (...)
          {
          }
          break;
        }
        case 2:
        {
          try
          {
            auto v = GetVectorNumeric<double>(Data, Size, Off, 32);
            doc.SetColumn(colName, v);
          }
          catch (...)
          {
          }
          break;
        }
        case 3:
        {
          try
          {
            auto v = GetVectorString(Data, Size, Off, 32, 64);
            doc.SetColumn(colName, v);
          }
          catch (...)
          {
          }
          break;
        }
        case 4:
        {
          try
          {
            int val = GetInt(Data, Size, Off);
            doc.SetCell(colName, rowName, val);
          }
          catch (...)
          {
          }
          break;
        }
        case 5:
        {
          try
          {
            double val;
            long long bits = GetLongLong(Data, Size, Off);
            std::memcpy(&val, &bits, sizeof(double));
            doc.SetCell(colName, rowName, val);
          }
          catch (...)
          {
          }
          break;
        }
        case 6:
        {
          try
          {
            std::string val = GetString(Data, Size, Off, 128);
            doc.SetCell(colName, rowName, val);
          }
          catch (...)
          {
          }
          break;
        }
        case 7:
        {
          try
          {
            auto v = GetVectorNumeric<int>(Data, Size, Off, 32);
            std::string newColName = GetString(Data, Size, Off, 64);
            doc.InsertColumn(colIdx, v, newColName);
          }
          catch (...)
          {
          }
          break;
        }
        case 8:
        {
          try
          {
            auto v = GetVectorString(Data, Size, Off, 32, 64);
            std::string newColName = GetString(Data, Size, Off, 64);
            doc.InsertColumn(colIdx, v, newColName);
          }
          catch (...)
          {
          }
          break;
        }
        case 9:
        {
          try
          {
            auto v = GetVectorNumeric<double>(Data, Size, Off, 32);
            doc.SetRow(rowName, v);
          }
          catch (...)
          {
          }
          try
          {
            auto v = GetVectorString(Data, Size, Off, 32, 64);
            doc.SetRow(rowName, v);
          }
          catch (...)
          {
          }
          break;
        }
      }
    }
  }
  catch (...)
  {
  }

  return 0;
}