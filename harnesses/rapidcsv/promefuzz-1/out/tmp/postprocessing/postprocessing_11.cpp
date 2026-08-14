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
#include <sstream>
#include <fstream>
#include <vector>
#include "rapidcsv.h"

static size_t ReadSizeT(const uint8_t*& ptr, size_t& remaining)
{
  size_t value = 0;
  const size_t n = (remaining < sizeof(size_t)) ? remaining : sizeof(size_t);
  for (size_t i = 0; i < n; ++i)
  {
    value = (value << 8) | ptr[i];
  }
  ptr += n;
  remaining -= n;
  return value;
}

static int ReadInt(const uint8_t*& ptr, size_t& remaining)
{
  int value = 0;
  const size_t n = (remaining < sizeof(int)) ? remaining : sizeof(int);
  for (size_t i = 0; i < n; ++i)
  {
    value = (value << 8) | ptr[i];
  }
  ptr += n;
  remaining -= n;
  return value;
}

static bool ReadBool(const uint8_t*& ptr, size_t& remaining)
{
  if (remaining == 0) return false;
  bool v = ((*ptr) & 1U) != 0;
  ++ptr;
  --remaining;
  return v;
}

static char ReadChar(const uint8_t*& ptr, size_t& remaining, char fallback)
{
  if (remaining == 0) return fallback;
  char c = static_cast<char>(*ptr);
  ++ptr;
  --remaining;
  return c;
}

static std::string ReadString(const uint8_t*& ptr, size_t& remaining, size_t maxLen)
{
  if (remaining == 0) return std::string();
  size_t len = ReadSizeT(ptr, remaining);
  if (maxLen > 0 && len > maxLen) len = maxLen;
  if (len > remaining) len = remaining;
  std::string s(reinterpret_cast<const char*>(ptr), len);
  ptr += len;
  remaining -= len;
  return s;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* Data, size_t Size)
{
  const uint8_t* ptr = Data;
  size_t remaining = Size;

  rapidcsv::LabelParams labelParams;
  labelParams.mColumnNameIdx = (remaining > 0) ? (ReadInt(ptr, remaining) % 5 - 2) : 0;

  rapidcsv::SeparatorParams separatorParams;
  separatorParams.mSeparator = ReadChar(ptr, remaining, ',');
  if (separatorParams.mSeparator == '\0' || separatorParams.mSeparator == '\n' || separatorParams.mSeparator == '\r')
  {
    separatorParams.mSeparator = ',';
  }
  separatorParams.mTrim = ReadBool(ptr, remaining);

  rapidcsv::ConverterParams converterParams;
  converterParams.mHasDefaultConverter = ReadBool(ptr, remaining);
  converterParams.mDefaultFloat = static_cast<long double>(ReadInt(ptr, remaining));
  converterParams.mDefaultInteger = static_cast<long long>(ReadInt(ptr, remaining));

  rapidcsv::LineReaderParams lineReaderParams;
  lineReaderParams.mSkipCommentLines = ReadBool(ptr, remaining);
  lineReaderParams.mCommentPrefix = ReadChar(ptr, remaining, '#');
  if (lineReaderParams.mCommentPrefix == '\0')
  {
    lineReaderParams.mCommentPrefix = '#';
  }

  std::string csvData = ReadString(ptr, remaining, 4096);
  std::string cellValue1 = ReadString(ptr, remaining, 256);
  std::string cellValue2 = ReadString(ptr, remaining, 256);
  size_t col1 = ReadSizeT(ptr, remaining) % 32;
  size_t row1 = ReadSizeT(ptr, remaining) % 32;
  size_t col2 = ReadSizeT(ptr, remaining) % 32;
  size_t row2 = ReadSizeT(ptr, remaining) % 32;
  size_t getCol1 = ReadSizeT(ptr, remaining) % 64;
  size_t getRow1 = ReadSizeT(ptr, remaining) % 64;
  size_t getCol2 = ReadSizeT(ptr, remaining) % 64;
  size_t getRow2 = ReadSizeT(ptr, remaining) % 64;

  {
    std::ofstream ofs("./dummy_file", std::ios::binary);
    ofs.write(csvData.data(), static_cast<std::streamsize>(csvData.size()));
  }

  std::stringstream ss;
  ss.write(csvData.data(), static_cast<std::streamsize>(csvData.size()));

  try
  {
    rapidcsv::Document doc(ss, labelParams, separatorParams, converterParams, lineReaderParams);

    try
    {
      doc.SetCell<std::string>(col1, row1, cellValue1);
    }
    catch (...)
    {
    }

    try
    {
      doc.SetCell<std::string>(col2, row2, cellValue2);
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

    size_t rowCount = 0;
    size_t colCount = 0;

    try
    {
      rowCount = doc.GetRowCount();
      (void)rowCount;
    }
    catch (...)
    {
    }

    try
    {
      colCount = doc.GetColumnCount();
      (void)colCount;
    }
    catch (...)
    {
    }

    try
    {
      (void)doc.GetCell<std::string>(getCol1, getRow1);
    }
    catch (...)
    {
    }

    try
    {
      size_t safeCol = (colCount > 0) ? (getCol2 % colCount) : getCol2;
      size_t safeRow = (rowCount > 0) ? (getRow2 % rowCount) : getRow2;
      (void)doc.GetCell<std::string>(safeCol, safeRow);
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