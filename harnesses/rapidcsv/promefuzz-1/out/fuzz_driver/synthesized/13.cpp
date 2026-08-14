// This fuzz driver is generated for library rapidcsv, aiming to fuzz the following functions:
// rapidcsv::Converter::ToVal at rapidcsv.h:184:10 in rapidcsv.h
// rapidcsv::Converter<std::basic_string<char>>::ToVal at rapidcsv.h:329:39 in rapidcsv.h
// rapidcsv::Document::GetRow at rapidcsv.h:944:20 in rapidcsv.h
// rapidcsv::Document::GetRow at rapidcsv.h:944:20 in rapidcsv.h
// rapidcsv::Document::GetRow at rapidcsv.h:944:20 in rapidcsv.h
// rapidcsv::Document::GetRow at rapidcsv.h:991:20 in rapidcsv.h
// rapidcsv::Document::GetRow at rapidcsv.h:991:20 in rapidcsv.h
// rapidcsv::Document::GetCell at rapidcsv.h:1257:7 in rapidcsv.h
// rapidcsv::Document::GetCell at rapidcsv.h:1257:7 in rapidcsv.h
// rapidcsv::Document::GetCell at rapidcsv.h:1257:7 in rapidcsv.h
// rapidcsv::Document::GetCell at rapidcsv.h:1294:7 in rapidcsv.h
// rapidcsv::Document::GetCell at rapidcsv.h:1294:7 in rapidcsv.h
// rapidcsv::Document::GetCell at rapidcsv.h:1294:7 in rapidcsv.h
// rapidcsv::Converter::ToVal at rapidcsv.h:184:10 in rapidcsv.h
// rapidcsv::Converter::ToVal at rapidcsv.h:184:10 in rapidcsv.h
// rapidcsv::Converter::ToVal at rapidcsv.h:184:10 in rapidcsv.h
// rapidcsv::Converter::ToVal at rapidcsv.h:184:10 in rapidcsv.h
// rapidcsv::Converter::ToVal at rapidcsv.h:184:10 in rapidcsv.h
// rapidcsv::Converter::ToVal at rapidcsv.h:184:10 in rapidcsv.h
// rapidcsv::Converter::ToVal at rapidcsv.h:184:10 in rapidcsv.h
// rapidcsv::Converter::ToVal at rapidcsv.h:184:10 in rapidcsv.h
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
#include <limits>
#include "rapidcsv.h"

static std::string MakeString(const uint8_t* Data, size_t Size)
{
  return std::string(reinterpret_cast<const char*>(Data), Size);
}

