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
#include <algorithm>

#include "rapidcsv.h"

static size_t ReadSizeT(const uint8_t* data, size_t size, size_t& off)
{
  size_t v = 0;
  const size_t n = std::min(sizeof(size_t), size - off);
  for (size_t i = 0; i < n; ++i)
  {
    v = (v << 8) | data[off + i];
  }
  off += n;
  return v;
}

static bool ReadBool(const uint8_t* data, size_t size, size_t& off)
{
  if (off >= size) return false;
  return (data[off++] & 1) != 0;
}

static char ReadChar(const uint8_t* data, size_t size, size_t& off, char fallback)
{
  if (off >= size) return fallback;
  char c = static_cast<char>(data[off++]);
  if (c == '\0') c = fallback;
  return c;
}

static std::string ReadString(const uint8_t* data, size_t size, size_t& off)
{
  if (off >= size) return std::string();
  size_t len = ReadSizeT(data, size, off);
  if (off > size) return std::string();
  len = std::min(len, size - off);
  std::string s(reinterpret_cast<const char*>(data + off), len);
  off += len;
  return s;
}

static std::vector<std::string> ReadStringVector(const uint8_t* data, size_t size, size_t& off)
{
  std::vector<std::string> out;
  size_t count = 0;
  if (off < size)
  {
    count = ReadSizeT(data, size, off) % 16;
  }
  for (size_t i = 0; i < count && off < size; ++i)
  {
    out.push_back(ReadString(data, size, off));
  }
  return out;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
  size_t off = 0;

  rapidcsv::LabelParams labelParams;
  labelParams.mColumnNameIdx = static_cast<int>(ReadSizeT(Data, Size, off) % 4) - 1;

  rapidcsv::SeparatorParams separatorParams;
  separatorParams.mSeparator = ReadChar(Data, Size, off, ',');
  separatorParams.mTrim = ReadBool(Data, Size, off);

  rapidcsv::ConverterParams converterParams;
  converterParams.mHasDefaultConverter = ReadBool(Data, Size, off);
  converterParams.mDefaultFloat = static_cast<long double>(ReadSizeT(Data, Size, off));
  converterParams.mDefaultInteger = static_cast<long long>(ReadSizeT(Data, Size, off));

  rapidcsv::LineReaderParams lineReaderParams;
  lineReaderParams.mSkipCommentLines = ReadBool(Data, Size, off);
  lineReaderParams.mCommentPrefix = ReadChar(Data, Size, off, '#');

  std::string initialFileContent = ReadString(Data, Size, off);
  {
    std::ofstream ofs("./dummy_file", std::ios::binary);
    ofs.write(initialFileContent.data(), static_cast<std::streamsize>(initialFileContent.size()));
  }

  try
  {
    rapidcsv::Document doc("./dummy_file", labelParams, separatorParams, converterParams, lineReaderParams);

    size_t insertIdx1 = ReadSizeT(Data, Size, off) % 16;
    std::vector<std::string> col1 = ReadStringVector(Data, Size, off);
    std::string colName1 = ReadString(Data, Size, off);
    doc.InsertColumn<std::string>(insertIdx1, col1, colName1);

    size_t insertIdx2 = ReadSizeT(Data, Size, off) % 16;
    std::vector<std::string> col2 = ReadStringVector(Data, Size, off);
    std::string colName2 = ReadString(Data, Size, off);
    doc.InsertColumn<std::string>(insertIdx2, col2, colName2);

    size_t insertIdx3 = ReadSizeT(Data, Size, off) % 16;
    std::vector<std::string> col3 = ReadStringVector(Data, Size, off);
    std::string colName3 = ReadString(Data, Size, off);
    doc.InsertColumn<std::string>(insertIdx3, col3, colName3);

    size_t setColIdx = ReadSizeT(Data, Size, off) % 16;
    std::vector<std::string> setCol = ReadStringVector(Data, Size, off);
    doc.SetColumn<std::string>(setColIdx, setCol);

    size_t setColNameIdx = ReadSizeT(Data, Size, off) % 16;
    std::string newColName = ReadString(Data, Size, off);
    doc.SetColumnName(setColNameIdx, newColName);

    size_t insertIdx4 = ReadSizeT(Data, Size, off) % 16;
    std::vector<std::string> col4 = ReadStringVector(Data, Size, off);
    std::string colName4 = ReadString(Data, Size, off);
    doc.InsertColumn<std::string>(insertIdx4, col4, colName4);

    std::string getColName = ReadString(Data, Size, off);
    try
    {
      (void)doc.GetColumn<std::string>(getColName);
    }
    catch (...)
    {
    }

    size_t rowIdx1 = ReadSizeT(Data, Size, off) % 16;
    std::string rowName1 = ReadString(Data, Size, off);
    doc.SetRowName(rowIdx1, rowName1);

    size_t rowIdx2 = ReadSizeT(Data, Size, off) % 16;
    std::string rowName2 = ReadString(Data, Size, off);
    doc.SetRowName(rowIdx2, rowName2);

    size_t rowIdx3 = ReadSizeT(Data, Size, off) % 16;
    std::string rowName3 = ReadString(Data, Size, off);
    doc.SetRowName(rowIdx3, rowName3);

    size_t rowIdx4 = ReadSizeT(Data, Size, off) % 16;
    std::string rowName4 = ReadString(Data, Size, off);
    doc.SetRowName(rowIdx4, rowName4);

    std::string savePath = ReadBool(Data, Size, off) ? ReadString(Data, Size, off) : std::string();
    if (savePath.empty())
    {
      doc.Save();
    }
    else
    {
      doc.Save(savePath);
    }
  }
  catch (...)
  {
  }

  return 0;
}