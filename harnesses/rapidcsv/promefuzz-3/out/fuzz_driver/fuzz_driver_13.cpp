// This fuzz driver is generated for library rapidcsv, aiming to fuzz the following functions:
// rapidcsv::no_converter::what at rapidcsv.h:102:17 in rapidcsv.h
// rapidcsv::Document::GetRow at rapidcsv.h:968:20 in rapidcsv.h
// rapidcsv::Document::GetRow at rapidcsv.h:968:20 in rapidcsv.h
// rapidcsv::no_converter::what at rapidcsv.h:102:17 in rapidcsv.h
// rapidcsv::Document::GetCell at rapidcsv.h:1191:7 in rapidcsv.h
// rapidcsv::Document::GetCell at rapidcsv.h:1191:7 in rapidcsv.h
// rapidcsv::no_converter::what at rapidcsv.h:102:17 in rapidcsv.h
// rapidcsv::Document::GetCell at rapidcsv.h:1313:7 in rapidcsv.h
// rapidcsv::Document::GetCell at rapidcsv.h:1313:7 in rapidcsv.h
// rapidcsv::no_converter::what at rapidcsv.h:102:17 in rapidcsv.h
// rapidcsv::Document::GetColumn at rapidcsv.h:691:20 in rapidcsv.h
// rapidcsv::Document::GetColumn at rapidcsv.h:691:20 in rapidcsv.h
// rapidcsv::no_converter::what at rapidcsv.h:102:17 in rapidcsv.h
// rapidcsv::Document::GetColumn at rapidcsv.h:730:20 in rapidcsv.h
// rapidcsv::Document::GetColumn at rapidcsv.h:730:20 in rapidcsv.h
// rapidcsv::no_converter::what at rapidcsv.h:102:17 in rapidcsv.h
// rapidcsv::no_converter::what at rapidcsv.h:102:17 in rapidcsv.h
#include <cstddef>
#include <cstdint>
#include <exception>
#include <fstream>
#include <string>
#include <vector>

#include "rapidcsv.h"

static std::string MakeString(const uint8_t* data, size_t size)
{
  return std::string(reinterpret_cast<const char*>(data), size);
}

static size_t PickIndex(const uint8_t* data, size_t size, size_t offset)
{
  if (size == 0)
  {
    return 0;
  }
  return static_cast<size_t>(data[offset % size]);
}

static rapidcsv::LabelParams MakeLabelParams(const uint8_t* data, size_t size, size_t seed)
{
  rapidcsv::LabelParams params;
  params.mColumnNameIdx = static_cast<int>(PickIndex(data, size, seed) % 4) - 1;
  return params;
}

static rapidcsv::SeparatorParams MakeSeparatorParams(const uint8_t* data, size_t size, size_t seed)
{
  rapidcsv::SeparatorParams params;
  static const char seps[] = {',', ';', '\t', '|', ':'};
  params.mSeparator = seps[PickIndex(data, size, seed) % (sizeof(seps) / sizeof(seps[0]))];
  params.mTrim = (PickIndex(data, size, seed + 1) & 1) != 0;
  return params;
}

static rapidcsv::ConverterParams MakeConverterParams(const uint8_t* data, size_t size, size_t seed)
{
  rapidcsv::ConverterParams params;
  params.mHasDefaultConverter = (PickIndex(data, size, seed) & 1) != 0;
  params.mDefaultFloat = static_cast<long double>(static_cast<int>(PickIndex(data, size, seed + 1))) - 128.0L;
  params.mDefaultInteger = static_cast<long long>(static_cast<int>(PickIndex(data, size, seed + 2))) - 128LL;
  return params;
}

static rapidcsv::LineReaderParams MakeLineReaderParams(const uint8_t* data, size_t size, size_t seed)
{
  rapidcsv::LineReaderParams params;
  params.mSkipCommentLines = (PickIndex(data, size, seed) & 1) != 0;
  static const char prefixes[] = {'#', ';', '/', '%', '!'};
  params.mCommentPrefix = prefixes[PickIndex(data, size, seed + 1) % (sizeof(prefixes) / sizeof(prefixes[0]))];
  return params;
}

static void ToStringConv(const std::string& in, std::string& out)
{
  out = in;
}

static void ToIntConv(const std::string& in, int& out)
{
  out = 0;
  try
  {
    out = std::stoi(in);
  }
  catch (...)
  {
  }
}

static void ToLongLongConv(const std::string& in, long long& out)
{
  out = 0;
  try
  {
    out = std::stoll(in);
  }
  catch (...)
  {
  }
}

static void ToFloatConv(const std::string& in, float& out)
{
  out = 0.0f;
  try
  {
    out = std::stof(in);
  }
  catch (...)
  {
  }
}

