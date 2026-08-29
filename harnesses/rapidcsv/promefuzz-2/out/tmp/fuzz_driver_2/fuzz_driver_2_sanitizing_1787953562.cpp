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
#include <functional>

#include "rapidcsv.h"

static std::string BytesToString(const uint8_t* Data, size_t Size)
{
  return std::string(reinterpret_cast<const char*>(Data), Size);
}

static size_t PickIndex(const uint8_t* Data, size_t Size, size_t offset)
{
  if (Size == 0) return 0;
  size_t v = 0;
  for (size_t i = 0; i < sizeof(size_t) && (offset + i) < Size; ++i)
  {
    v = (v << 8) ^ static_cast<size_t>(Data[offset + i]);
  }
  return v;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
  std::ofstream ofs("./dummy_file", std::ios::binary);
  if (ofs)
  {
    ofs.write(reinterpret_cast<const char*>(Data), static_cast<std::streamsize>(Size));
    ofs.close();
  }

  std::string csv = BytesToString(Data, Size);
  std::istringstream iss(csv);

  rapidcsv::LabelParams labelParams;
  labelParams.mColumnNameIdx = (Size > 0) ? (static_cast<int>(Data[0]) % 3 - 1) : 0;

  rapidcsv::SeparatorParams sepParams;
  sepParams.mSeparator = (Size > 1 && Data[1] != '\n' && Data[1] != '\r' && Data[1] != '\0')
                           ? static_cast<char>(Data[1])
                           : ',';
  sepParams.mTrim = (Size > 2) ? ((Data[2] & 1) != 0) : false;

  rapidcsv::ConverterParams convParams;
  convParams.mHasDefaultConverter = (Size > 3) ? ((Data[3] & 1) != 0) : true;
  convParams.mDefaultFloat = (Size > 4) ? static_cast<long double>(static_cast<int8_t>(Data[4])) : 0.0L;
  convParams.mDefaultInteger = (Size > 5) ? static_cast<long long>(static_cast<int8_t>(Data[5])) : 0LL;

  rapidcsv::LineReaderParams lineParams;
  lineParams.mSkipCommentLines = (Size > 6) ? ((Data[6] & 1) != 0) : false;
  lineParams.mCommentPrefix = (Size > 7) ? static_cast<char>(Data[7]) : '#';

  try
  {
    rapidcsv::Document doc(iss, labelParams, sepParams, convParams, lineParams);

    const size_t idxA = PickIndex(Data, Size, 8);
    const size_t idxB = PickIndex(Data, Size, 16);
    const size_t idxC = PickIndex(Data, Size, 24);
    const size_t idxD = PickIndex(Data, Size, 32);

    const std::string nameA = (Size > 0) ? csv.substr(0, Size / 4) : std::string();
    const std::string nameB = (Size > 1) ? csv.substr(Size / 4, Size / 4) : std::string();
    const std::string nameC = (Size > 2) ? csv.substr(Size / 2, Size / 4) : std::string();
    const std::string nameD = (Size > 3) ? csv.substr((Size * 3) / 4) : std::string();

    auto toInt = [](const std::string& s) -> int {
      try { return std::stoi(s); } catch (...) { return 0; }
    };
    auto toString = [](const std::string& s) -> std::string {
      return s;
    };

    try { (void)doc.GetCell<std::string>(nameA, idxA, toString); } catch (...) {}
    try { (void)doc.GetCell<std::string>(nameB, nameC, toString); } catch (...) {}
    try { (void)doc.GetCell<int>(idxB, idxC, toInt); } catch (...) {}
    try { (void)doc.GetCell<int>(idxD, nameD, toInt); } catch (...) {}

    try { (void)doc.GetRow<std::string>(idxA, toString); } catch (...) {}
    try { (void)doc.GetRow<int>(nameA, toInt); } catch (...) {}

    try { (void)doc.GetColumn<std::string>(nameB, toString); } catch (...) {}
    try { (void)doc.GetColumn<int>(idxB, toInt); } catch (...) {}

    try { (void)doc.GetCell<int>(nameC, idxD, toInt); } catch (...) {}
    try { (void)doc.GetCell<std::string>(nameD, nameA, toString); } catch (...) {}
    try { (void)doc.GetCell<std::string>(idxA, idxB, toString); } catch (...) {}
    try { (void)doc.GetCell<int>(idxC, nameB, toInt); } catch (...) {}

    try { (void)doc.GetRow<int>(idxC, toInt); } catch (...) {}
    try { (void)doc.GetRow<std::string>(nameD, toString); } catch (...) {}

    try { (void)doc.GetColumn<int>(nameA, toInt); } catch (...) {}
    try { (void)doc.GetColumn<std::string>(idxD, toString); } catch (...) {}
  }
  catch (...)
  {
  }

  return 0;
}