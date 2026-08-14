// This fuzz driver is generated for library rapidcsv, aiming to fuzz the following functions:
// rapidcsv::Document::SetRowName at rapidcsv.h:1515:10 in rapidcsv.h
// rapidcsv::Document::GetRowIdx at rapidcsv.h:926:9 in rapidcsv.h
// rapidcsv::Document::GetRowIdx at rapidcsv.h:926:9 in rapidcsv.h
// rapidcsv::Document::SetColumnName at rapidcsv.h:1441:10 in rapidcsv.h
// rapidcsv::Document::GetCell at rapidcsv.h:1208:7 in rapidcsv.h
// rapidcsv::Document::SetRowName at rapidcsv.h:1515:10 in rapidcsv.h
// rapidcsv::Document::GetCell at rapidcsv.h:1208:7 in rapidcsv.h
// rapidcsv::Document::GetColumnIdx at rapidcsv.h:636:9 in rapidcsv.h
// rapidcsv::Document::SetColumnName at rapidcsv.h:1441:10 in rapidcsv.h
// rapidcsv::Document::GetColumnIdx at rapidcsv.h:636:9 in rapidcsv.h
// rapidcsv::Document::GetColumnIdx at rapidcsv.h:636:9 in rapidcsv.h
// rapidcsv::Document::GetRowIdx at rapidcsv.h:926:9 in rapidcsv.h
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

static std::string MakeString(const uint8_t* data, size_t size, size_t& offset)
{
  if (offset >= size) {
    return std::string();
  }

  size_t remaining = size - offset;
  size_t len = data[offset] % (remaining + 1);
  offset++;

  remaining = size - offset;
  len = std::min(len, remaining);

  std::string s(reinterpret_cast<const char*>(data + offset), len);
  offset += len;
  return s;
}

static size_t MakeIndex(const uint8_t* data, size_t size, size_t& offset)
{
  if (offset >= size) {
    return 0;
  }

  size_t v = data[offset];
  offset++;
  return v;
}

extern "C" int LLVMFuzzerTestOneInput_8(const uint8_t *Data, size_t Size)
{
  std::ofstream ofs("./dummy_file", std::ios::binary);
  if (ofs) {
    ofs.write(reinterpret_cast<const char*>(Data), static_cast<std::streamsize>(Size));
  }
  ofs.close();

  size_t offset = 0;

  rapidcsv::LabelParams labelParams;
  labelParams.mColumnNameIdx = (Size > 0) ? static_cast<int>(static_cast<int8_t>(Data[offset++])) : 0;

  rapidcsv::SeparatorParams separatorParams;
  separatorParams.mSeparator = (offset < Size && Data[offset] != 0) ? static_cast<char>(Data[offset]) : ',';
  if (offset < Size) offset++;
  separatorParams.mTrim = (offset < Size) ? ((Data[offset++] & 1) != 0) : false;

  rapidcsv::ConverterParams converterParams;
  converterParams.mHasDefaultConverter = (offset < Size) ? ((Data[offset++] & 1) != 0) : false;
  converterParams.mDefaultFloat = (offset < Size) ? static_cast<long double>(static_cast<int8_t>(Data[offset++])) : 0.0L;
  converterParams.mDefaultInteger = (offset < Size) ? static_cast<long long>(static_cast<int8_t>(Data[offset++])) : 0LL;

  rapidcsv::LineReaderParams lineReaderParams;
  lineReaderParams.mSkipCommentLines = (offset < Size) ? ((Data[offset++] & 1) != 0) : false;
  lineReaderParams.mCommentPrefix = (offset < Size && Data[offset] != 0) ? static_cast<char>(Data[offset]) : '#';
  if (offset < Size) offset++;

  try {
    rapidcsv::Document doc("./dummy_file", labelParams, separatorParams, converterParams, lineReaderParams);

    std::string colName1 = MakeString(Data, Size, offset);
    std::string colName2 = MakeString(Data, Size, offset);
    std::string colName3 = MakeString(Data, Size, offset);
    std::string rowName1 = MakeString(Data, Size, offset);
    std::string rowName2 = MakeString(Data, Size, offset);
    std::string rowName3 = MakeString(Data, Size, offset);
    std::string cellCol1 = MakeString(Data, Size, offset);
    std::string cellRow1 = MakeString(Data, Size, offset);
    std::string cellCol2 = MakeString(Data, Size, offset);
    std::string cellRow2 = MakeString(Data, Size, offset);

    size_t colIdx1 = MakeIndex(Data, Size, offset);
    size_t colIdx2 = MakeIndex(Data, Size, offset);
    size_t rowIdx1 = MakeIndex(Data, Size, offset);
    size_t rowIdx2 = MakeIndex(Data, Size, offset);

    try { (void)doc.GetColumnIdx(colName1); } catch (...) {}
    try { doc.SetColumnName(colIdx1, colName2); } catch (...) {}
    try { (void)doc.GetColumnIdx(colName2); } catch (...) {}
    try { (void)doc.GetColumnIdx(colName3); } catch (...) {}
    try { (void)doc.GetRowIdx(rowName1); } catch (...) {}
    try { doc.SetRowName(rowIdx1, rowName2); } catch (...) {}
    try { (void)doc.GetRowIdx(rowName2); } catch (...) {}
    try { (void)doc.GetRowIdx(rowName3); } catch (...) {}
    try { doc.SetColumnName(colIdx2, cellCol1); } catch (...) {}
    try { (void)doc.GetCell<std::string>(cellCol1, cellRow1); } catch (...) {}
    try { doc.SetRowName(rowIdx2, cellRow2); } catch (...) {}
    try { (void)doc.GetCell<std::string>(cellCol2, cellRow2); } catch (...) {}

  } catch (...) {
  }

  return 0;
}