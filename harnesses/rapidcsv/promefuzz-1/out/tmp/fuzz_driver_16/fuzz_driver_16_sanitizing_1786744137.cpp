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
#include <sstream>
#include <algorithm>

#include "rapidcsv.h"

static uint8_t GetByte(const uint8_t* Data, size_t Size, size_t& Offset)
{
  if (Offset >= Size) return 0;
  return Data[Offset++];
}

static uint32_t GetU32(const uint8_t* Data, size_t Size, size_t& Offset)
{
  uint32_t v = 0;
  for (int i = 0; i < 4; ++i)
  {
    v = (v << 8) | GetByte(Data, Size, Offset);
  }
  return v;
}

static size_t GetSizeT(const uint8_t* Data, size_t Size, size_t& Offset, size_t Mod)
{
  uint32_t v = GetU32(Data, Size, Offset);
  if (Mod == 0) return static_cast<size_t>(v);
  return static_cast<size_t>(v % Mod);
}

static std::string GetString(const uint8_t* Data, size_t Size, size_t& Offset, size_t MaxLen)
{
  size_t len = GetSizeT(Data, Size, Offset, MaxLen + 1);
  len = std::min(len, Size - std::min(Size, Offset));
  std::string s;
  s.reserve(len);
  for (size_t i = 0; i < len && Offset < Size; ++i)
  {
    s.push_back(static_cast<char>(Data[Offset++]));
  }
  return s;
}

static std::vector<std::string> GetStringVector(const uint8_t* Data, size_t Size, size_t& Offset, size_t MaxItems, size_t MaxStrLen)
{
  size_t count = GetSizeT(Data, Size, Offset, MaxItems + 1);
  std::vector<std::string> out;
  out.reserve(count);
  for (size_t i = 0; i < count; ++i)
  {
    out.push_back(GetString(Data, Size, Offset, MaxStrLen));
  }
  return out;
}

static std::vector<int> GetIntVector(const uint8_t* Data, size_t Size, size_t& Offset, size_t MaxItems)
{
  size_t count = GetSizeT(Data, Size, Offset, MaxItems + 1);
  std::vector<int> out;
  out.reserve(count);
  for (size_t i = 0; i < count; ++i)
  {
    out.push_back(static_cast<int>(GetU32(Data, Size, Offset)));
  }
  return out;
}

static std::vector<double> GetDoubleVector(const uint8_t* Data, size_t Size, size_t& Offset, size_t MaxItems)
{
  size_t count = GetSizeT(Data, Size, Offset, MaxItems + 1);
  std::vector<double> out;
  out.reserve(count);
  for (size_t i = 0; i < count; ++i)
  {
    uint32_t a = GetU32(Data, Size, Offset);
    double v = static_cast<double>(static_cast<int32_t>(a)) / 17.0;
    out.push_back(v);
  }
  return out;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
  size_t offset = 0;

  std::ofstream dummy("./dummy_file", std::ios::binary);
  if (dummy)
  {
    dummy.write(reinterpret_cast<const char*>(Data), static_cast<std::streamsize>(Size));
    dummy.close();
  }

  rapidcsv::LabelParams labelParams;
  labelParams.mColumnNameIdx = static_cast<int>(GetByte(Data, Size, offset) % 4) - 1;

  rapidcsv::SeparatorParams separatorParams;
  {
    uint8_t sepChoice = GetByte(Data, Size, offset);
    const char seps[] = {',', ';', '\t', '|', ':'};
    separatorParams.mSeparator = seps[sepChoice % (sizeof(seps) / sizeof(seps[0]))];
    separatorParams.mTrim = (GetByte(Data, Size, offset) & 1) != 0;
  }

  rapidcsv::ConverterParams converterParams;
  converterParams.mHasDefaultConverter = (GetByte(Data, Size, offset) & 1) != 0;
  converterParams.mDefaultFloat = static_cast<long double>(static_cast<int32_t>(GetU32(Data, Size, offset))) / 13.0L;
  converterParams.mDefaultInteger = static_cast<long long>(static_cast<int32_t>(GetU32(Data, Size, offset)));

  rapidcsv::LineReaderParams lineReaderParams;
  lineReaderParams.mSkipCommentLines = (GetByte(Data, Size, offset) & 1) != 0;
  lineReaderParams.mCommentPrefix = static_cast<char>(GetByte(Data, Size, offset));

  try
  {
    rapidcsv::Document doc("./dummy_file", labelParams, separatorParams, converterParams, lineReaderParams);

    for (int iter = 0; iter < 12 && offset < Size; ++iter)
    {
      uint8_t op = GetByte(Data, Size, offset) % 10;
      size_t idx1 = GetSizeT(Data, Size, offset, 64);
      size_t idx2 = GetSizeT(Data, Size, offset, 64);

      try
      {
        switch (op)
        {
          case 0:
          {
            std::vector<std::string> col = GetStringVector(Data, Size, offset, 16, 32);
            doc.SetColumn<std::string>(idx1, col);
            break;
          }
          case 1:
          {
            std::vector<int> col = GetIntVector(Data, Size, offset, 16);
            doc.SetColumn<int>(idx1, col);
            break;
          }
          case 2:
          {
            std::string cell = GetString(Data, Size, offset, 64);
            doc.SetCell<std::string>(idx1, idx2, cell);
            break;
          }
          case 3:
          {
            int cell = static_cast<int>(GetU32(Data, Size, offset));
            doc.SetCell<int>(idx1, idx2, cell);
            break;
          }
          case 4:
          {
            std::vector<std::string> col = GetStringVector(Data, Size, offset, 16, 32);
            std::string name = GetString(Data, Size, offset, 32);
            doc.InsertColumn<std::string>(idx1, col, name);
            break;
          }
          case 5:
          {
            std::vector<double> row = GetDoubleVector(Data, Size, offset, 16);
            doc.SetRow<double>(idx1, row);
            break;
          }
          case 6:
          {
            std::vector<std::string> row = GetStringVector(Data, Size, offset, 16, 32);
            doc.SetRow<std::string>(idx1, row);
            break;
          }
          case 7:
          {
            std::vector<std::string> row = GetStringVector(Data, Size, offset, 16, 32);
            std::string name = GetString(Data, Size, offset, 32);
            doc.InsertRow<std::string>(idx1, row, name);
            break;
          }
          case 8:
          {
            std::ostringstream oss;
            doc.Save(oss);
            (void)oss.str();
            break;
          }
          case 9:
          {
            std::vector<int> col = GetIntVector(Data, Size, offset, 16);
            std::string name = GetString(Data, Size, offset, 32);
            doc.InsertColumn<int>(idx1, col, name);
            break;
          }
        }
      }
      catch (...)
      {
      }
    }

    try
    {
      std::ostringstream oss;
      doc.Save(oss);
      (void)oss.str();
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