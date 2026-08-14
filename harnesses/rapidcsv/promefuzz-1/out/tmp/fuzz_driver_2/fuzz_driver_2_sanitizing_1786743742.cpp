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
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <functional>

#include "rapidcsv.h"

static int ToInt(const std::string& s)
{
  try
  {
    return std::stoi(s);
  }
  catch (...)
  {
    return 0;
  }
}

static double ToDouble(const std::string& s)
{
  try
  {
    return std::stod(s);
  }
  catch (...)
  {
    return 0.0;
  }
}

static std::string ToString(const std::string& s)
{
  return s;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* Data, size_t Size)
{
  std::ofstream ofs("./dummy_file", std::ios::binary);
  if (ofs)
  {
    ofs.write(reinterpret_cast<const char*>(Data), static_cast<std::streamsize>(Size));
    ofs.close();
  }

  std::string input(reinterpret_cast<const char*>(Data), Size);
  std::istringstream iss(input);

  rapidcsv::LabelParams labelParams;
  labelParams.mColumnNameIdx = (Size > 0) ? static_cast<int>(Data[0] % 3) - 1 : 0;

  rapidcsv::SeparatorParams separatorParams;
  separatorParams.mSeparator = (Size > 1 && Data[1] != 0) ? static_cast<char>(Data[1]) : ',';
  separatorParams.mTrim = (Size > 2) ? ((Data[2] & 1) != 0) : false;

  rapidcsv::ConverterParams converterParams;
  converterParams.mHasDefaultConverter = (Size > 3) ? ((Data[3] & 1) != 0) : true;
  converterParams.mDefaultFloat = (Size > 4) ? static_cast<long double>(Data[4]) : 0.0L;
  converterParams.mDefaultInteger = (Size > 5) ? static_cast<long long>(Data[5]) : 0LL;

  rapidcsv::LineReaderParams lineReaderParams;
  lineReaderParams.mSkipCommentLines = (Size > 6) ? ((Data[6] & 1) != 0) : false;
  lineReaderParams.mCommentPrefix = (Size > 7 && Data[7] != 0) ? static_cast<char>(Data[7]) : '#';

  try
  {
    rapidcsv::Document doc(iss, labelParams, separatorParams, converterParams, lineReaderParams);

    size_t idx1 = (Size > 8) ? static_cast<size_t>(Data[8]) : 0;
    size_t idx2 = (Size > 9) ? static_cast<size_t>(Data[9]) : 0;
    size_t idx3 = (Size > 10) ? static_cast<size_t>(Data[10]) : 0;
    size_t idx4 = (Size > 11) ? static_cast<size_t>(Data[11]) : 0;

    std::string name1 = input.substr(0, Size > 0 ? (Size / 4) : 0);
    std::string name2 = (Size > 0) ? input.substr(Size / 4, Size > 1 ? (Size / 4) : 0) : std::string();

    try { (void)doc.GetCell<int>(idx1, idx2, ToInt); } catch (...) {}
    try { (void)doc.GetCell<std::string>(name1, name2, ToString); } catch (...) {}
    try { (void)doc.GetCell<double>(idx3, name1, ToDouble); } catch (...) {}
    try { (void)doc.GetCell<std::string>(name2, idx4, ToString); } catch (...) {}

    try { (void)doc.GetRow<int>(idx1, ToInt); } catch (...) {}
    try { (void)doc.GetRow<std::string>(name1, ToString); } catch (...) {}

    try { (void)doc.GetColumn<double>(idx2, ToDouble); } catch (...) {}
    try { (void)doc.GetColumn<std::string>(name2, ToString); } catch (...) {}

    try { (void)doc.GetCell<std::string>(idx4, idx3, ToString); } catch (...) {}
    try { (void)doc.GetCell<int>(name2, name1, ToInt); } catch (...) {}
    try { (void)doc.GetCell<double>(idx2, name2, ToDouble); } catch (...) {}
    try { (void)doc.GetCell<std::string>(name1, idx1, ToString); } catch (...) {}

    try { (void)doc.GetRow<double>(idx3, ToDouble); } catch (...) {}
    try { (void)doc.GetRow<std::string>(name2, ToString); } catch (...) {}

    try { (void)doc.GetColumn<int>(idx4, ToInt); } catch (...) {}
    try { (void)doc.GetColumn<std::string>(name1, ToString); } catch (...) {}
  }
  catch (...)
  {
  }

  return 0;
}