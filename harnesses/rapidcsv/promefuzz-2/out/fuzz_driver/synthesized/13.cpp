// This fuzz driver is generated for library rapidcsv, aiming to fuzz the following functions:
// rapidcsv::Document::GetCell at rapidcsv.h:1294:7 in rapidcsv.h
// rapidcsv::Converter::ToVal at rapidcsv.h:184:10 in rapidcsv.h
// rapidcsv::Converter::ToVal at rapidcsv.h:184:10 in rapidcsv.h
// rapidcsv::Converter::ToVal at rapidcsv.h:184:10 in rapidcsv.h
// rapidcsv::Converter::ToVal at rapidcsv.h:184:10 in rapidcsv.h
// rapidcsv::Converter::ToVal at rapidcsv.h:184:10 in rapidcsv.h
// rapidcsv::Converter::ToVal at rapidcsv.h:184:10 in rapidcsv.h
// rapidcsv::Converter::ToVal at rapidcsv.h:184:10 in rapidcsv.h
// rapidcsv::Converter::ToVal at rapidcsv.h:184:10 in rapidcsv.h
// rapidcsv::Converter::ToVal at rapidcsv.h:184:10 in rapidcsv.h
// rapidcsv::Converter::ToVal at rapidcsv.h:184:10 in rapidcsv.h
// rapidcsv::Converter::ToVal at rapidcsv.h:184:10 in rapidcsv.h
// rapidcsv::Converter::ToVal at rapidcsv.h:184:10 in rapidcsv.h
// rapidcsv::Converter::ToVal at rapidcsv.h:184:10 in rapidcsv.h
// rapidcsv::Converter::ToVal at rapidcsv.h:184:10 in rapidcsv.h
// rapidcsv::Converter::ToVal at rapidcsv.h:184:10 in rapidcsv.h
// rapidcsv::Converter::ToVal at rapidcsv.h:184:10 in rapidcsv.h
// rapidcsv::Converter::ToVal at rapidcsv.h:184:10 in rapidcsv.h
// rapidcsv::Converter::ToVal at rapidcsv.h:184:10 in rapidcsv.h
// rapidcsv::Converter::ToVal at rapidcsv.h:184:10 in rapidcsv.h
// rapidcsv::Converter::ToVal at rapidcsv.h:184:10 in rapidcsv.h
// rapidcsv::Converter<std::basic_string<char>>::ToVal at rapidcsv.h:329:39 in rapidcsv.h
// rapidcsv::Converter<std::basic_string<char>>::ToVal at rapidcsv.h:329:39 in rapidcsv.h
// rapidcsv::Converter<std::basic_string<char>>::ToVal at rapidcsv.h:329:39 in rapidcsv.h
// rapidcsv::Document::GetRow at rapidcsv.h:991:20 in rapidcsv.h
// rapidcsv::Document::GetRow at rapidcsv.h:991:20 in rapidcsv.h
// rapidcsv::Document::GetRow at rapidcsv.h:991:20 in rapidcsv.h
// rapidcsv::Document::GetRow at rapidcsv.h:991:20 in rapidcsv.h
// rapidcsv::Document::GetRow at rapidcsv.h:991:20 in rapidcsv.h
// rapidcsv::Document::GetRow at rapidcsv.h:991:20 in rapidcsv.h
// rapidcsv::Document::GetColumn at rapidcsv.h:713:20 in rapidcsv.h
// rapidcsv::Document::GetColumn at rapidcsv.h:713:20 in rapidcsv.h
// rapidcsv::Document::GetColumn at rapidcsv.h:713:20 in rapidcsv.h
// rapidcsv::Document::GetColumn at rapidcsv.h:713:20 in rapidcsv.h
// rapidcsv::Document::GetColumn at rapidcsv.h:713:20 in rapidcsv.h
// rapidcsv::Document::GetColumn at rapidcsv.h:713:20 in rapidcsv.h
// rapidcsv::Document::GetRow at rapidcsv.h:991:20 in rapidcsv.h
// rapidcsv::Document::GetColumn at rapidcsv.h:713:20 in rapidcsv.h
// rapidcsv::Document::GetCell at rapidcsv.h:1257:7 in rapidcsv.h
// rapidcsv::Document::GetCell at rapidcsv.h:1257:7 in rapidcsv.h
// rapidcsv::Document::GetCell at rapidcsv.h:1257:7 in rapidcsv.h
// rapidcsv::Document::GetCell at rapidcsv.h:1294:7 in rapidcsv.h
// rapidcsv::Document::GetCell at rapidcsv.h:1294:7 in rapidcsv.h
// rapidcsv::Document::GetCell at rapidcsv.h:1294:7 in rapidcsv.h
// rapidcsv::Document::GetCell at rapidcsv.h:1294:7 in rapidcsv.h
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
#include <limits>
#include "rapidcsv.h"

