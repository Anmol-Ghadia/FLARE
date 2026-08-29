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
#include <functional>
#include <exception>

#include "rapidcsv.h"

static std::string MakeString(const uint8_t* data, size_t size)
{
  return std::string(reinterpret_cast<const char*>(data), size);
}

static size_t PickIndex(const uint8_t* data, size_t size, size_t offset)
{
  if (size == 0) return 0;
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
    auto rowStr = doc.GetRow<std::string>(idx1, [](const std::string& s) { return s; });
    (void)rowStr;
  }
  catch (...)
  {
  }

  try
  {
    auto rowInt = doc.GetRow<int>(idx1, [](const std::string& s) { return std::stoi(s); });
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
    auto cell1 = doc.GetCell<std::string>(idx1, idx2, [](const std::string& s) { return s; });
    (void)cell1;
  }
  catch (...)
  {
  }

  try
  {
    auto cell2 = doc.GetCell<int>(idx1, idx2, [](const std::string& s) { return std::stoi(s); });
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
    auto cell3 = doc.GetCell<std::string>(idx1, fuzzStr, [](const std::string& s) { return s; });
    (void)cell3;
  }
  catch (...)
  {
  }

  try
  {
    auto cell4 = doc.GetCell<double>(idx1, fuzzStr, [](const std::string& s) { return std::stod(s); });
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
    auto col1 = doc.GetColumn<std::string>(idx1, [](const std::string& s) { return s; });
    (void)col1;
  }
  catch (...)
  {
  }

  try
  {
    auto col2 = doc.GetColumn<long long>(idx1, [](const std::string& s) { return std::stoll(s); });
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
    auto col3 = doc.GetColumn<std::string>(fuzzStr, [](const std::string& s) { return s; });
    (void)col3;
  }
  catch (...)
  {
  }

  try
  {
    auto col4 = doc.GetColumn<float>(fuzzStr, [](const std::string& s) { return std::stof(s); });
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