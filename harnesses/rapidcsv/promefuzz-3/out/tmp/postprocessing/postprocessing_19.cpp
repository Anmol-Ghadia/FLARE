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

static std::string MakeString(const uint8_t* data, size_t size)
{
  return std::string(reinterpret_cast<const char*>(data), size);
}

static rapidcsv::LabelParams MakeLabelParams(const uint8_t* data, size_t size, size_t offset)
{
  rapidcsv::LabelParams params;
  int a = (size > offset) ? static_cast<int>(data[offset]) : 0;
  int b = (size > offset + 1) ? static_cast<int>(data[offset + 1]) : 0;

  params.mColumnNameIdx = (a % 5) - 1;

#ifdef __cpp_lib_clamp
  (void)b;
#else
  (void)b;
#endif

  return params;
}

static rapidcsv::SeparatorParams MakeSeparatorParams(const uint8_t* data, size_t size, size_t offset)
{
  rapidcsv::SeparatorParams params;
  char sep = (size > offset) ? static_cast<char>(data[offset]) : ',';
  if ((sep == '\0') || (sep == '\n') || (sep == '\r'))
    sep = ',';
  params.mSeparator = sep;
  params.mTrim = (size > offset + 1) ? ((data[offset + 1] & 1U) != 0) : false;
  return params;
}

static rapidcsv::ConverterParams MakeConverterParams(const uint8_t* data, size_t size, size_t offset)
{
  rapidcsv::ConverterParams params;
  params.mHasDefaultConverter = (size > offset) ? ((data[offset] & 1U) != 0) : true;
  params.mDefaultFloat = (size > offset + 1) ? static_cast<long double>(static_cast<int8_t>(data[offset + 1])) : 0.0L;
  params.mDefaultInteger = (size > offset + 2) ? static_cast<long long>(static_cast<int8_t>(data[offset + 2])) : 0;
  return params;
}

static rapidcsv::LineReaderParams MakeLineReaderParams(const uint8_t* data, size_t size, size_t offset)
{
  rapidcsv::LineReaderParams params;
  params.mSkipCommentLines = (size > offset) ? ((data[offset] & 1U) != 0) : false;
  char prefix = (size > offset + 1) ? static_cast<char>(data[offset + 1]) : '#';
  if ((prefix == '\0') || (prefix == '\n') || (prefix == '\r'))
    prefix = '#';
  params.mCommentPrefix = prefix;
  return params;
}

static void ExerciseDocument(const rapidcsv::Document& doc,
                             const std::string& fuzzStr,
                             size_t idxA,
                             size_t idxB)
{
  try { (void)doc.GetRow<std::string>(fuzzStr); } catch (...) {}
  try { (void)doc.GetRow<int>(fuzzStr); } catch (...) {}
  try { (void)doc.GetRow<double>(fuzzStr); } catch (...) {}

  try { (void)doc.GetRow<std::string>(idxA); } catch (...) {}
  try { (void)doc.GetRow<int>(idxA); } catch (...) {}
  try { (void)doc.GetRow<double>(idxA); } catch (...) {}

  try { (void)doc.GetRowName(idxA); } catch (...) {}
  try { (void)doc.GetRowName(idxB); } catch (...) {}

  try { (void)doc.GetCell<std::string>(fuzzStr, idxA); } catch (...) {}
  try { (void)doc.GetCell<int>(fuzzStr, idxA); } catch (...) {}
  try { (void)doc.GetCell<double>(fuzzStr, idxA); } catch (...) {}

  try { (void)doc.GetCell<std::string>(idxA, fuzzStr); } catch (...) {}
  try { (void)doc.GetCell<int>(idxA, fuzzStr); } catch (...) {}
  try { (void)doc.GetCell<double>(idxA, fuzzStr); } catch (...) {}

  try { (void)doc.GetColumn<std::string>(fuzzStr); } catch (...) {}
  try { (void)doc.GetColumn<int>(fuzzStr); } catch (...) {}
  try { (void)doc.GetColumn<double>(fuzzStr); } catch (...) {}

  static const char* commonNames[] = {
    "", "A", "B", "C", "col", "column", "row", "name", "id", "0", "1"
  };

  for (const char* s : commonNames)
  {
    try { (void)doc.GetRow<std::string>(std::string(s)); } catch (...) {}
    try { (void)doc.GetCell<std::string>(std::string(s), idxB); } catch (...) {}
    try { (void)doc.GetColumn<std::string>(std::string(s)); } catch (...) {}
    try { (void)doc.GetCell<std::string>(idxB, std::string(s)); } catch (...) {}
  }
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
  std::ofstream ofs("./dummy_file", std::ios::binary);
  if (ofs)
  {
    ofs.write(reinterpret_cast<const char*>(Data), static_cast<std::streamsize>(Size));
    ofs.close();
  }

  const std::string fuzzStr = MakeString(Data, Size);
  const size_t idxA = (Size > 0) ? static_cast<size_t>(Data[0]) : 0;
  const size_t idxB = (Size > 1) ? static_cast<size_t>(Data[1]) : 0;

  rapidcsv::LabelParams labelParams1 = MakeLabelParams(Data, Size, 0);
  rapidcsv::SeparatorParams sepParams1 = MakeSeparatorParams(Data, Size, 2);
  rapidcsv::ConverterParams convParams1 = MakeConverterParams(Data, Size, 4);
  rapidcsv::LineReaderParams lineParams1 = MakeLineReaderParams(Data, Size, 7);

  rapidcsv::LabelParams labelParams2 = MakeLabelParams(Data, Size, 9);
  labelParams2.mColumnNameIdx = -1;
  rapidcsv::SeparatorParams sepParams2 = MakeSeparatorParams(Data, Size, 11);
  rapidcsv::ConverterParams convParams2 = MakeConverterParams(Data, Size, 13);
  rapidcsv::LineReaderParams lineParams2 = MakeLineReaderParams(Data, Size, 16);

  try
  {
    rapidcsv::Document doc("./dummy_file", labelParams1, sepParams1, convParams1, lineParams1);
    ExerciseDocument(doc, fuzzStr, idxA, idxB);
  }
  catch (...)
  {
  }

  try
  {
    rapidcsv::Document doc("./dummy_file", labelParams2, sepParams2, convParams2, lineParams2);
    ExerciseDocument(doc, fuzzStr, idxB, idxA);
  }
  catch (...)
  {
  }

  try
  {
    rapidcsv::Document doc("./dummy_file");
    ExerciseDocument(doc, fuzzStr, idxA ^ idxB, idxA + idxB);
  }
  catch (...)
  {
  }

  return 0;
}