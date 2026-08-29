// This fuzz driver is generated for library rapidcsv, aiming to fuzz the following functions:
// rapidcsv::Document::SetCell at rapidcsv.h:1391:10 in rapidcsv.h
// rapidcsv::Document::SetCell at rapidcsv.h:1391:10 in rapidcsv.h
// rapidcsv::Document::SetCell at rapidcsv.h:1391:10 in rapidcsv.h
// rapidcsv::Document::SetCell at rapidcsv.h:1409:10 in rapidcsv.h
// rapidcsv::Document::SetCell at rapidcsv.h:1409:10 in rapidcsv.h
// rapidcsv::Document::SetCell at rapidcsv.h:1409:10 in rapidcsv.h
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
#include <vector>
#include <sstream>
#include <fstream>
#include <limits>
#include "rapidcsv.h"

static uint64_t ReadU64(const uint8_t* Data, size_t Size, size_t& Offset)
{
  uint64_t v = 0;
  for (size_t i = 0; i < 8; ++i)
  {
    v <<= 8;
    if (Offset < Size)
    {
      v |= Data[Offset++];
    }
  }
  return v;
}

static bool ReadBool(const uint8_t* Data, size_t Size, size_t& Offset)
{
  if (Offset >= Size) return false;
  return (Data[Offset++] & 1) != 0;
}

static char ReadChar(const uint8_t* Data, size_t Size, size_t& Offset)
{
  if (Offset >= Size) return ',';
  return static_cast<char>(Data[Offset++]);
}

static size_t ReadSizeT(const uint8_t* Data, size_t Size, size_t& Offset)
{
  return static_cast<size_t>(ReadU64(Data, Size, Offset));
}

static std::string ReadString(const uint8_t* Data, size_t Size, size_t& Offset, size_t MaxLen = 32)
{
  size_t len = ReadSizeT(Data, Size, Offset);
  len = len % (MaxLen + 1);
  std::string s;
  s.reserve(len);
  for (size_t i = 0; i < len && Offset < Size; ++i)
  {
    char c = static_cast<char>(Data[Offset++]);
    if (c == '\0') c = 'A';
    s.push_back(c);
  }
  return s;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
  size_t offset = 0;

  rapidcsv::LabelParams labelParams;
  labelParams.mColumnNameIdx = ReadBool(Data, Size, offset) ? 0 : -1;

  rapidcsv::SeparatorParams separatorParams;
  separatorParams.mSeparator = ReadChar(Data, Size, offset);
  if (separatorParams.mSeparator == '\0' || separatorParams.mSeparator == '\n' || separatorParams.mSeparator == '\r')
  {
    separatorParams.mSeparator = ',';
  }
  separatorParams.mTrim = ReadBool(Data, Size, offset);

  rapidcsv::ConverterParams converterParams;
  converterParams.mHasDefaultConverter = ReadBool(Data, Size, offset);
  converterParams.mDefaultFloat = static_cast<long double>(static_cast<int64_t>(ReadU64(Data, Size, offset)));
  converterParams.mDefaultInteger = static_cast<long long>(static_cast<int64_t>(ReadU64(Data, Size, offset)));

  rapidcsv::LineReaderParams lineReaderParams;
  lineReaderParams.mSkipCommentLines = ReadBool(Data, Size, offset);
  lineReaderParams.mCommentPrefix = ReadChar(Data, Size, offset);
  if (lineReaderParams.mCommentPrefix == '\0' || lineReaderParams.mCommentPrefix == '\n' || lineReaderParams.mCommentPrefix == '\r')
  {
    lineReaderParams.mCommentPrefix = '#';
  }

  std::string csvData(reinterpret_cast<const char*>(Data + offset), Size - offset);
  std::stringstream ss(csvData);

  try
  {
    rapidcsv::Document doc(ss, labelParams, separatorParams, converterParams, lineReaderParams);

    std::string rowName1 = ReadString(Data, Size, offset);
    std::string rowName2 = ReadString(Data, Size, offset);
    std::string rowName3 = ReadString(Data, Size, offset);
    std::string colName1 = ReadString(Data, Size, offset);
    std::string colName2 = ReadString(Data, Size, offset);
    std::string colName3 = ReadString(Data, Size, offset);

    if (rowName1.empty()) rowName1 = "r1";
    if (rowName2.empty()) rowName2 = "r2";
    if (rowName3.empty()) rowName3 = "r3";
    if (colName1.empty()) colName1 = "c1";
    if (colName2.empty()) colName2 = "c2";
    if (colName3.empty()) colName3 = "c3";

    size_t colIdx1 = ReadSizeT(Data, Size, offset) % 8;
    size_t colIdx2 = ReadSizeT(Data, Size, offset) % 8;
    size_t colIdx3 = ReadSizeT(Data, Size, offset) % 8;
    size_t rowIdx1 = ReadSizeT(Data, Size, offset) % 8;
    size_t rowIdx2 = ReadSizeT(Data, Size, offset) % 8;
    size_t rowIdx3 = ReadSizeT(Data, Size, offset) % 8;

    std::string cellStr1 = ReadString(Data, Size, offset, 64);
    std::string cellStr2 = ReadString(Data, Size, offset, 64);
    int cellInt1 = static_cast<int>(ReadU64(Data, Size, offset));
    int cellInt2 = static_cast<int>(ReadU64(Data, Size, offset));
    double cellDouble1 = static_cast<double>(static_cast<int64_t>(ReadU64(Data, Size, offset)));
    double cellDouble2 = static_cast<double>(static_cast<int64_t>(ReadU64(Data, Size, offset)));

    try { doc.SetCell<std::string>(colIdx1, rowName1, cellStr1); } catch (...) {}
    try { doc.SetCell<int>(colIdx2, rowName2, cellInt1); } catch (...) {}
    try { doc.SetCell<double>(colIdx3, rowName3, cellDouble1); } catch (...) {}
    try { doc.SetCell<std::string>(colName1, rowIdx1, cellStr2); } catch (...) {}
    try { doc.SetCell<int>(colName2, rowIdx2, cellInt2); } catch (...) {}
    try { doc.SetCell<double>(colName3, rowIdx3, cellDouble2); } catch (...) {}

    try { (void)doc.GetCell<std::string>(colIdx1, rowIdx1); } catch (...) {}
    try { (void)doc.GetCell<int>(colIdx2, rowIdx2); } catch (...) {}
    try { (void)doc.GetCell<double>(colIdx3, rowIdx3); } catch (...) {}
    try { (void)doc.GetCell<std::string>(colName1, rowName1); } catch (...) {}
    try { (void)doc.GetCell<int>(colName2, rowName2); } catch (...) {}
    try { (void)doc.GetCell<double>(colName3, rowName3); } catch (...) {}

    try
    {
      std::ofstream out("./dummy_file", std::ios::binary);
      out.write(csvData.data(), static_cast<std::streamsize>(csvData.size()));
    }
    catch (...)
    {
    }

    try { doc.Save("./dummy_file"); } catch (...) {}
  }
  catch (...)
  {
  }

  return 0;
}