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
#include <algorithm>

#include "rapidcsv.h"

static int ClampInt(int v, int lo, int hi)
{
  return (v < lo) ? lo : ((v > hi) ? hi : v);
}

static rapidcsv::LabelParams MakeLabelParams(const uint8_t* Data, size_t Size, size_t offset)
{
  rapidcsv::LabelParams p;
  int colNameIdx = -1;
  if (Size > offset)
  {
    colNameIdx = static_cast<int>(Data[offset] % 4) - 1; // -1..2
  }
  p.mColumnNameIdx = colNameIdx;
  return p;
}

static rapidcsv::SeparatorParams MakeSeparatorParams(const uint8_t* Data, size_t Size, size_t offset)
{
  rapidcsv::SeparatorParams p;
  static const char seps[] = {',', ';', '\t', '|', ':'};
  p.mSeparator = seps[(Size > offset) ? (Data[offset] % (sizeof(seps) / sizeof(seps[0]))) : 0];
  p.mTrim = (Size > offset + 1) ? ((Data[offset + 1] & 1) != 0) : false;
  return p;
}

static rapidcsv::ConverterParams MakeConverterParams(const uint8_t* Data, size_t Size, size_t offset)
{
  rapidcsv::ConverterParams p;
  p.mHasDefaultConverter = (Size > offset) ? ((Data[offset] & 1) != 0) : false;
  p.mDefaultFloat = (Size > offset + 1) ? static_cast<long double>(static_cast<int8_t>(Data[offset + 1])) : 0.0L;
  p.mDefaultInteger = (Size > offset + 2) ? static_cast<long long>(static_cast<int8_t>(Data[offset + 2])) : 0;
  return p;
}

static rapidcsv::LineReaderParams MakeLineReaderParams(const uint8_t* Data, size_t Size, size_t offset)
{
  rapidcsv::LineReaderParams p;
  p.mSkipCommentLines = (Size > offset) ? ((Data[offset] & 1) != 0) : false;
  p.mCommentPrefix = (Size > offset + 1) ? static_cast<char>(Data[offset + 1]) : '#';
  if (p.mCommentPrefix == '\0')
  {
    p.mCommentPrefix = '#';
  }
  return p;
}

static std::string MakeRowQueryName(const std::vector<std::string>& rowNames, const uint8_t* Data, size_t Size, size_t offset)
{
  if (!rowNames.empty() && Size > offset)
  {
    return rowNames[Data[offset] % rowNames.size()];
  }

  if (Size > offset + 1)
  {
    return std::string(reinterpret_cast<const char*>(Data + offset),
                       reinterpret_cast<const char*>(Data + std::min(Size, offset + 8)));
  }

  return std::string("nonexistent");
}

static int ToIntConv(const std::string& s)
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

static double ToDoubleConv(const std::string& s)
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

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
  std::string csv(reinterpret_cast<const char*>(Data), Size);

  {
    std::ofstream ofs("./dummy_file", std::ios::binary);
    ofs.write(csv.data(), static_cast<std::streamsize>(csv.size()));
  }

  rapidcsv::LabelParams labelParams1 = MakeLabelParams(Data, Size, 0);
  rapidcsv::SeparatorParams sepParams1 = MakeSeparatorParams(Data, Size, 1);
  rapidcsv::ConverterParams convParams1 = MakeConverterParams(Data, Size, 3);
  rapidcsv::LineReaderParams lineParams1 = MakeLineReaderParams(Data, Size, 6);

  rapidcsv::LabelParams labelParams2 = MakeLabelParams(Data, Size, 8);
  rapidcsv::SeparatorParams sepParams2 = MakeSeparatorParams(Data, Size, 9);
  rapidcsv::ConverterParams convParams2 = MakeConverterParams(Data, Size, 11);
  rapidcsv::LineReaderParams lineParams2 = MakeLineReaderParams(Data, Size, 14);

  rapidcsv::Document doc;

  try
  {
    std::stringstream ss(csv);
    doc.Load(ss, labelParams1, sepParams1, convParams1, lineParams1);
  }
  catch (...)
  {
  }

  try
  {
    doc.Load(std::string("./dummy_file"), labelParams2, sepParams2, convParams2, lineParams2);
  }
  catch (...)
  {
  }

  try
  {
    std::vector<std::string> rowNames = doc.GetRowNames();

    for (size_t i = 0; i < rowNames.size() && i < 4; ++i)
    {
      volatile size_t len = rowNames[i].size();
      (void)len;
    }

    try
    {
      std::string rowName0 = doc.GetRowName(0);
      volatile size_t len0 = rowName0.size();
      (void)len0;
    }
    catch (...)
    {
    }

    if (Size > 16)
    {
      size_t idx = Data[16];
      try
      {
        std::string rowNameX = doc.GetRowName(idx);
        volatile size_t lenX = rowNameX.size();
        (void)lenX;
      }
      catch (...)
      {
      }
    }

    std::string queryName = MakeRowQueryName(rowNames, Data, Size, 17);

    try
    {
      std::vector<std::string> rowStr = doc.GetRow<std::string>(
          queryName,
          [](const std::string& s) { return s; });
      volatile size_t n = rowStr.size();
      (void)n;
    }
    catch (...)
    {
    }

    try
    {
      std::vector<int> rowInt = doc.GetRow<int>(
          queryName,
          ToIntConv);
      volatile size_t n = rowInt.size();
      (void)n;
    }
    catch (...)
    {
    }

    try
    {
      std::vector<double> rowDouble = doc.GetRow<double>(
          queryName,
          ToDoubleConv);
      volatile size_t n = rowDouble.size();
      (void)n;
    }
    catch (...)
    {
    }
  }
  catch (...)
  {
  }

  try
  {
    rapidcsv::no_converter ex;
    volatile const char* msg = ex.what();
    (void)msg;
  }
  catch (...)
  {
  }

  return 0;
}