static void ToDoubleConv(const std::string& in, double& out)
{
  out = 0.0;
  try
  {
    out = std::stod(in);
  }
  catch (...)
  {
  }
}

static void TouchNoConverterWhat()
{
  try
  {
    throw rapidcsv::no_converter();
  }
  catch (const rapidcsv::no_converter& e)
  {
    (void)e.what();
  }
  catch (...)
  {
  }
}

static void ExerciseDocument(const rapidcsv::Document& doc, const std::string& fuzzStr, size_t idx1, size_t idx2)
{
  try
  {
    std::vector<std::string> rowStr = doc.GetRow<std::string>(idx1, ToStringConv);
    (void)rowStr;
  }
  catch (...)
  {
  }

  try
  {
    std::vector<int> rowInt = doc.GetRow<int>(idx1, ToIntConv);
    (void)rowInt;
  }
  catch (const rapidcsv::no_converter& e)
  {
    (void)e.what();
  }
  catch (const std::exception& e)
  {
    (void)e.what();
  }
  catch (...)
  {
  }

  try
  {
    std::string cell1 = doc.GetCell<std::string>(idx1, idx2, ToStringConv);
    (void)cell1;
  }
  catch (...)
  {
  }

  try
  {
    int cell2 = doc.GetCell<int>(idx1, idx2, ToIntConv);
    (void)cell2;
  }
  catch (const rapidcsv::no_converter& e)
  {
    (void)e.what();
  }
  catch (const std::exception& e)
  {
    (void)e.what();
  }
  catch (...)
  {
  }

  try
  {
    std::string cell3 = doc.GetCell<std::string>(idx1, fuzzStr, ToStringConv);
    (void)cell3;
  }
  catch (...)
  {
  }

  try
  {
    double cell4 = doc.GetCell<double>(idx1, fuzzStr, ToDoubleConv);
    (void)cell4;
  }
  catch (const rapidcsv::no_converter& e)
  {
    (void)e.what();
  }
  catch (const std::exception& e)
  {
    (void)e.what();
  }
  catch (...)
  {
  }

  try
  {
    std::vector<std::string> col1 = doc.GetColumn<std::string>(idx1, ToStringConv);
    (void)col1;
  }
  catch (...)
  {
  }

  try
  {
    std::vector<long long> col2 = doc.GetColumn<long long>(idx1, ToLongLongConv);
    (void)col2;
  }
  catch (const rapidcsv::no_converter& e)
  {
    (void)e.what();
  }
  catch (const std::exception& e)
  {
    (void)e.what();
  }
  catch (...)
  {
  }

  try
  {
    std::vector<std::string> col3 = doc.GetColumn<std::string>(fuzzStr, ToStringConv);
    (void)col3;
  }
  catch (...)
  {
  }

  try
  {
    std::vector<float> col4 = doc.GetColumn<float>(fuzzStr, ToFloatConv);
    (void)col4;
  }
  catch (const rapidcsv::no_converter& e)
  {
    (void)e.what();
  }
  catch (const std::exception& e)
  {
    (void)e.what();
  }
  catch (...)
  {
  }
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* Data, size_t Size)
{
  std::ofstream ofs("./dummy_file", std::ios::binary);
  if (ofs)
  {
    ofs.write(reinterpret_cast<const char*>(Data), static_cast<std::streamsize>(Size));
  }
  ofs.close();

  TouchNoConverterWhat();

  const std::string fuzzStr = MakeString(Data, Size);
  const size_t idx1 = PickIndex(Data, Size, 0);
  const size_t idx2 = PickIndex(Data, Size, 1);

  for (size_t i = 0; i < 3; ++i)
  {
    try
    {
      rapidcsv::LabelParams labelParams = MakeLabelParams(Data, Size, i * 7 + 0);
      rapidcsv::SeparatorParams separatorParams = MakeSeparatorParams(Data, Size, i * 7 + 1);
      rapidcsv::ConverterParams converterParams = MakeConverterParams(Data, Size, i * 7 + 2);
      rapidcsv::LineReaderParams lineReaderParams = MakeLineReaderParams(Data, Size, i * 7 + 3);

      rapidcsv::Document doc("./dummy_file",
                             labelParams,
                             separatorParams,
                             converterParams,
                             lineReaderParams);

      ExerciseDocument(doc, fuzzStr, idx1, idx2);
    }
    catch (const rapidcsv::no_converter& e)
    {
      (void)e.what();
    }
    catch (const std::exception& e)
    {
      (void)e.what();
    }
    catch (...)
    {
    }
  }

  return 0;
}