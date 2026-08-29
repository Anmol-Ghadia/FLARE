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
#include <sstream>
#include <algorithm>

#include "rapidcsv.h"

static uint8_t ReadByte(const uint8_t* Data, size_t Size, size_t& Off)
{
  if (Off >= Size) return 0;
  return Data[Off++];
}

static bool ReadBool(const uint8_t* Data, size_t Size, size_t& Off)
{
  return (ReadByte(Data, Size, Off) & 1u) != 0;
}

static size_t ReadSizeT(const uint8_t* Data, size_t Size, size_t& Off)
{
  size_t v = 0;
  for (size_t i = 0; i < sizeof(size_t); ++i)
  {
    v = (v << 8) | ReadByte(Data, Size, Off);
  }
  return v;
}

static int ReadInt(const uint8_t* Data, size_t Size, size_t& Off)
{
  int v = 0;
  for (size_t i = 0; i < sizeof(int); ++i)
  {
    v = (v << 8) | ReadByte(Data, Size, Off);
  }
  return v;
}

static long long ReadLongLong(const uint8_t* Data, size_t Size, size_t& Off)
{
  long long v = 0;
  for (size_t i = 0; i < sizeof(long long); ++i)
  {
    v = (v << 8) | ReadByte(Data, Size, Off);
  }
  return v;
}

static long double ReadLongDouble(const uint8_t* Data, size_t Size, size_t& Off)
{
  long double out = 0.0L;
  unsigned long long bits = 0;
  for (size_t i = 0; i < sizeof(unsigned long long); ++i)
  {
    bits = (bits << 8) | ReadByte(Data, Size, Off);
  }
  out = static_cast<long double>(bits % 1000000ULL) / 17.0L;
  return out;
}

static std::string ReadString(const uint8_t* Data, size_t Size, size_t& Off, size_t MaxLen = 64)
{
  size_t remaining = (Off < Size) ? (Size - Off) : 0;
  size_t len = remaining ? (ReadByte(Data, Size, Off) % (std::min(MaxLen, remaining) + 1)) : 0;
  std::string s;
  s.reserve(len);
  for (size_t i = 0; i < len && Off < Size; ++i)
  {
    char c = static_cast<char>(Data[Off++]);
    if (c == '\0') c = 'A';
    s.push_back(c);
  }
  return s;
}

static std::vector<std::string> ReadStringVector(const uint8_t* Data, size_t Size, size_t& Off, size_t MaxCount = 16, size_t MaxStrLen = 32)
{
  size_t count = ReadByte(Data, Size, Off) % (MaxCount + 1);
  std::vector<std::string> out;
  out.reserve(count);
  for (size_t i = 0; i < count; ++i)
  {
    out.push_back(ReadString(Data, Size, Off, MaxStrLen));
  }
  return out;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
  size_t Off = 0;

  std::ofstream seedFile("./dummy_file", std::ios::binary);
  if (seedFile)
  {
    seedFile.write(reinterpret_cast<const char*>(Data), static_cast<std::streamsize>(Size));
    seedFile.close();
  }

  rapidcsv::LabelParams labelParams;
  labelParams.mColumnNameIdx = (ReadInt(Data, Size, Off) % 5) - 2;

  rapidcsv::SeparatorParams separatorParams;
  separatorParams.mSeparator = static_cast<char>(ReadByte(Data, Size, Off));
  if (separatorParams.mSeparator == '\0' || separatorParams.mSeparator == '\n' || separatorParams.mSeparator == '\r')
  {
    separatorParams.mSeparator = ',';
  }
  separatorParams.mTrim = ReadBool(Data, Size, Off);

  rapidcsv::ConverterParams converterParams;
  converterParams.mHasDefaultConverter = ReadBool(Data, Size, Off);
  converterParams.mDefaultFloat = ReadLongDouble(Data, Size, Off);
  converterParams.mDefaultInteger = ReadLongLong(Data, Size, Off);

  rapidcsv::LineReaderParams lineReaderParams;
  lineReaderParams.mSkipCommentLines = ReadBool(Data, Size, Off);
  lineReaderParams.mCommentPrefix = static_cast<char>(ReadByte(Data, Size, Off));
  if (lineReaderParams.mCommentPrefix == '\0')
  {
    lineReaderParams.mCommentPrefix = '#';
  }

  try
  {
    rapidcsv::Document doc("./dummy_file", labelParams, separatorParams, converterParams, lineReaderParams);

    for (int iter = 0; iter < 12 && Off < Size; ++iter)
    {
      uint8_t op = ReadByte(Data, Size, Off) % 8;

      try
      {
        switch (op)
        {
          case 0:
          {
            std::ostringstream oss;
            doc.Save(oss);
            break;
          }
          case 1:
          {
            std::string rowName = ReadString(Data, Size, Off);
            std::vector<std::string> row = ReadStringVector(Data, Size, Off);
            doc.SetRow(rowName, row);
            break;
          }
          case 2:
          {
            size_t colIdx = ReadSizeT(Data, Size, Off) % 32;
            std::vector<std::string> col = ReadStringVector(Data, Size, Off);
            std::string colName = ReadString(Data, Size, Off);
            doc.InsertColumn(colIdx, col, colName);
            break;
          }
          case 3:
          {
            std::string colName = ReadString(Data, Size, Off);
            std::vector<std::string> col = ReadStringVector(Data, Size, Off);
            doc.SetColumn(colName, col);
            break;
          }
          case 4:
          {
            size_t rowIdx = ReadSizeT(Data, Size, Off) % 32;
            std::vector<std::string> row = ReadStringVector(Data, Size, Off);
            std::string rowName = ReadString(Data, Size, Off);
            doc.InsertRow(rowIdx, row, rowName);
            break;
          }
          case 5:
          {
            std::string colName = ReadString(Data, Size, Off);
            std::string rowName = ReadString(Data, Size, Off);
            std::string cell = ReadString(Data, Size, Off, 128);
            doc.SetCell(colName, rowName, cell);
            break;
          }
          case 6:
          {
            std::vector<std::string> emptyCol;
            size_t colIdx = ReadSizeT(Data, Size, Off) % 32;
            std::string colName = ReadString(Data, Size, Off);
            doc.InsertColumn(colIdx, emptyCol, colName);
            break;
          }
          case 7:
          {
            std::vector<std::string> emptyRow;
            size_t rowIdx = ReadSizeT(Data, Size, Off) % 32;
            std::string rowName = ReadString(Data, Size, Off);
            doc.InsertRow(rowIdx, emptyRow, rowName);
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
      std::ofstream outFile("./dummy_file", std::ios::binary | std::ios::trunc);
      if (outFile)
      {
        doc.Save(outFile);
      }
    }
    catch (...)
    {
    }

    try
    {
      std::ostringstream finalStream;
      doc.Save(finalStream);
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