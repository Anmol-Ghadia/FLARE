// This fuzz driver is generated for library rapidcsv, aiming to fuzz the following functions:
// rapidcsv::Document::GetRow at rapidcsv.h:1008:20 in rapidcsv.h
// rapidcsv::Document::GetRow at rapidcsv.h:1008:20 in rapidcsv.h
// rapidcsv::Document::GetRow at rapidcsv.h:968:20 in rapidcsv.h
// rapidcsv::Document::GetRow at rapidcsv.h:968:20 in rapidcsv.h
// rapidcsv::Document::GetCell at rapidcsv.h:1233:7 in rapidcsv.h
// rapidcsv::Document::GetCell at rapidcsv.h:1233:7 in rapidcsv.h
// rapidcsv::Document::GetCell at rapidcsv.h:1276:7 in rapidcsv.h
// rapidcsv::Document::GetCell at rapidcsv.h:1276:7 in rapidcsv.h
// rapidcsv::Document::GetColumn at rapidcsv.h:730:20 in rapidcsv.h
// rapidcsv::Document::GetColumn at rapidcsv.h:730:20 in rapidcsv.h
// rapidcsv::Document::GetColumn at rapidcsv.h:691:20 in rapidcsv.h
// rapidcsv::Document::GetColumn at rapidcsv.h:691:20 in rapidcsv.h
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <functional>
#include <sstream>
#include <string>
#include <vector>

#include "rapidcsv.h"

static std::string MakeString(const uint8_t* data, size_t size)
{
  return std::string(reinterpret_cast<const char*>(data), size);
}

static rapidcsv::LabelParams MakeLabelParams(const uint8_t* data, size_t size, size_t offset)
{
  rapidcsv::LabelParams params;
  params.mColumnNameIdx = (size > offset) ? static_cast<int>(static_cast<int8_t>(data[offset])) : 0;
  return params;
}

static rapidcsv::SeparatorParams MakeSeparatorParams(const uint8_t* data, size_t size, size_t offset)
{
  rapidcsv::SeparatorParams params;
  params.mSeparator = (size > offset && data[offset] != 0) ? static_cast<char>(data[offset]) : ',';
  if (params.mSeparator == '\n' || params.mSeparator == '\r')
  {
    params.mSeparator = ',';
  }
  params.mTrim = (size > offset + 1) ? ((data[offset + 1] & 1) != 0) : false;
  return params;
}

static rapidcsv::ConverterParams MakeConverterParams(const uint8_t* data, size_t size, size_t offset)
{
  rapidcsv::ConverterParams params;
  params.mHasDefaultConverter = (size > offset) ? ((data[offset] & 1) != 0) : false;
  params.mDefaultFloat = (size > offset + 1) ? static_cast<long double>(static_cast<int8_t>(data[offset + 1])) : 0.0L;
  params.mDefaultInteger = (size > offset + 2) ? static_cast<long long>(static_cast<int8_t>(data[offset + 2])) : 0LL;
  return params;
}

static rapidcsv::LineReaderParams MakeLineReaderParams(const uint8_t* data, size_t size, size_t offset)
{
  rapidcsv::LineReaderParams params;
  params.mSkipCommentLines = (size > offset) ? ((data[offset] & 1) != 0) : false;
  params.mCommentPrefix = (size > offset + 1 && data[offset + 1] != 0) ? static_cast<char>(data[offset + 1]) : '#';
  return params;
}

static size_t PickIndex(const uint8_t* data, size_t size, size_t offset)
{
  return (size > offset) ? static_cast<size_t>(data[offset]) : 0U;
}

static std::string PickToken(const std::string& s, size_t which)
{
  if (s.empty())
  {
    return std::string();
  }

  size_t start = 0;
  size_t current = 0;
  while (start <= s.size())
  {
    size_t end = s.find_first_of(",\r\n", start);
    if (current == which)
    {
      if (end == std::string::npos)
      {
        return s.substr(start);
      }
      return s.substr(start, end - start);
    }

    if (end == std::string::npos)
    {
      break;
    }

    start = end + 1;
    ++current;
  }

  return std::string();
}

static void StrConv(const std::string& in, std::string& out)
{
  out = in;
}

