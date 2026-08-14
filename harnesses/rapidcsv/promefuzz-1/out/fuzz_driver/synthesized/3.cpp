// This fuzz driver is generated for library rapidcsv, aiming to fuzz the following functions:
// rapidcsv::Document::InsertColumn at rapidcsv.h:845:10 in rapidcsv.h
// rapidcsv::Document::InsertColumn at rapidcsv.h:845:10 in rapidcsv.h
// rapidcsv::Document::InsertColumn at rapidcsv.h:845:10 in rapidcsv.h
// rapidcsv::Document::SetColumn at rapidcsv.h:746:10 in rapidcsv.h
// rapidcsv::Document::SetColumnName at rapidcsv.h:1441:10 in rapidcsv.h
// rapidcsv::Document::InsertColumn at rapidcsv.h:845:10 in rapidcsv.h
// rapidcsv::Document::GetColumn at rapidcsv.h:713:20 in rapidcsv.h
// rapidcsv::Document::SetRowName at rapidcsv.h:1515:10 in rapidcsv.h
// rapidcsv::Document::SetRowName at rapidcsv.h:1515:10 in rapidcsv.h
// rapidcsv::Document::SetRowName at rapidcsv.h:1515:10 in rapidcsv.h
// rapidcsv::Document::SetRowName at rapidcsv.h:1515:10 in rapidcsv.h
// rapidcsv::Document::Save at rapidcsv.h:597:10 in rapidcsv.h
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

static size_t ReadSizeT(const uint8_t* Data, size_t Size, size_t& Offset)
{
  size_t value = 0;
  for (size_t i = 0; i < sizeof(size_t) && Offset < Size; ++i)
  {
    value = (value << 8) | Data[Offset++];
  }
  return value;
}

static bool ReadBool(const uint8_t* Data, size_t Size, size_t& Offset)
{
  if (Offset >= Size) return false;
  return (Data[Offset++] & 1) != 0;
}

static char ReadChar(const uint8_t* Data, size_t Size, size_t& Offset, char fallback)
{
  if (Offset >= Size) return fallback;
  char c = static_cast<char>(Data[Offset++]);
  if (c == '\0') c = fallback;
  return c;
}

static std::string ReadString(const uint8_t* Data, size_t Size, size_t& Offset)
{
  if (Offset >= Size) return std::string();
  size_t len = ReadSizeT(Data, Size, Offset);
  if (Offset >= Size) return std::string();
  len = std::min(len, Size - Offset);
  std::string s(reinterpret_cast<const char*>(Data + Offset), len);
  Offset += len;
  return s;
}

static std::vector<std::string> ReadStringVector(const uint8_t* Data, size_t Size, size_t& Offset)
{
  std::vector<std::string> out;
  size_t count = ReadSizeT(Data, Size, Offset) % 16;
  for (size_t i = 0; i < count; ++i)
  {
    out.push_back(ReadString(Data, Size, Offset));
  }
  return out;
}

extern "C" int LLVMFuzzerTestOneInput_3(const uint8_t *Data, size_t Size)
{
  size_t offset = 0;

  std::string initialCsv;
  if (Size > 0)
  {
    size_t csvLen = ReadSizeT(Data, Size, offset);
    csvLen = std::min(csvLen, Size - offset);
    initialCsv.assign(reinterpret_cast<const char*>(Data + offset), csvLen);
    offset += csvLen;
  }

  {
    std::ofstream ofs("./dummy_file", std::ios::binary);
    ofs.write(initialCsv.data(), static_cast<std::streamsize>(initialCsv.size()));
  }

  rapidcsv::LabelParams labelParams;
  labelParams.mColumnNameIdx = ReadBool(Data, Size, offset) ? 0 : -1;

  rapidcsv::SeparatorParams separatorParams;
  separatorParams.mSeparator = ReadChar(Data, Size, offset, ',');
  separatorParams.mTrim = ReadBool(Data, Size, offset);

  rapidcsv::ConverterParams converterParams;
  converterParams.mHasDefaultConverter = ReadBool(Data, Size, offset);
  converterParams.mDefaultFloat = static_cast<long double>(ReadSizeT(Data, Size, offset));
  converterParams.mDefaultInteger = static_cast<long long>(ReadSizeT(Data, Size, offset));

  rapidcsv::LineReaderParams lineReaderParams;
  lineReaderParams.mSkipCommentLines = ReadBool(Data, Size, offset);
  lineReaderParams.mCommentPrefix = ReadChar(Data, Size, offset, '#');

  try
  {
    rapidcsv::Document doc("./dummy_file", labelParams, separatorParams, converterParams, lineReaderParams);

    size_t idx1 = ReadSizeT(Data, Size, offset) % 8;
    size_t idx2 = ReadSizeT(Data, Size, offset) % 8;
    size_t idx3 = ReadSizeT(Data, Size, offset) % 8;
    size_t idx4 = ReadSizeT(Data, Size, offset) % 8;
    size_t idx5 = ReadSizeT(Data, Size, offset) % 8;

    std::vector<std::string> col1 = ReadStringVector(Data, Size, offset);
    std::vector<std::string> col2 = ReadStringVector(Data, Size, offset);
    std::vector<std::string> col3 = ReadStringVector(Data, Size, offset);
    std::vector<std::string> col4 = ReadStringVector(Data, Size, offset);

    std::string colName1 = ReadString(Data, Size, offset);
    std::string colName2 = ReadString(Data, Size, offset);
    std::string colName3 = ReadString(Data, Size, offset);
    std::string setColName = ReadString(Data, Size, offset);
    std::string colName4 = ReadString(Data, Size, offset);

    std::string getColName = ReadBool(Data, Size, offset) ? setColName : ReadString(Data, Size, offset);

    std::string rowName1 = ReadString(Data, Size, offset);
    std::string rowName2 = ReadString(Data, Size, offset);
    std::string rowName3 = ReadString(Data, Size, offset);
    std::string rowName4 = ReadString(Data, Size, offset);

    size_t rowIdx1 = ReadSizeT(Data, Size, offset) % 8;
    size_t rowIdx2 = ReadSizeT(Data, Size, offset) % 8;
    size_t rowIdx3 = ReadSizeT(Data, Size, offset) % 8;
    size_t rowIdx4 = ReadSizeT(Data, Size, offset) % 8;

    try { doc.InsertColumn<std::string>(idx1, col1, colName1); } catch (...) {}
    try { doc.InsertColumn<std::string>(idx2, col2, colName2); } catch (...) {}
    try { doc.InsertColumn<std::string>(idx3, col3, colName3); } catch (...) {}
    try { doc.SetColumn<std::string>(idx4, col4); } catch (...) {}
    try { doc.SetColumnName(idx4, setColName); } catch (...) {}
    try { doc.InsertColumn<std::string>(idx5, std::vector<std::string>(), colName4); } catch (...) {}
    try { (void)doc.GetColumn<std::string>(getColName); } catch (...) {}
    try { doc.SetRowName(rowIdx1, rowName1); } catch (...) {}
    try { doc.SetRowName(rowIdx2, rowName2); } catch (...) {}
    try { doc.SetRowName(rowIdx3, rowName3); } catch (...) {}
    try { doc.SetRowName(rowIdx4, rowName4); } catch (...) {}
    try { doc.Save("./dummy_file"); } catch (...) {}
  }
  catch (...)
  {
  }

  return 0;
}