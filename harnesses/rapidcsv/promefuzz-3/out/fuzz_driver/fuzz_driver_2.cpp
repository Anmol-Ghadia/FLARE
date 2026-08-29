// This fuzz driver is generated for library rapidcsv, aiming to fuzz the following functions:
// rapidcsv::Document::GetCell at rapidcsv.h:1172:7 in rapidcsv.h
// rapidcsv::Document::GetCell at rapidcsv.h:1208:7 in rapidcsv.h
// rapidcsv::Document::GetCell at rapidcsv.h:1294:7 in rapidcsv.h
// rapidcsv::Document::GetCell at rapidcsv.h:1257:7 in rapidcsv.h
// rapidcsv::Document::GetRow at rapidcsv.h:991:20 in rapidcsv.h
// rapidcsv::Document::GetRow at rapidcsv.h:944:20 in rapidcsv.h
// rapidcsv::Document::GetColumn at rapidcsv.h:713:20 in rapidcsv.h
// rapidcsv::Document::GetColumn at rapidcsv.h:654:20 in rapidcsv.h
// rapidcsv::Document::GetCell at rapidcsv.h:1172:7 in rapidcsv.h
// rapidcsv::Document::GetCell at rapidcsv.h:1208:7 in rapidcsv.h
// rapidcsv::Document::GetCell at rapidcsv.h:1294:7 in rapidcsv.h
// rapidcsv::Document::GetCell at rapidcsv.h:1257:7 in rapidcsv.h
// rapidcsv::Document::GetRow at rapidcsv.h:991:20 in rapidcsv.h
// rapidcsv::Document::GetRow at rapidcsv.h:944:20 in rapidcsv.h
// rapidcsv::Document::GetColumn at rapidcsv.h:713:20 in rapidcsv.h
// rapidcsv::Document::GetColumn at rapidcsv.h:654:20 in rapidcsv.h
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
#include <functional>
#include "rapidcsv.h"

static std::string MakeString(const uint8_t* data, size_t size, size_t& offset)
{
  if (offset >= size) {
    return std::string();
  }

  size_t len = static_cast<size_t>(data[offset++]);
  if (offset > size) {
    return std::string();
  }

  size_t remaining = size - offset;
  if (len > remaining) {
    len = remaining;
  }

  std::string s(reinterpret_cast<const char*>(data + offset), len);
  offset += len;
  return s;
}

static size_t MakeIndex(const uint8_t* data, size_t size, size_t& offset)
{
  if (offset >= size) {
    return 0;
  }

  size_t value = 0;
  size_t count = 0;
  while ((offset < size) && (count < sizeof(size_t))) {
    value = (value << 8) | data[offset++];
    ++count;
  }
  return value;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
  if (Data == nullptr) {
    return 0;
  }

  std::ofstream ofs("./dummy_file", std::ios::binary);
  if (!ofs) {
    return 0;
  }
  ofs.write(reinterpret_cast<const char*>(Data), static_cast<std::streamsize>(Size));
  ofs.close();

  size_t offset = 0;

  rapidcsv::LabelParams labelParams;
  labelParams.mColumnNameIdx = (Size > 0) ? static_cast<int>(static_cast<int8_t>(Data[offset++])) : 0;

  rapidcsv::SeparatorParams separatorParams;
  separatorParams.mSeparator = (offset < Size) ? static_cast<char>(Data[offset++]) : ',';
  separatorParams.mTrim = (offset < Size) ? ((Data[offset++] & 1U) != 0) : false;

  rapidcsv::ConverterParams converterParams;
  converterParams.mHasDefaultConverter = (offset < Size) ? ((Data[offset++] & 1U) != 0) : false;
  converterParams.mDefaultFloat = (offset < Size) ? static_cast<long double>(static_cast<int8_t>(Data[offset++])) : 0.0L;
  converterParams.mDefaultInteger = (offset < Size) ? static_cast<long long>(static_cast<int8_t>(Data[offset++])) : 0LL;

  rapidcsv::LineReaderParams lineReaderParams;
  lineReaderParams.mSkipCommentLines = (offset < Size) ? ((Data[offset++] & 1U) != 0) : false;
  lineReaderParams.mCommentPrefix = (offset < Size) ? static_cast<char>(Data[offset++]) : '#';

  try {
    rapidcsv::Document doc("./dummy_file", labelParams, separatorParams, converterParams, lineReaderParams);

    const size_t colIdx1 = MakeIndex(Data, Size, offset);
    const size_t rowIdx1 = MakeIndex(Data, Size, offset);
    const std::string colName1 = MakeString(Data, Size, offset);
    const std::string rowName1 = MakeString(Data, Size, offset);

    const size_t colIdx2 = MakeIndex(Data, Size, offset);
    const size_t rowIdx2 = MakeIndex(Data, Size, offset);
    const std::string colName2 = MakeString(Data, Size, offset);
    const std::string rowName2 = MakeString(Data, Size, offset);

    const size_t rowIdx3 = MakeIndex(Data, Size, offset);
    const std::string rowName3 = MakeString(Data, Size, offset);

    const size_t rowIdx4 = MakeIndex(Data, Size, offset);
    const std::string rowName4 = MakeString(Data, Size, offset);

    const size_t colIdx3 = MakeIndex(Data, Size, offset);
    const std::string colName3 = MakeString(Data, Size, offset);

    const size_t colIdx4 = MakeIndex(Data, Size, offset);
    const std::string colName4 = MakeString(Data, Size, offset);

    try { (void)doc.GetCell<std::string>(colIdx1, rowIdx1); } catch (...) {}
    try { (void)doc.GetCell<std::string>(colName1, rowName1); } catch (...) {}
    try { (void)doc.GetCell<std::string>(colIdx2, rowName2); } catch (...) {}
    try { (void)doc.GetCell<std::string>(colName2, rowIdx2); } catch (...) {}
    try { (void)doc.GetRow<std::string>(rowName3); } catch (...) {}
    try { (void)doc.GetRow<std::string>(rowIdx3); } catch (...) {}
    try { (void)doc.GetColumn<std::string>(colName3); } catch (...) {}
    try { (void)doc.GetColumn<std::string>(colIdx3); } catch (...) {}

    try { (void)doc.GetCell<long long>(colIdx1, rowIdx1); } catch (...) {}
    try { (void)doc.GetCell<long long>(colName1, rowName1); } catch (...) {}
    try { (void)doc.GetCell<long long>(colIdx2, rowName2); } catch (...) {}
    try { (void)doc.GetCell<long long>(colName2, rowIdx2); } catch (...) {}
    try { (void)doc.GetRow<long double>(rowName4); } catch (...) {}
    try { (void)doc.GetRow<long double>(rowIdx4); } catch (...) {}
    try { (void)doc.GetColumn<long double>(colName4); } catch (...) {}
    try { (void)doc.GetColumn<long double>(colIdx4); } catch (...) {}
  } catch (...) {
  }

  return 0;
}