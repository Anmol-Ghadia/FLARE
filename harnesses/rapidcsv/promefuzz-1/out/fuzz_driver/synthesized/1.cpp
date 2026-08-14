// This fuzz driver is generated for library rapidcsv, aiming to fuzz the following functions:
// rapidcsv::Document::GetRow at rapidcsv.h:991:20 in rapidcsv.h
// rapidcsv::Document::InsertRow at rapidcsv.h:1115:10 in rapidcsv.h
// rapidcsv::Document::GetRow at rapidcsv.h:991:20 in rapidcsv.h
// rapidcsv::Document::GetCell at rapidcsv.h:1257:7 in rapidcsv.h
// rapidcsv::Document::GetCell at rapidcsv.h:1172:7 in rapidcsv.h
// rapidcsv::Document::GetCell at rapidcsv.h:1294:7 in rapidcsv.h
// rapidcsv::Document::GetCell at rapidcsv.h:1208:7 in rapidcsv.h
// rapidcsv::Document::SetCell at rapidcsv.h:1367:10 in rapidcsv.h
// rapidcsv::Document::GetCell at rapidcsv.h:1208:7 in rapidcsv.h
// rapidcsv::Document::SetCell at rapidcsv.h:1331:10 in rapidcsv.h
// rapidcsv::Document::GetCell at rapidcsv.h:1172:7 in rapidcsv.h
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

static bool ReadBool(const uint8_t* data, size_t size, size_t& offset)
{
  if (offset >= size) return false;
  return (data[offset++] & 1U) != 0;
}

static char ReadChar(const uint8_t* data, size_t size, size_t& offset, char fallback)
{
  if (offset >= size) return fallback;
  char c = static_cast<char>(data[offset++]);
  if (c == '\0' || c == '\n' || c == '\r')
  {
    return fallback;
  }
  return c;
}

static std::string ReadString(const uint8_t* data, size_t size, size_t& offset)
{
  if (offset >= size) return std::string();
  size_t len = static_cast<size_t>(data[offset++]);
  len = std::min(len, size - offset);
  std::string s(reinterpret_cast<const char*>(data + offset), len);
  offset += len;
  return s;
}

extern "C" int LLVMFuzzerTestOneInput_1(const uint8_t *Data, size_t Size)
{
  size_t offset = 0;

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

  std::string csvContent;
  if (offset < Size)
  {
    csvContent.assign(reinterpret_cast<const char*>(Data + offset), Size - offset);
  }

  {
    std::ofstream ofs("./dummy_file", std::ios::binary);
    ofs.write(csvContent.data(), static_cast<std::streamsize>(csvContent.size()));
  }

  try
  {
    rapidcsv::Document doc("./dummy_file", labelParams, separatorParams, converterParams, lineReaderParams);

    std::string rowName1 = ReadString(Data, Size, offset);
    std::string rowName2 = ReadString(Data, Size, offset);
    std::string rowName3 = ReadString(Data, Size, offset);
    std::string colName1 = ReadString(Data, Size, offset);
    std::string colName2 = ReadString(Data, Size, offset);

    size_t rowIdx1 = ReadSizeT(Data, Size, offset) % 16;
    size_t rowIdx2 = ReadSizeT(Data, Size, offset) % 16;
    size_t rowIdx3 = ReadSizeT(Data, Size, offset) % 16;
    size_t rowIdx4 = ReadSizeT(Data, Size, offset) % 16;
    size_t colIdx1 = ReadSizeT(Data, Size, offset) % 16;
    size_t colIdx2 = ReadSizeT(Data, Size, offset) % 16;
    size_t colIdx3 = ReadSizeT(Data, Size, offset) % 16;
    size_t colIdx4 = ReadSizeT(Data, Size, offset) % 16;

    std::string setValue1 = ReadString(Data, Size, offset);
    std::string setValue2 = ReadString(Data, Size, offset);

    std::vector<std::string> insertedRow;
    size_t insertedCount = ReadSizeT(Data, Size, offset) % 8;
    for (size_t i = 0; i < insertedCount; ++i)
    {
      insertedRow.push_back(ReadString(Data, Size, offset));
    }

    try { (void)doc.GetRow<std::string>(rowName1); } catch (...) {}

    try { doc.InsertRow<std::string>(rowIdx1, insertedRow, rowName2); } catch (...) {}

    try { (void)doc.GetRow<std::string>(rowName2); } catch (...) {}

    try { (void)doc.GetCell<std::string>(colName1, rowIdx2); } catch (...) {}
    try { (void)doc.GetCell<std::string>(colIdx1, rowIdx3); } catch (...) {}
    try { (void)doc.GetCell<std::string>(colIdx2, rowName2); } catch (...) {}
    try { (void)doc.GetCell<std::string>(colName2, rowName3); } catch (...) {}

    try { doc.SetCell<std::string>(colName1, rowName2, setValue1); } catch (...) {}

    try { (void)doc.GetCell<std::string>(colName1, rowName2); } catch (...) {}

    try { doc.SetCell<std::string>(colIdx3, rowIdx4, setValue2); } catch (...) {}

    try { (void)doc.GetCell<std::string>(colIdx4, rowIdx4); } catch (...) {}
  }
  catch (...)
  {
  }

  return 0;
}