// This fuzz driver is generated for library rapidcsv, aiming to fuzz the following functions:
// rapidcsv::Document::GetColumnCount at rapidcsv.h:910:12 in rapidcsv.h
// rapidcsv::Document::GetRowCount at rapidcsv.h:1158:12 in rapidcsv.h
// rapidcsv::Document::GetColumnCount at rapidcsv.h:910:12 in rapidcsv.h
// rapidcsv::Document::GetRowCount at rapidcsv.h:1158:12 in rapidcsv.h
// rapidcsv::Document::SetCell at rapidcsv.h:1331:10 in rapidcsv.h
// rapidcsv::Document::SetRow at rapidcsv.h:1024:10 in rapidcsv.h
// rapidcsv::Document::InsertRow at rapidcsv.h:1115:10 in rapidcsv.h
// rapidcsv::Document::Clear at rapidcsv.h:619:10 in rapidcsv.h
// rapidcsv::Document::SetCell at rapidcsv.h:1331:10 in rapidcsv.h
// rapidcsv::Document::GetColumnCount at rapidcsv.h:910:12 in rapidcsv.h
// rapidcsv::Document::GetRowCount at rapidcsv.h:1158:12 in rapidcsv.h
// rapidcsv::Document::Clear at rapidcsv.h:619:10 in rapidcsv.h
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
#include <sstream>
#include <fstream>
#include <algorithm>

#include "rapidcsv.h"

static uint8_t ReadByte(const uint8_t* Data, size_t Size, size_t& Offset)
{
  if (Offset >= Size) return 0;
  return Data[Offset++];
}

static bool ReadBool(const uint8_t* Data, size_t Size, size_t& Offset)
{
  return (ReadByte(Data, Size, Offset) & 1u) != 0;
}

static int ReadInt(const uint8_t* Data, size_t Size, size_t& Offset)
{
  int v = 0;
  for (int i = 0; i < 4; ++i)
  {
    v = (v << 8) | ReadByte(Data, Size, Offset);
  }
  return v;
}

static size_t ReadSizeT(const uint8_t* Data, size_t Size, size_t& Offset, size_t Limit)
{
  size_t v = 0;
  for (int i = 0; i < 4; ++i)
  {
    v = (v << 8) | ReadByte(Data, Size, Offset);
  }
  if (Limit == 0) return 0;
  return v % Limit;
}

static std::string ReadString(const uint8_t* Data, size_t Size, size_t& Offset, size_t MaxLen)
{
  size_t remaining = (Offset < Size) ? (Size - Offset) : 0;
  size_t len = ReadSizeT(Data, Size, Offset, MaxLen + 1);
  len = std::min(len, remaining);
  std::string s;
  s.reserve(len);
  for (size_t i = 0; i < len && Offset < Size; ++i)
  {
    char c = static_cast<char>(Data[Offset++]);
    s.push_back(c);
  }
  return s;
}

static std::vector<std::string> ReadStringVector(const uint8_t* Data, size_t Size, size_t& Offset, size_t MaxItems, size_t MaxStrLen)
{
  size_t count = ReadSizeT(Data, Size, Offset, MaxItems + 1);
  std::vector<std::string> out;
  out.reserve(count);
  for (size_t i = 0; i < count; ++i)
  {
    out.push_back(ReadString(Data, Size, Offset, MaxStrLen));
  }
  return out;
}

extern "C" int LLVMFuzzerTestOneInput_14(const uint8_t *Data, size_t Size)
{
  size_t offset = 0;

  rapidcsv::LabelParams labelParams;
  labelParams.mColumnNameIdx = (ReadInt(Data, Size, offset) % 7) - 3;

  rapidcsv::SeparatorParams separatorParams;
  separatorParams.mSeparator = static_cast<char>(ReadByte(Data, Size, offset));
  if (separatorParams.mSeparator == '\0')
  {
    separatorParams.mSeparator = ',';
  }
  separatorParams.mTrim = ReadBool(Data, Size, offset);

  rapidcsv::ConverterParams converterParams;
  converterParams.mHasDefaultConverter = ReadBool(Data, Size, offset);
  converterParams.mDefaultFloat = static_cast<long double>(ReadInt(Data, Size, offset));
  converterParams.mDefaultInteger = static_cast<long long>(ReadInt(Data, Size, offset));

  rapidcsv::LineReaderParams lineReaderParams;
  lineReaderParams.mSkipCommentLines = ReadBool(Data, Size, offset);
  lineReaderParams.mCommentPrefix = static_cast<char>(ReadByte(Data, Size, offset));

  std::string csvData(reinterpret_cast<const char*>(Data), Size);
  std::ofstream dummy("./dummy_file", std::ios::binary);
  if (dummy.is_open())
  {
    dummy.write(csvData.data(), static_cast<std::streamsize>(csvData.size()));
    dummy.close();
  }

  std::stringstream ss(csvData);

  try
  {
    rapidcsv::Document doc(ss, labelParams, separatorParams, converterParams, lineReaderParams);

    (void)doc.GetColumnCount();
    (void)doc.GetRowCount();

    for (int iter = 0; iter < 8; ++iter)
    {
      uint8_t op = ReadByte(Data, Size, offset) % 7;
      try
      {
        switch (op)
        {
          case 0:
          {
            (void)doc.GetColumnCount();
            break;
          }
          case 1:
          {
            (void)doc.GetRowCount();
            break;
          }
          case 2:
          {
            size_t col = ReadSizeT(Data, Size, offset, 32);
            size_t row = ReadSizeT(Data, Size, offset, 32);
            std::string cell = ReadString(Data, Size, offset, 64);
            doc.SetCell<std::string>(col, row, cell);
            break;
          }
          case 3:
          {
            size_t row = ReadSizeT(Data, Size, offset, 32);
            std::vector<std::string> values = ReadStringVector(Data, Size, offset, 16, 32);
            doc.SetRow<std::string>(row, values);
            break;
          }
          case 4:
          {
            size_t row = ReadSizeT(Data, Size, offset, 32);
            std::vector<std::string> values = ReadStringVector(Data, Size, offset, 16, 32);
            std::string rowName = ReadString(Data, Size, offset, 32);
            doc.InsertRow<std::string>(row, values, rowName);
            break;
          }
          case 5:
          {
            doc.Clear();
            break;
          }
          case 6:
          {
            size_t col = ReadSizeT(Data, Size, offset, 32);
            size_t row = ReadSizeT(Data, Size, offset, 32);
            int value = ReadInt(Data, Size, offset);
            doc.SetCell<int>(col, row, value);
            break;
          }
        }
      }
      catch (...)
      {
      }

      try
      {
        (void)doc.GetColumnCount();
        (void)doc.GetRowCount();
      }
      catch (...)
      {
      }
    }

    try
    {
      doc.Clear();
      (void)doc.GetColumnCount();
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