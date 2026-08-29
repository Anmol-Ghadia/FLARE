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

static uint64_t ReadU64(const uint8_t* data, size_t size, size_t& offset)
{
  uint64_t value = 0;
  for (size_t i = 0; i < 8 && offset < size; ++i)
  {
    value = (value << 8) | data[offset++];
  }
  return value;
}

static int ReadInt(const uint8_t* data, size_t size, size_t& offset)
{
  return static_cast<int>(ReadU64(data, size, offset));
}

static size_t ReadSizeT(const uint8_t* data, size_t size, size_t& offset)
{
  return static_cast<size_t>(ReadU64(data, size, offset));
}

static bool ReadBool(const uint8_t* data, size_t size, size_t& offset)
{
  if (offset >= size)
  {
    return false;
  }
  return (data[offset++] & 1) != 0;
}

static char ReadChar(const uint8_t* data, size_t size, size_t& offset, char fallback)
{
  if (offset >= size)
  {
    return fallback;
  }
  return static_cast<char>(data[offset++]);
}

static std::string ReadString(const uint8_t* data, size_t size, size_t& offset)
{
  if (offset >= size)
  {
    return std::string();
  }

  size_t remaining = size - offset;
  size_t len = ReadSizeT(data, size, offset);
  if (remaining == 0)
  {
    return std::string();
  }

  if (offset > size)
  {
    offset = size;
    return std::string();
  }

  remaining = size - offset;
  if (len > remaining)
  {
    len = remaining;
  }

  std::string out(reinterpret_cast<const char*>(data + offset), len);
  offset += len;
  return out;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
  size_t offset = 0;

  rapidcsv::LabelParams labelParams;
  labelParams.mColumnNameIdx = ReadInt(Data, Size, offset);

  rapidcsv::SeparatorParams separatorParams;
  separatorParams.mSeparator = ReadChar(Data, Size, offset, ',');
  if (separatorParams.mSeparator == '\0' || separatorParams.mSeparator == '\n' || separatorParams.mSeparator == '\r')
  {
    separatorParams.mSeparator = ',';
  }
  separatorParams.mTrim = ReadBool(Data, Size, offset);

  rapidcsv::ConverterParams converterParams;
  converterParams.mHasDefaultConverter = ReadBool(Data, Size, offset);
  converterParams.mDefaultFloat = static_cast<long double>(ReadU64(Data, Size, offset));
  converterParams.mDefaultInteger = static_cast<long long>(ReadU64(Data, Size, offset));

  rapidcsv::LineReaderParams lineReaderParams;
  lineReaderParams.mSkipCommentLines = ReadBool(Data, Size, offset);
  lineReaderParams.mCommentPrefix = ReadChar(Data, Size, offset, '#');

  std::string initialCsv = ReadString(Data, Size, offset);
  std::istringstream csvStream(initialCsv);

  try
  {
    rapidcsv::Document doc(csvStream, labelParams, separatorParams, converterParams, lineReaderParams);

    size_t col1 = ReadSizeT(Data, Size, offset);
    size_t row1 = ReadSizeT(Data, Size, offset);
    std::string cell1 = ReadString(Data, Size, offset);

    size_t col2 = ReadSizeT(Data, Size, offset);
    size_t row2 = ReadSizeT(Data, Size, offset);
    std::string cell2 = ReadString(Data, Size, offset);

    try
    {
      doc.SetCell<std::string>(col1, row1, cell1);
    }
    catch (...)
    {
    }

    try
    {
      doc.SetCell<std::string>(col2, row2, cell2);
    }
    catch (...)
    {
    }

    {
      std::ofstream ofs("./dummy_file", std::ios::binary);
      ofs.write(initialCsv.data(), static_cast<std::streamsize>(initialCsv.size()));
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
    }
    catch (...)
    {
    }

    try
    {
      colCount = doc.GetColumnCount();
    }
    catch (...)
    {
    }

    size_t getCol1 = ReadSizeT(Data, Size, offset);
    size_t getRow1 = ReadSizeT(Data, Size, offset);
    size_t getCol2 = ReadSizeT(Data, Size, offset);
    size_t getRow2 = ReadSizeT(Data, Size, offset);

    if (colCount != 0)
    {
      getCol1 %= (colCount + 1);
      getCol2 %= (colCount + 1);
    }
    if (rowCount != 0)
    {
      getRow1 %= (rowCount + 1);
      getRow2 %= (rowCount + 1);
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
      (void)doc.GetCell<std::string>(getCol2, getRow2);
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