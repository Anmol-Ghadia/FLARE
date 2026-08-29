// This fuzz driver is generated for library rapidcsv, aiming to fuzz the following functions:
// rapidcsv::Document::GetColumnIdx at rapidcsv.h:636:9 in rapidcsv.h
// rapidcsv::Document::SetColumnName at rapidcsv.h:1441:10 in rapidcsv.h
// rapidcsv::Document::GetColumnIdx at rapidcsv.h:636:9 in rapidcsv.h
// rapidcsv::Document::GetColumnIdx at rapidcsv.h:636:9 in rapidcsv.h
// rapidcsv::Document::GetRowIdx at rapidcsv.h:926:9 in rapidcsv.h
// rapidcsv::Document::SetRowName at rapidcsv.h:1515:10 in rapidcsv.h
// rapidcsv::Document::GetRowIdx at rapidcsv.h:926:9 in rapidcsv.h
// rapidcsv::Document::GetRowIdx at rapidcsv.h:926:9 in rapidcsv.h
// rapidcsv::Document::SetColumnName at rapidcsv.h:1441:10 in rapidcsv.h
// rapidcsv::Document::GetCell at rapidcsv.h:1208:7 in rapidcsv.h
// rapidcsv::Document::SetRowName at rapidcsv.h:1515:10 in rapidcsv.h
// rapidcsv::Document::GetCell at rapidcsv.h:1208:7 in rapidcsv.h
// rapidcsv::Document::GetColumnIdx at rapidcsv.h:636:9 in rapidcsv.h
// rapidcsv::Document::GetRowIdx at rapidcsv.h:926:9 in rapidcsv.h
// rapidcsv::Document::GetCell at rapidcsv.h:1208:7 in rapidcsv.h
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

static uint32_t ReadU32(const uint8_t* data, size_t size, size_t& offset)
{
  if (offset + 4 > size) {
    offset = size;
    return 0;
  }

  uint32_t v = static_cast<uint32_t>(data[offset]) |
               (static_cast<uint32_t>(data[offset + 1]) << 8) |
               (static_cast<uint32_t>(data[offset + 2]) << 16) |
               (static_cast<uint32_t>(data[offset + 3]) << 24);
  offset += 4;
  return v;
}

static std::string ReadString(const uint8_t* data, size_t size, size_t& offset)
{
  if (offset >= size) {
    return std::string();
  }

  uint32_t len = ReadU32(data, size, offset);
  size_t remaining = (offset <= size) ? (size - offset) : 0;
  size_t actualLen = std::min<size_t>(len, remaining);
  std::string s(reinterpret_cast<const char*>(data + offset), actualLen);
  offset += actualLen;
  return s;
}

static size_t ReadIndex(const uint8_t* data, size_t size, size_t& offset)
{
  return static_cast<size_t>(ReadU32(data, size, offset));
}

extern "C" int LLVMFuzzerTestOneInput_8(const uint8_t *Data, size_t Size)
{
  size_t offset = 0;

  rapidcsv::LabelParams labelParams;
  labelParams.mColumnNameIdx = (Size > 0) ? 0 : -1;

  rapidcsv::SeparatorParams separatorParams;
  separatorParams.mSeparator = ',';
  separatorParams.mTrim = (Size > 1) ? ((Data[0] & 1) != 0) : false;

  rapidcsv::ConverterParams converterParams;
  converterParams.mHasDefaultConverter = true;
  converterParams.mDefaultFloat = 0.0L;
  converterParams.mDefaultInteger = 0;

  rapidcsv::LineReaderParams lineReaderParams;
  lineReaderParams.mSkipCommentLines = (Size > 2) ? ((Data[1] & 1) != 0) : false;
  lineReaderParams.mCommentPrefix = '#';

  std::string csvContent;
  if (Size == 0) {
    csvContent = "corner,c0\nr0,0\n";
  } else {
    csvContent = "corner,c0,c1\nr0,1,2\nr1,3,4\n";
    csvContent.append(reinterpret_cast<const char*>(Data), Size);
  }

  {
    std::ofstream ofs("./dummy_file", std::ios::binary);
    ofs.write(csvContent.data(), static_cast<std::streamsize>(csvContent.size()));
  }

  try {
    rapidcsv::Document doc("./dummy_file",
                           labelParams,
                           separatorParams,
                           converterParams,
                           lineReaderParams);

    std::string colName1 = ReadString(Data, Size, offset);
    size_t colIdx1 = ReadIndex(Data, Size, offset);
    std::string colName2 = ReadString(Data, Size, offset);
    std::string colName3 = ReadString(Data, Size, offset);
    std::string rowName1 = ReadString(Data, Size, offset);
    size_t rowIdx1 = ReadIndex(Data, Size, offset);
    std::string rowName2 = ReadString(Data, Size, offset);
    std::string rowName3 = ReadString(Data, Size, offset);
    size_t colIdx2 = ReadIndex(Data, Size, offset);
    std::string colName4 = ReadString(Data, Size, offset);
    std::string getCellCol1 = ReadString(Data, Size, offset);
    std::string getCellRow1 = ReadString(Data, Size, offset);
    size_t rowIdx2 = ReadIndex(Data, Size, offset);
    std::string rowName4 = ReadString(Data, Size, offset);
    std::string getCellCol2 = ReadString(Data, Size, offset);
    std::string getCellRow2 = ReadString(Data, Size, offset);

    try { (void)doc.GetColumnIdx(colName1); } catch (...) {}
    try { doc.SetColumnName(colIdx1, colName2); } catch (...) {}
    try { (void)doc.GetColumnIdx(colName2); } catch (...) {}
    try { (void)doc.GetColumnIdx(colName3); } catch (...) {}
    try { (void)doc.GetRowIdx(rowName1); } catch (...) {}
    try { doc.SetRowName(rowIdx1, rowName2); } catch (...) {}
    try { (void)doc.GetRowIdx(rowName2); } catch (...) {}
    try { (void)doc.GetRowIdx(rowName3); } catch (...) {}
    try { doc.SetColumnName(colIdx2, colName4); } catch (...) {}
    try { (void)doc.GetCell<std::string>(getCellCol1, getCellRow1); } catch (...) {}
    try { doc.SetRowName(rowIdx2, rowName4); } catch (...) {}
    try { (void)doc.GetCell<std::string>(getCellCol2, getCellRow2); } catch (...) {}

    try {
      if (!colName2.empty()) {
        (void)doc.GetColumnIdx(colName2);
      }
      if (!rowName2.empty()) {
        (void)doc.GetRowIdx(rowName2);
      }
      if (!colName4.empty() && !rowName4.empty()) {
        (void)doc.GetCell<std::string>(colName4, rowName4);
      }
    } catch (...) {}
  } catch (...) {
  }

  return 0;
}