extern "C" int LLVMFuzzerTestOneInput_13(const uint8_t *Data, size_t Size)
{
  std::string input = MakeString(Data, Size);

  {
    rapidcsv::ConverterParams convParams;
    if (Size > 0) convParams.mHasDefaultConverter = (Data[0] & 1) != 0;
    if (Size > 1) convParams.mDefaultInteger = static_cast<long long>(static_cast<int8_t>(Data[1]));
    if (Size > 2) convParams.mDefaultFloat = static_cast<long double>(static_cast<int8_t>(Data[2]));

    try
    {
      rapidcsv::Converter<int> c(convParams);
      int v = 0;
      c.ToVal(input, v);
    }
    catch (...) {}

    try
    {
      rapidcsv::Converter<short> c(convParams);
      short v = 0;
      c.ToVal(input, v);
    }
    catch (...) {}

    try
    {
      rapidcsv::Converter<unsigned short> c(convParams);
      unsigned short v = 0;
      c.ToVal(input, v);
    }
    catch (...) {}

    try
    {
      rapidcsv::Converter<long> c(convParams);
      long v = 0;
      c.ToVal(input, v);
    }
    catch (...) {}

    try
    {
      rapidcsv::Converter<long long> c(convParams);
      long long v = 0;
      c.ToVal(input, v);
    }
    catch (...) {}

    try
    {
      rapidcsv::Converter<unsigned int> c(convParams);
      unsigned int v = 0;
      c.ToVal(input, v);
    }
    catch (...) {}

    try
    {
      rapidcsv::Converter<float> c(convParams);
      float v = 0.0f;
      c.ToVal(input, v);
    }
    catch (...) {}

    try
    {
      rapidcsv::Converter<double> c(convParams);
      double v = 0.0;
      c.ToVal(input, v);
    }
    catch (...) {}

    try
    {
      rapidcsv::Converter<long double> c(convParams);
      long double v = 0.0L;
      c.ToVal(input, v);
    }
    catch (...) {}

    try
    {
      rapidcsv::Converter<std::string> c(convParams);
      std::string v;
      c.ToVal(input, v);
    }
    catch (...) {}
  }

  {
    std::ofstream ofs("./dummy_file", std::ios::binary);
    ofs.write(reinterpret_cast<const char*>(Data), static_cast<std::streamsize>(Size));
  }

  rapidcsv::LabelParams labelParams;
  rapidcsv::SeparatorParams sepParams;
  rapidcsv::ConverterParams convParams;
  rapidcsv::LineReaderParams lineParams;

  if (Size > 0) labelParams.mColumnNameIdx = static_cast<int>(Data[0] % 4) - 1;
  if (Size > 1) sepParams.mSeparator = static_cast<char>(Data[1]);
  if (sepParams.mSeparator == '\0') sepParams.mSeparator = ',';
  if (Size > 2) sepParams.mTrim = (Data[2] & 1) != 0;
  if (Size > 3) convParams.mHasDefaultConverter = (Data[3] & 1) != 0;
  if (Size > 4) convParams.mDefaultInteger = static_cast<long long>(static_cast<int8_t>(Data[4]));
  if (Size > 5) convParams.mDefaultFloat = static_cast<long double>(static_cast<int8_t>(Data[5]));
  if (Size > 6) lineParams.mSkipCommentLines = (Data[6] & 1) != 0;
  if (Size > 7) lineParams.mCommentPrefix = static_cast<char>(Data[7]);
  if (lineParams.mCommentPrefix == '\0') lineParams.mCommentPrefix = '#';

  try
  {
    rapidcsv::Document doc("./dummy_file", labelParams, sepParams, convParams, lineParams);

    try
    {
      std::vector<std::string> row0 = doc.GetRow<std::string>(static_cast<size_t>(0));
      (void)row0;
    }
    catch (...) {}

    try
    {
      std::vector<int> row0 = doc.GetRow<int>(static_cast<size_t>(0));
      (void)row0;
    }
    catch (...) {}

    try
    {
      std::vector<double> row1 = doc.GetRow<double>(static_cast<size_t>(1));
      (void)row1;
    }
    catch (...) {}

    std::string rowName = input;
    if (rowName.size() > 64) rowName.resize(64);

    try
    {
      std::vector<std::string> rowByName = doc.GetRow<std::string>(rowName);
      (void)rowByName;
    }
    catch (...) {}

    try
    {
      std::vector<long long> rowByName = doc.GetRow<long long>(rowName);
      (void)rowByName;
    }
    catch (...) {}

    std::string colName = input;
    if (colName.size() > 64) colName.resize(64);

    try
    {
      std::string cell = doc.GetCell<std::string>(colName, static_cast<size_t>(0));
      (void)cell;
    }
    catch (...) {}

    try
    {
      int cell = doc.GetCell<int>(colName, static_cast<size_t>(0));
      (void)cell;
    }
    catch (...) {}

    try
    {
      double cell = doc.GetCell<double>(colName, static_cast<size_t>(1));
      (void)cell;
    }
    catch (...) {}

    try
    {
      std::string cell = doc.GetCell<std::string>(static_cast<size_t>(0), rowName);
      (void)cell;
    }
    catch (...) {}

    try
    {
      int cell = doc.GetCell<int>(static_cast<size_t>(0), rowName);
      (void)cell;
    }
    catch (...) {}

    try
    {
      long long cell = doc.GetCell<long long>(static_cast<size_t>(1), rowName);
      (void)cell;
    }
    catch (...) {}
  }
  catch (...) {}

  return 0;
}