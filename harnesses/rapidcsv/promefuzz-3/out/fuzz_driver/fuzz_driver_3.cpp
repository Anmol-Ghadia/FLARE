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
#include <algorithm>

#include "rapidcsv.h"

static std::string MakeString(const uint8_t* data, size_t size, size_t offset, size_t len)
{
  if (offset >= size) {
    return std::string();
  }
  const size_t n = std::min(len, size - offset);
  return std::string(reinterpret_cast<const char*>(data + offset), n);
}

static std::vector<std::string> MakeStringVector(const uint8_t* data, size_t size, size_t& offset, size_t count)
{
  std::vector<std::string> out;
  out.reserve(count);
  for (size_t i = 0; i < count; ++i) {
    size_t len = 0;
    if (offset < size) {
      len = static_cast<size_t>(data[offset++]) % 16U;
    }
    out.push_back(MakeString(data, size, offset, len));
    offset += std::min(len, size > offset ? size - offset : 0U);
  }
  return out;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
  std::ofstream ofs("./dummy_file", std::ios::binary);
  if (ofs) {
    ofs.write(reinterpret_cast<const char*>(Data), static_cast<std::streamsize>(Size));
  }
  ofs.close();

  rapidcsv::LabelParams labelParams;
  labelParams.mColumnNameIdx = 0;

  rapidcsv::SeparatorParams separatorParams;
  separatorParams.mSeparator = (Size > 0 && Data[0] != '\0') ? static_cast<char>(Data[0]) : ',';
  separatorParams.mTrim = (Size > 1) ? ((Data[1] & 1U) != 0) : false;

  rapidcsv::ConverterParams converterParams;
  converterParams.mHasDefaultConverter = (Size > 2) ? ((Data[2] & 1U) != 0) : true;
  converterParams.mDefaultFloat = (Size > 3) ? static_cast<long double>(static_cast<int8_t>(Data[3])) : 0.0L;
  converterParams.mDefaultInteger = (Size > 4) ? static_cast<long long>(static_cast<int8_t>(Data[4])) : 0LL;

  rapidcsv::LineReaderParams lineReaderParams;
  lineReaderParams.mSkipCommentLines = (Size > 5) ? ((Data[5] & 1U) != 0) : false;
  lineReaderParams.mCommentPrefix = (Size > 6 && Data[6] != '\0') ? static_cast<char>(Data[6]) : '#';

  try {
    rapidcsv::Document doc("./dummy_file", labelParams, separatorParams, converterParams, lineReaderParams);

    size_t offset = 7;

    size_t idx1 = (offset < Size) ? static_cast<size_t>(Data[offset++]) : 0U;
    size_t idx2 = (offset < Size) ? static_cast<size_t>(Data[offset++]) : 1U;
    size_t idx3 = (offset < Size) ? static_cast<size_t>(Data[offset++]) : 2U;
    size_t idx4 = (offset < Size) ? static_cast<size_t>(Data[offset++]) : 3U;

    size_t cnt1 = (offset < Size) ? static_cast<size_t>(Data[offset++] % 8U) : 0U;
    size_t cnt2 = (offset < Size) ? static_cast<size_t>(Data[offset++] % 8U) : 0U;
    size_t cnt3 = (offset < Size) ? static_cast<size_t>(Data[offset++] % 8U) : 0U;
    size_t cnt4 = (offset < Size) ? static_cast<size_t>(Data[offset++] % 8U) : 0U;

    std::string colName1 = MakeString(Data, Size, offset, (offset < Size) ? (Data[offset] % 16U) : 0U);
    if (offset < Size) {
      size_t len = Data[offset++] % 16U;
      offset += std::min(len, Size > offset ? Size - offset : 0U);
    }

    std::string colName2 = MakeString(Data, Size, offset, (offset < Size) ? (Data[offset] % 16U) : 0U);
    if (offset < Size) {
      size_t len = Data[offset++] % 16U;
      offset += std::min(len, Size > offset ? Size - offset : 0U);
    }

    std::string colName3 = MakeString(Data, Size, offset, (offset < Size) ? (Data[offset] % 16U) : 0U);
    if (offset < Size) {
      size_t len = Data[offset++] % 16U;
      offset += std::min(len, Size > offset ? Size - offset : 0U);
    }

    std::string setColName = MakeString(Data, Size, offset, (offset < Size) ? (Data[offset] % 16U) : 0U);
    if (offset < Size) {
      size_t len = Data[offset++] % 16U;
      offset += std::min(len, Size > offset ? Size - offset : 0U);
    }

    std::string getColName = MakeString(Data, Size, offset, (offset < Size) ? (Data[offset] % 16U) : 0U);
    if (offset < Size) {
      size_t len = Data[offset++] % 16U;
      offset += std::min(len, Size > offset ? Size - offset : 0U);
    }

    std::vector<std::string> vec1 = MakeStringVector(Data, Size, offset, cnt1);
    std::vector<std::string> vec2 = MakeStringVector(Data, Size, offset, cnt2);
    std::vector<std::string> vec3 = MakeStringVector(Data, Size, offset, cnt3);
    std::vector<std::string> vec4 = MakeStringVector(Data, Size, offset, cnt4);

    size_t rowIdx1 = (offset < Size) ? static_cast<size_t>(Data[offset++]) : 0U;
    size_t rowIdx2 = (offset < Size) ? static_cast<size_t>(Data[offset++]) : 1U;
    size_t rowIdx3 = (offset < Size) ? static_cast<size_t>(Data[offset++]) : 2U;
    size_t rowIdx4 = (offset < Size) ? static_cast<size_t>(Data[offset++]) : 3U;

    std::string rowName1 = MakeString(Data, Size, offset, (offset < Size) ? (Data[offset] % 16U) : 0U);
    if (offset < Size) {
      size_t len = Data[offset++] % 16U;
      offset += std::min(len, Size > offset ? Size - offset : 0U);
    }

    std::string rowName2 = MakeString(Data, Size, offset, (offset < Size) ? (Data[offset] % 16U) : 0U);
    if (offset < Size) {
      size_t len = Data[offset++] % 16U;
      offset += std::min(len, Size > offset ? Size - offset : 0U);
    }

    std::string rowName3 = MakeString(Data, Size, offset, (offset < Size) ? (Data[offset] % 16U) : 0U);
    if (offset < Size) {
      size_t len = Data[offset++] % 16U;
      offset += std::min(len, Size > offset ? Size - offset : 0U);
    }

    std::string rowName4 = MakeString(Data, Size, offset, (offset < Size) ? (Data[offset] % 16U) : 0U);
    if (offset < Size) {
      size_t len = Data[offset++] % 16U;
      offset += std::min(len, Size > offset ? Size - offset : 0U);
    }

    try { doc.InsertColumn(idx1, vec1, colName1); } catch (...) {}
    try { doc.InsertColumn(idx2, vec2, colName2); } catch (...) {}
    try { doc.InsertColumn(idx3, vec3, colName3); } catch (...) {}
    try { doc.SetColumn(idx1, vec4); } catch (...) {}
    try { doc.SetColumnName(idx2, setColName); } catch (...) {}
    try { doc.InsertColumn(idx4, std::vector<std::string>(), getColName); } catch (...) {}
    try { (void)doc.GetColumn<std::string>(getColName); } catch (...) {}
    try { doc.SetRowName(rowIdx1, rowName1); } catch (...) {}
    try { doc.SetRowName(rowIdx2, rowName2); } catch (...) {}
    try { doc.SetRowName(rowIdx3, rowName3); } catch (...) {}
    try { doc.SetRowName(rowIdx4, rowName4); } catch (...) {}
    try { doc.Save("./dummy_file"); } catch (...) {}
  } catch (...) {
  }

  return 0;
}