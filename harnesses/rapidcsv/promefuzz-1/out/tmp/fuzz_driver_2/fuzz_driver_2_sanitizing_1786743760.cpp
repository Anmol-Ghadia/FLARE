#include <cstddef>
#include <cstdint>
#include <fstream>
#include <functional>
#include <sstream>
#include <string>
#include <vector>

#include "rapidcsv.h"

static void ConvInt(const std::string& s, int& out)
{
  try
  {
    out = std::stoi(s);
  }
  catch (...)
  {
    out = 0;
  }
}

static void ConvDouble(const std::string& s, double& out)
{
  try
  {
    out = std::stod(s);
  }
  catch (...)
  {
    out = 0.0;
  }
}

static void ConvString(const std::string& s, std::string& out)
{
  out = s;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* Data, size_t Size)
{
  {
    std::ofstream ofs("./dummy_file", std::ios::binary);
    if (ofs)
    {
      ofs.write(reinterpret_cast<const char*>(Data), static_cast<std::streamsize>(Size));
    }
  }

  std::string input(reinterpret_cast<const char*>(Data), Size);
  std::istringstream iss(input);

  rapidcsv::LabelParams labelParams;
  labelParams.mColumnNameIdx = (Size > 0) ? (static_cast<int>(Data[0] % 3) - 1) : 0;

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

    const size_t idx1 = (Size > 8) ? static_cast<size_t>(Data[8]) : 0;
    const size_t idx2 = (Size > 9) ? static_cast<size_t>(Data[9]) : 0;
    const size_t idx3 = (Size > 10) ? static_cast<size_t>(Data[10]) : 0;
    const size_t idx4 = (Size > 11) ? static_cast<size_t>(Data[11]) : 0;

    const size_t q1 = Size / 4;
    const size_t q2 = Size / 2;
    const size_t q3 = (Size * 3) / 4;

    const std::string name1 = input.substr(0, q1);
    const std::string name2 = input.substr(q1, q2 - q1);
    const std::string name3 = input.substr(q2, q3 - q2);
    const std::string name4 = input.substr(q3);

    try { (void)doc.GetCell<int>(idx1, idx2, ConvInt); } catch (...) {}
    try { (void)doc.GetCell<std::string>(name1, name2, ConvString); } catch (...) {}
    try { (void)doc.GetCell<double>(idx3, name3, ConvDouble); } catch (...) {}
    try { (void)doc.GetCell<std::string>(name4, idx4, ConvString); } catch (...) {}

    try { (void)doc.GetRow<int>(idx1, ConvInt); } catch (...) {}
    try { (void)doc.GetRow<std::string>(name1, ConvString); } catch (...) {}

    try { (void)doc.GetColumn<double>(idx2, ConvDouble); } catch (...) {}
    try { (void)doc.GetColumn<std::string>(name2, ConvString); } catch (...) {}

    try { (void)doc.GetCell<std::string>(idx4, idx3, ConvString); } catch (...) {}
    try { (void)doc.GetCell<int>(name2, name1, ConvInt); } catch (...) {}
    try { (void)doc.GetCell<double>(idx2, name4, ConvDouble); } catch (...) {}
    try { (void)doc.GetCell<std::string>(name3, idx1, ConvString); } catch (...) {}

    try { (void)doc.GetRow<double>(idx3, ConvDouble); } catch (...) {}
    try { (void)doc.GetRow<std::string>(name4, ConvString); } catch (...) {}

    try { (void)doc.GetColumn<int>(idx4, ConvInt); } catch (...) {}
    try { (void)doc.GetColumn<std::string>(name3, ConvString); } catch (...) {}
  }
  catch (...)
  {
  }

  return 0;
}