static std::string MakeString(const uint8_t* data, size_t size)
{
  return std::string(reinterpret_cast<const char*>(data), size);
}

static char PickChar(const uint8_t* data, size_t size, size_t idx, char fallback)
{
  if (idx < size) {
    char c = static_cast<char>(data[idx]);
    if (c == '\0') {
      return fallback;
    }
    return c;
  }
  return fallback;
}

extern "C" int LLVMFuzzerTestOneInput_13(const uint8_t *Data, size_t Size)
{
  std::string input = MakeString(Data, Size);

  {
    std::ofstream ofs("./dummy_file", std::ios::binary);
    ofs.write(input.data(), static_cast<std::streamsize>(input.size()));
  }

  rapidcsv::LabelParams labelParams;
  rapidcsv::SeparatorParams separatorParams;
  rapidcsv::ConverterParams converterParams;
  rapidcsv::LineReaderParams lineReaderParams;

  if (Size > 0) {
    labelParams.mColumnNameIdx = static_cast<int>(Data[0] % 4) - 1;
  } else {
    labelParams.mColumnNameIdx = 0;
  }

  separatorParams.mSeparator = PickChar(Data, Size, 1, ',');
  if (separatorParams.mSeparator == '\n' || separatorParams.mSeparator == '\r') {
    separatorParams.mSeparator = ',';
  }
  separatorParams.mTrim = (Size > 2) ? ((Data[2] & 1) != 0) : false;

  converterParams.mHasDefaultConverter = (Size > 3) ? ((Data[3] & 1) != 0) : false;
  converterParams.mDefaultFloat = (Size > 4) ? static_cast<long double>(static_cast<int8_t>(Data[4])) : 0.0L;
  converterParams.mDefaultInteger = (Size > 5) ? static_cast<long long>(static_cast<int8_t>(Data[5])) : 0LL;

  lineReaderParams.mSkipCommentLines = (Size > 6) ? ((Data[6] & 1) != 0) : false;
  lineReaderParams.mCommentPrefix = PickChar(Data, Size, 7, '#');
  if (lineReaderParams.mCommentPrefix == '\0' || lineReaderParams.mCommentPrefix == '\n' || lineReaderParams.mCommentPrefix == '\r') {
    lineReaderParams.mCommentPrefix = '#';
  }

  try {
    rapidcsv::Document doc("./dummy_file", labelParams, separatorParams, converterParams, lineReaderParams);

    try { (void)doc.GetRow<std::string>(""); } catch (...) {}
    try { (void)doc.GetRow<std::string>("0"); } catch (...) {}
    try { (void)doc.GetRow<std::string>("row"); } catch (...) {}
    try { (void)doc.GetRow<int>(""); } catch (...) {}
    try { (void)doc.GetRow<long long>("1"); } catch (...) {}
    try { (void)doc.GetRow<double>("row"); } catch (...) {}

    try { (void)doc.GetColumn<std::string>(""); } catch (...) {}
    try { (void)doc.GetColumn<std::string>("0"); } catch (...) {}
    try { (void)doc.GetColumn<std::string>("col"); } catch (...) {}
    try { (void)doc.GetColumn<int>(""); } catch (...) {}
    try { (void)doc.GetColumn<float>("1"); } catch (...) {}
    try { (void)doc.GetColumn<double>("col"); } catch (...) {}

    std::string fuzzLabel = input.substr(0, input.size() > 32 ? 32 : input.size());
    try { (void)doc.GetRow<std::string>(fuzzLabel); } catch (...) {}
    try { (void)doc.GetColumn<std::string>(fuzzLabel); } catch (...) {}
    try { (void)doc.GetCell<std::string>(fuzzLabel, static_cast<size_t>(0)); } catch (...) {}
    try { (void)doc.GetCell<int>(fuzzLabel, static_cast<size_t>(1)); } catch (...) {}
    try { (void)doc.GetCell<double>(fuzzLabel, static_cast<size_t>(2)); } catch (...) {}

    try { (void)doc.GetCell<std::string>(static_cast<size_t>(0), std::string("")); } catch (...) {}
    try { (void)doc.GetCell<int>(static_cast<size_t>(1), std::string("0")); } catch (...) {}
    try { (void)doc.GetCell<double>(static_cast<size_t>(2), std::string("row")); } catch (...) {}
    try { (void)doc.GetCell<std::string>(static_cast<size_t>(3), fuzzLabel); } catch (...) {}
    try { (void)doc.GetCell<long long>(static_cast<size_t>(4), fuzzLabel); } catch (...) {}
  } catch (...) {
  }

  try {
    rapidcsv::Converter<int> convInt(converterParams);
    int vi = 0;
    try { convInt.ToVal(input, vi); } catch (...) {}
    try { convInt.ToVal(std::string(), vi); } catch (...) {}
    try { convInt.ToVal(std::string("0"), vi); } catch (...) {}
    try { convInt.ToVal(std::string("-1"), vi); } catch (...) {}
    try { convInt.ToVal(std::string("999999999999999999999999"), vi); } catch (...) {}
  } catch (...) {
  }

  try {
    rapidcsv::Converter<long long> convLL(converterParams);
    long long vll = 0;
    try { convLL.ToVal(input, vll); } catch (...) {}
    try { convLL.ToVal(std::string("9223372036854775807"), vll); } catch (...) {}
    try { convLL.ToVal(std::string("-9223372036854775808"), vll); } catch (...) {}
    try { convLL.ToVal(std::string("18446744073709551616"), vll); } catch (...) {}
  } catch (...) {
  }

  try {
    rapidcsv::Converter<unsigned int> convUInt(converterParams);
    unsigned int vui = 0;
    try { convUInt.ToVal(input, vui); } catch (...) {}
    try { convUInt.ToVal(std::string("-1"), vui); } catch (...) {}
    try { convUInt.ToVal(std::string("4294967296"), vui); } catch (...) {}
  } catch (...) {
  }

  try {
    rapidcsv::Converter<float> convFloat(converterParams);
    float vf = 0.0f;
    try { convFloat.ToVal(input, vf); } catch (...) {}
    try { convFloat.ToVal(std::string("nan"), vf); } catch (...) {}
    try { convFloat.ToVal(std::string("inf"), vf); } catch (...) {}
    try { convFloat.ToVal(std::string("1.2345e39"), vf); } catch (...) {}
  } catch (...) {
  }

  try {
    rapidcsv::Converter<double> convDouble(converterParams);
    double vd = 0.0;
    try { convDouble.ToVal(input, vd); } catch (...) {}
    try { convDouble.ToVal(std::string("0.0"), vd); } catch (...) {}
    try { convDouble.ToVal(std::string("-123.456"), vd); } catch (...) {}
    try { convDouble.ToVal(std::string("1e309"), vd); } catch (...) {}
  } catch (...) {
  }

  try {
    rapidcsv::Converter<std::string> convString(converterParams);
    std::string vs;
    try { convString.ToVal(input, vs); } catch (...) {}
    try { convString.ToVal(std::string(), vs); } catch (...) {}
    try { convString.ToVal(std::string("text"), vs); } catch (...) {}
  } catch (...) {
  }

  return 0;
}