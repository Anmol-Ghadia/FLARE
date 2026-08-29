// This fuzz driver is generated for library rapidcsv, aiming to fuzz the following functions:
// rapidcsv::Document::GetColumnCount at rapidcsv.h:910:12 in rapidcsv.h
// rapidcsv::Document::GetRowCount at rapidcsv.h:1158:12 in rapidcsv.h
// rapidcsv::Document::GetColumnCount at rapidcsv.h:910:12 in rapidcsv.h
// rapidcsv::Document::GetRowCount at rapidcsv.h:1158:12 in rapidcsv.h
// rapidcsv::Document::SetRow at rapidcsv.h:1024:10 in rapidcsv.h
// rapidcsv::Document::SetCell at rapidcsv.h:1331:10 in rapidcsv.h
// rapidcsv::Document::InsertRow at rapidcsv.h:1115:10 in rapidcsv.h
// rapidcsv::Document::Clear at rapidcsv.h:619:10 in rapidcsv.h
// rapidcsv::Document::Clear at rapidcsv.h:619:10 in rapidcsv.h
// rapidcsv::Document::GetColumnCount at rapidcsv.h:910:12 in rapidcsv.h
// rapidcsv::Document::GetRowCount at rapidcsv.h:1158:12 in rapidcsv.h
// rapidcsv::Document::InsertRow at rapidcsv.h:1115:10 in rapidcsv.h
// rapidcsv::Document::SetCell at rapidcsv.h:1331:10 in rapidcsv.h
// rapidcsv::Document::SetRow at rapidcsv.h:1024:10 in rapidcsv.h
// rapidcsv::Document::GetColumnCount at rapidcsv.h:910:12 in rapidcsv.h
// rapidcsv::Document::GetRowCount at rapidcsv.h:1158:12 in rapidcsv.h
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

static int ReadInt(const uint8_t* data, size_t size, size_t& offset)
{
  int value = 0;
  const size_t n = std::min(sizeof(int), size - offset);
  for (size_t i = 0; i < n; ++i)
  {
    value = (value << 8) | data[offset++];
  }
  return value;
}

static bool ReadBool(const uint8_t* data, size_t size, size_t& offset)
{
  if (offset >= size) return false;
  return (data[offset++] & 1) != 0;
}

static char ReadChar(const uint8_t* data, size_t size, size_t& offset, char fallback)
{
  if (offset >= size) return fallback;
  char c = static_cast<char>(data[offset++]);
  if (c == '\0') c = fallback;
  return c;
}

static std::string ReadString(const uint8_t* data, size_t size, size_t& offset, size_t maxLen = 64)
{
  if (offset >= size) return std::string();
  size_t len = data[offset++] % (maxLen + 1);
  len = std::min(len, size - offset);
  std::string s(reinterpret_cast<const char*>(data + offset), len);
  offset += len;
  return s;
}

static std::vector<std::string> ReadStringVector(const uint8_t* data, size_t size, size_t& offset, size_t maxItems = 16)
{
  std::vector<std::string> out;
  if (offset >= size) return out;
  size_t count = data[offset++] % (maxItems + 1);
  out.reserve(count);
  for (size_t i = 0; i < count; ++i)
  {
    out.push_back(ReadString(data, size, offset));
  }
  return out;
}

extern "C" int LLVMFuzzerTestOneInput_16(const uint8_t *Data, size_t Size)
{
  size_t offset = 0;

  std::ofstream ofs("./dummy_file", std::ios::binary);
  if (ofs)
  {
    ofs.write(reinterpret_cast<const char*>(Data), static_cast<std::streamsize>(Size));
    ofs.close();
  }

  rapidcsv::LabelParams labelParams;
  labelParams.mColumnNameIdx = ReadInt(Data, Size, offset);

  rapidcsv::SeparatorParams separatorParams;
  separatorParams.mSeparator = ReadChar(Data, Size, offset, ',');
  separatorParams.mTrim = ReadBool(Data, Size, offset);

  rapidcsv::ConverterParams converterParams;
  converterParams.mHasDefaultConverter = ReadBool(Data, Size, offset);
  converterParams.mDefaultFloat = static_cast<long double>(ReadInt(Data, Size, offset));
  converterParams.mDefaultInteger = static_cast<long long>(ReadInt(Data, Size, offset));

  rapidcsv::LineReaderParams lineReaderParams;
  lineReaderParams.mSkipCommentLines = ReadBool(Data, Size, offset);
  lineReaderParams.mCommentPrefix = ReadChar(Data, Size, offset, '#');

  try
  {
    rapidcsv::Document doc("./dummy_file", labelParams, separatorParams, converterParams, lineReaderParams);

    try
    {
      (void)doc.GetColumnCount();
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

    for (int i = 0; i < 8 && offset < Size; ++i)
    {
      uint8_t op = Data[offset++];

      switch (op % 8)
      {
        case 0:
        {
          try
          {
            (void)doc.GetColumnCount();
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
            (void)doc.GetRowCount();
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
            size_t rowIdx = ReadSizeT(Data, Size, offset) % 64;
            std::vector<std::string> row = ReadStringVector(Data, Size, offset);
            doc.SetRow<std::string>(rowIdx, row);
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
            size_t colIdx = ReadSizeT(Data, Size, offset) % 64;
            size_t rowIdx = ReadSizeT(Data, Size, offset) % 64;
            std::string cell = ReadString(Data, Size, offset);
            doc.SetCell<std::string>(colIdx, rowIdx, cell);
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
            size_t rowIdx = ReadSizeT(Data, Size, offset) % 64;
            std::vector<std::string> row = ReadStringVector(Data, Size, offset);
            std::string rowName = ReadString(Data, Size, offset);
            doc.InsertRow<std::string>(rowIdx, row, rowName);
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
            doc.Clear();
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
            doc.Clear();
            (void)doc.GetColumnCount();
            (void)doc.GetRowCount();
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
            size_t rowIdx = ReadSizeT(Data, Size, offset) % 64;
            std::vector<std::string> row;
            doc.InsertRow<std::string>(rowIdx, row, ReadString(Data, Size, offset));
            size_t colIdx = ReadSizeT(Data, Size, offset) % 64;
            doc.SetCell<std::string>(colIdx, rowIdx, ReadString(Data, Size, offset));
            doc.SetRow<std::string>(rowIdx, ReadStringVector(Data, Size, offset));
            (void)doc.GetColumnCount();
            (void)doc.GetRowCount();
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