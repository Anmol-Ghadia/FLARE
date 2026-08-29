// This fuzz driver is generated for library rapidcsv, aiming to fuzz the following functions:
// rapidcsv::Document::SetRow at rapidcsv.h:1024:10 in rapidcsv.h
// rapidcsv::Document::GetRow at rapidcsv.h:944:20 in rapidcsv.h
// rapidcsv::Document::GetRow at rapidcsv.h:991:20 in rapidcsv.h
// rapidcsv::Document::Save at rapidcsv.h:597:10 in rapidcsv.h
// rapidcsv::Document::SetRow at rapidcsv.h:1062:10 in rapidcsv.h
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
#include "rapidcsv.h"

static std::string MakeString(const uint8_t* data, size_t size, size_t& offset)
{
  if (offset >= size) {
    return std::string();
  }

  size_t len = data[offset++];
  if (size > offset) {
    len %= (size - offset + 1);
  } else {
    len = 0;
  }

  std::string s;
  s.reserve(len);
  for (size_t i = 0; i < len && offset < size; ++i) {
    char c = static_cast<char>(data[offset++]);
    if (c == '\0') c = 'A';
    s.push_back(c);
  }
  return s;
}

static std::vector<std::string> MakeRow(const uint8_t* data, size_t size, size_t& offset)
{
  std::vector<std::string> row;
  size_t count = 0;
  if (offset < size) {
    count = data[offset++] % 8;
  }

  row.reserve(count);
  for (size_t i = 0; i < count; ++i) {
    row.push_back(MakeString(data, size, offset));
  }
  return row;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
  size_t offset = 0;

  std::string initialContent(reinterpret_cast<const char*>(Data), Size);
  {
    std::ofstream ofs("./dummy_file", std::ios::binary);
    ofs.write(initialContent.data(), static_cast<std::streamsize>(initialContent.size()));
  }

  rapidcsv::LabelParams labelParams;
  labelParams.mColumnNameIdx = (offset < Size) ? (static_cast<int>(Data[offset++]) % 3 - 1) : -1;

  rapidcsv::SeparatorParams separatorParams;
  separatorParams.mSeparator = (offset < Size) ? static_cast<char>(Data[offset++]) : ',';
  if (separatorParams.mSeparator == '\0' || separatorParams.mSeparator == '\n' || separatorParams.mSeparator == '\r') {
    separatorParams.mSeparator = ',';
  }
  separatorParams.mTrim = (offset < Size) ? ((Data[offset++] & 1) != 0) : false;

  rapidcsv::ConverterParams converterParams;
  converterParams.mHasDefaultConverter = (offset < Size) ? ((Data[offset++] & 1) != 0) : true;
  converterParams.mDefaultFloat = (offset < Size) ? static_cast<long double>(static_cast<int8_t>(Data[offset++])) : 0.0L;
  converterParams.mDefaultInteger = (offset < Size) ? static_cast<long long>(static_cast<int8_t>(Data[offset++])) : 0LL;

  rapidcsv::LineReaderParams lineReaderParams;
  lineReaderParams.mSkipCommentLines = (offset < Size) ? ((Data[offset++] & 1) != 0) : false;
  lineReaderParams.mCommentPrefix = (offset < Size) ? static_cast<char>(Data[offset++]) : '#';
  if (lineReaderParams.mCommentPrefix == '\0') {
    lineReaderParams.mCommentPrefix = '#';
  }

  try {
    rapidcsv::Document doc("./dummy_file", labelParams, separatorParams, converterParams, lineReaderParams);

    std::string rowName = MakeString(Data, Size, offset);
    std::vector<std::string> rowByName = MakeRow(Data, Size, offset);
    size_t rowIdx = (offset < Size) ? static_cast<size_t>(Data[offset++]) : 0;
    std::vector<std::string> rowByIdx = MakeRow(Data, Size, offset);

    try {
      doc.SetRow<std::string>(rowName, rowByName);
    } catch (...) {
    }

    try {
      doc.SetRow<std::string>(rowIdx, rowByIdx);
    } catch (...) {
    }

    try {
      volatile auto r1 = doc.GetRow<std::string>(rowIdx);
      (void)r1;
    } catch (...) {
    }

    try {
      volatile auto r2 = doc.GetRow<std::string>(rowName);
      (void)r2;
    } catch (...) {
    }

    try {
      doc.Save("./dummy_file");
    } catch (...) {
    }
  } catch (...) {
  }

  return 0;
}