static void IntConv(const std::string& in, int& out)
{
  try
  {
    out = std::stoi(in);
  }
  catch (...)
  {
    out = 0;
  }
}

static void DoubleConv(const std::string& in, double& out)
{
  try
  {
    out = std::stod(in);
  }
  catch (...)
  {
    out = 0.0;
  }
}

static void ExerciseDocument(const rapidcsv::Document& doc, const std::string& content, const uint8_t* data, size_t size)
{
  const size_t idx0 = PickIndex(data, size, 8);
  const size_t idx1 = PickIndex(data, size, 9);
  const size_t idx2 = PickIndex(data, size, 10);
  const size_t idx3 = PickIndex(data, size, 11);

  const std::string name0 = PickToken(content, idx0 % 8);
  const std::string name1 = PickToken(content, idx1 % 8);

  rapidcsv::ConvFunc<std::string> strConv = StrConv;
  rapidcsv::ConvFunc<int> intConv = IntConv;
  rapidcsv::ConvFunc<double> dblConv = DoubleConv;

  try { (void)doc.GetRow<std::string>(name0, strConv); } catch (...) {}
  try { (void)doc.GetRow<int>(name1, intConv); } catch (...) {}
  try { (void)doc.GetRow<std::string>(idx0, strConv); } catch (...) {}
  try { (void)doc.GetRow<double>(idx1, dblConv); } catch (...) {}

  try { (void)doc.GetCell<std::string>(name0, name1, strConv); } catch (...) {}
  try { (void)doc.GetCell<int>(name1, name0, intConv); } catch (...) {}
  try { (void)doc.GetCell<std::string>(name0, idx2, strConv); } catch (...) {}
  try { (void)doc.GetCell<double>(name1, idx3, dblConv); } catch (...) {}

  try { (void)doc.GetColumn<std::string>(name0, strConv); } catch (...) {}
  try { (void)doc.GetColumn<int>(name1, intConv); } catch (...) {}
  try { (void)doc.GetColumn<std::string>(idx2, strConv); } catch (...) {}
  try { (void)doc.GetColumn<double>(idx3, dblConv); } catch (...) {}
}

extern "C" int LLVMFuzzerTestOneInput_15(const uint8_t* Data, size_t Size)
{
  const std::string content = MakeString(Data, Size);

  {
    std::ofstream ofs("./dummy_file", std::ios::binary);
    if (ofs)
    {
      ofs.write(content.data(), static_cast<std::streamsize>(content.size()));
    }
  }

  rapidcsv::LabelParams labelParams = MakeLabelParams(Data, Size, 0);
  rapidcsv::SeparatorParams separatorParams = MakeSeparatorParams(Data, Size, 1);
  rapidcsv::ConverterParams converterParams = MakeConverterParams(Data, Size, 3);
  rapidcsv::LineReaderParams lineReaderParams = MakeLineReaderParams(Data, Size, 6);

  try
  {
    std::stringstream ss(content);
    rapidcsv::Document doc(ss, labelParams, separatorParams, converterParams, lineReaderParams);
    ExerciseDocument(doc, content, Data, Size);
  }
  catch (...)
  {
  }

  try
  {
    rapidcsv::LabelParams altLabelParams = labelParams;
    if (altLabelParams.mColumnNameIdx < -1)
    {
      altLabelParams.mColumnNameIdx = -1;
    }

    rapidcsv::SeparatorParams altSepParams = separatorParams;
    altSepParams.mTrim = !altSepParams.mTrim;
    if (altSepParams.mSeparator == 0 || altSepParams.mSeparator == '\n' || altSepParams.mSeparator == '\r')
    {
      altSepParams.mSeparator = ';';
    }

    rapidcsv::ConverterParams altConvParams = converterParams;
    altConvParams.mHasDefaultConverter = true;

    rapidcsv::LineReaderParams altLineParams = lineReaderParams;
    altLineParams.mSkipCommentLines = !altLineParams.mSkipCommentLines;

    std::stringstream ss2(content);
    rapidcsv::Document doc2(ss2, altLabelParams, altSepParams, altConvParams, altLineParams);
    ExerciseDocument(doc2, content, Data, Size);
  }
  catch (...)
  {
  }

  return 0;
}