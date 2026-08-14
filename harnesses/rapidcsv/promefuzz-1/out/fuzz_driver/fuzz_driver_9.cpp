// This fuzz driver is generated for library rapidcsv, aiming to fuzz the following functions:
// rapidcsv::Document::SetCell at rapidcsv.h:1409:10 in rapidcsv.h
// rapidcsv::Document::SetCell at rapidcsv.h:1391:10 in rapidcsv.h
// rapidcsv::Document::SetCell at rapidcsv.h:1409:10 in rapidcsv.h
// rapidcsv::Document::SetCell at rapidcsv.h:1391:10 in rapidcsv.h
// rapidcsv::Document::SetCell at rapidcsv.h:1409:10 in rapidcsv.h
// rapidcsv::Document::SetCell at rapidcsv.h:1391:10 in rapidcsv.h
// rapidcsv::Document::GetCell at rapidcsv.h:1172:7 in rapidcsv.h
// rapidcsv::Document::GetCell at rapidcsv.h:1172:7 in rapidcsv.h
// rapidcsv::Document::GetCell at rapidcsv.h:1172:7 in rapidcsv.h
// rapidcsv::Document::GetCell at rapidcsv.h:1208:7 in rapidcsv.h
// rapidcsv::Document::GetCell at rapidcsv.h:1208:7 in rapidcsv.h
// rapidcsv::Document::GetCell at rapidcsv.h:1208:7 in rapidcsv.h
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
#include <sstream>
#include <fstream>
#include <vector>
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
  size_t take = std::min<size_t>(len, remaining);
  std::string s(reinterpret_cast<const char*>(data + offset), take);
  offset += take;
  return s;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
  size_t offset = 0;

  rapidcsv::LabelParams labelParams;
  labelParams.mColumnNameIdx = (Size > 0) ? static_cast<int>(Data[offset++] % 3) - 1 : 0;

  rapidcsv::SeparatorParams separatorParams;
  separatorParams.mSeparator = (Size > offset) ? static_cast<char>(Data[offset++]) : ',';
  if (separatorParams.mSeparator == '\0' || separatorParams.mSeparator == '\n' || separatorParams.mSeparator == '\r') {
    separatorParams.mSeparator = ',';
  }
  separatorParams.mTrim = (Size > offset) ? ((Data[offset++] & 1) != 0) : false;

  rapidcsv::ConverterParams converterParams;
  converterParams.mHasDefaultConverter = (Size > offset) ? ((Data[offset++] & 1) != 0) : false;
  converterParams.mDefaultFloat = static_cast<long double>((Size > offset) ? static_cast<int8_t>(Data[offset++]) : 0);
  converterParams.mDefaultInteger = static_cast<long long>((Size > offset) ? static_cast<int8_t>(Data[offset++]) : 0);

  rapidcsv::LineReaderParams lineReaderParams;
  lineReaderParams.mSkipCommentLines = (Size > offset) ? ((Data[offset++] & 1) != 0) : false;
  lineReaderParams.mCommentPrefix = (Size > offset) ? static_cast<char>(Data[offset++]) : '#';
  if (lineReaderParams.mCommentPrefix == '\0' || lineReaderParams.mCommentPrefix == '\n' || lineReaderParams.mCommentPrefix == '\r') {
    lineReaderParams.mCommentPrefix = '#';
  }

  std::string csvData;
  if (offset < Size) {
    csvData.assign(reinterpret_cast<const char*>(Data + offset), Size - offset);
  }

  std::stringstream ss(csvData);

  try {
    rapidcsv::Document doc(ss, labelParams, separatorParams, converterParams, lineReaderParams);

    size_t local = 0;
    const uint8_t* aux = reinterpret_cast<const uint8_t*>(csvData.data());
    size_t auxSize = csvData.size();

    std::string colName1 = ReadString(aux, auxSize, local);
    std::string rowName1 = ReadString(aux, auxSize, local);
    std::string cellStr1 = ReadString(aux, auxSize, local);
    std::string colName2 = ReadString(aux, auxSize, local);
    std::string rowName2 = ReadString(aux, auxSize, local);
    std::string cellStr2 = ReadString(aux, auxSize, local);

    size_t rowIdx1 = ReadU32(aux, auxSize, local) % 8;
    size_t rowIdx2 = ReadU32(aux, auxSize, local) % 8;
    size_t rowIdx3 = ReadU32(aux, auxSize, local) % 8;
    size_t colIdx1 = ReadU32(aux, auxSize, local) % 8;
    size_t colIdx2 = ReadU32(aux, auxSize, local) % 8;
    size_t colIdx3 = ReadU32(aux, auxSize, local) % 8;

    int intVal1 = static_cast<int>(ReadU32(aux, auxSize, local));
    int intVal2 = static_cast<int>(ReadU32(aux, auxSize, local));
    double dblVal1 = static_cast<double>(static_cast<int32_t>(ReadU32(aux, auxSize, local))) / 17.0;
    double dblVal2 = static_cast<double>(static_cast<int32_t>(ReadU32(aux, auxSize, local))) / 31.0;

    if (colName1.empty()) colName1 = "A";
    if (colName2.empty()) colName2 = "B";
    if (rowName1.empty()) rowName1 = "R1";
    if (rowName2.empty()) rowName2 = "R2";

    try { doc.SetCell<std::string>(colName1, rowIdx1, cellStr1); } catch (...) {}
    try { doc.SetCell<int>(colIdx1, rowName1, intVal1); } catch (...) {}
    try { doc.SetCell<double>(colName2, rowIdx2, dblVal1); } catch (...) {}
    try { doc.SetCell<std::string>(colIdx2, rowName2, cellStr2); } catch (...) {}
    try { doc.SetCell<int>(colName1, rowIdx3, intVal2); } catch (...) {}
    try { doc.SetCell<double>(colIdx3, rowName1, dblVal2); } catch (...) {}

    try { (void)doc.GetCell<std::string>(colIdx1, rowIdx1); } catch (...) {}
    try { (void)doc.GetCell<int>(colIdx2, rowIdx2); } catch (...) {}
    try { (void)doc.GetCell<double>(colIdx3, rowIdx3); } catch (...) {}
    try { (void)doc.GetCell<std::string>(colName1, rowName1); } catch (...) {}
    try { (void)doc.GetCell<int>(colName2, rowName2); } catch (...) {}
    try { (void)doc.GetCell<double>(colName1, rowName2); } catch (...) {}

    try {
      std::ofstream out("./dummy_file", std::ios::binary);
      out.write(csvData.data(), static_cast<std::streamsize>(csvData.size()));
    } catch (...) {
    }

    try { doc.Save("./dummy_file"); } catch (...) {}
  } catch (...) {
  }

  return 0;
}