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

#include "rapidcsv.h"

static int ToInt(const uint8_t* data, size_t size, size_t& offset, int fallback)
{
  if (offset + 4 > size) return fallback;
  int v = static_cast<int>(data[offset]) |
          (static_cast<int>(data[offset + 1]) << 8) |
          (static_cast<int>(data[offset + 2]) << 16) |
          (static_cast<int>(data[offset + 3]) << 24);
  offset += 4;
  return v;
}

static size_t ToSizeT(const uint8_t* data, size_t size, size_t& offset, size_t fallback)
{
  if (offset + 8 > size) return fallback;
  size_t v = 0;
  for (size_t i = 0; i < 8; ++i)
  {
    v |= (static_cast<size_t>(data[offset + i]) << (i * 8));
  }
  offset += 8;
  return v;
}

static bool ToBool(const uint8_t* data, size_t size, size_t& offset, bool fallback)
{
  if (offset >= size) return fallback;
  return (data[offset++] & 1) != 0;
}

static char ToChar(const uint8_t* data, size_t size, size_t& offset, char fallback)
{
  if (offset >= size) return fallback;
  return static_cast<char>(data[offset++]);
}

static std::string ToString(const uint8_t* data, size_t size, size_t& offset, size_t maxLen)
{
  if (offset >= size) return std::string();
  size_t remaining = size - offset;
  size_t len = remaining ? static_cast<size_t>(data[offset++]) : 0;
  len = std::min(len, maxLen);
  len = std::min(len, size - offset);
  std::string s(reinterpret_cast<const char*>(data + offset), len);
  offset += len;
  return s;
}

static std::vector<std::string> ToStringVector(const uint8_t* data, size_t size, size_t& offset, size_t maxElems, size_t maxStrLen)
{
  std::vector<std::string> out;
  if (offset >= size) return out;
  size_t count = static_cast<size_t>(data[offset++]) % (maxElems + 1);
  out.reserve(count);
  for (size_t i = 0; i < count; ++i)
  {
    out.push_back(ToString(data, size, offset, maxStrLen));
  }
  return out;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
  size_t offset = 0;

  rapidcsv::LabelParams labelParams;
  labelParams.mColumnNameIdx = ToInt(Data, Size, offset, -1);

  rapidcsv::SeparatorParams separatorParams;
  separatorParams.mSeparator = ToChar(Data, Size, offset, ',');
  if (separatorParams.mSeparator == '\0')
  {
    separatorParams.mSeparator = ',';
  }
  separatorParams.mTrim = ToBool(Data, Size, offset, false);

  rapidcsv::ConverterParams converterParams;
  converterParams.mHasDefaultConverter = ToBool(Data, Size, offset, false);
  converterParams.mDefaultFloat = static_cast<long double>(ToInt(Data, Size, offset, 0));
  converterParams.mDefaultInteger = static_cast<long long>(ToInt(Data, Size, offset, 0));

  rapidcsv::LineReaderParams lineReaderParams;
  lineReaderParams.mSkipCommentLines = ToBool(Data, Size, offset, false);
  lineReaderParams.mCommentPrefix = ToChar(Data, Size, offset, '#');
  if (lineReaderParams.mCommentPrefix == '\0')
  {
    lineReaderParams.mCommentPrefix = '#';
  }

  std::string csv(reinterpret_cast<const char*>(Data), Size);

  {
    std::ofstream ofs("./dummy_file", std::ios::binary);
    ofs.write(csv.data(), static_cast<std::streamsize>(csv.size()));
  }

  try
  {
    std::stringstream ss(csv);
    rapidcsv::Document doc(ss, labelParams, separatorParams, converterParams, lineReaderParams);

    try
    {
      (void)doc.GetRowCount();
    }
    catch (...)
    {
    }

    try
    {
      std::stringstream reload(csv);
      doc.Load(reload, labelParams, separatorParams, converterParams, lineReaderParams);
    }
    catch (...)
    {
    }

    try
    {
      size_t colIdx = ToSizeT(Data, Size, offset, 0) % 32;
      size_t rowIdx = ToSizeT(Data, Size, offset, 0) % 32;
      std::string cellValue = ToString(Data, Size, offset, 64);
      doc.SetCell<std::string>(colIdx, rowIdx, cellValue);
    }
    catch (...)
    {
    }

    try
    {
      size_t colIdx = ToSizeT(Data, Size, offset, 0) % 32;
      int intValue = ToInt(Data, Size, offset, 0);
      size_t rowIdx = ToSizeT(Data, Size, offset, 0) % 32;
      doc.SetCell<int>(colIdx, rowIdx, intValue);
    }
    catch (...)
    {
    }

    try
    {
      std::stringstream out;
      doc.Save(out);
      std::string saved = out.str();
      std::ofstream ofs("./dummy_file", std::ios::binary | std::ios::trunc);
      ofs.write(saved.data(), static_cast<std::streamsize>(saved.size()));
    }
    catch (...)
    {
    }

    try
    {
      size_t colIdx = ToSizeT(Data, Size, offset, 0) % 32;
      std::string colName = ToString(Data, Size, offset, 64);
      doc.SetColumnName(colIdx, colName);
    }
    catch (...)
    {
    }

    try
    {
      size_t colIdx = ToSizeT(Data, Size, offset, 0) % 32;
      std::vector<std::string> col = ToStringVector(Data, Size, offset, 32, 64);
      doc.SetColumn<std::string>(colIdx, col);
    }
    catch (...)
    {
    }

    try
    {
      size_t colIdx = ToSizeT(Data, Size, offset, 0) % 32;
      std::vector<int> col;
      if (offset < Size)
      {
        size_t count = static_cast<size_t>(Data[offset++]) % 32;
        col.reserve(count);
        for (size_t i = 0; i < count; ++i)
        {
          col.push_back(ToInt(Data, Size, offset, 0));
        }
      }
      doc.SetColumn<int>(colIdx, col);
    }
    catch (...)
    {
    }

    try
    {
      (void)doc.GetRowCount();
    }
    catch (...)
    {
    }

    try
    {
      std::stringstream out2;
      doc.Save(out2);
    }
    catch (...)
    {
    }

    try
    {
      std::ifstream ifs("./dummy_file", std::ios::binary);
      if (ifs)
      {
        doc.Load(ifs, labelParams, separatorParams, converterParams, lineReaderParams);
      }
    }
    catch (...)
    {
    }

    try
    {
      (void)doc.GetRowCount();
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