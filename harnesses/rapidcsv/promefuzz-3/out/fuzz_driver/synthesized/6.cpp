// This fuzz driver is generated for library rapidcsv, aiming to fuzz the following functions:
// rapidcsv::Document::RemoveColumn at rapidcsv.h:827:10 in rapidcsv.h
// rapidcsv::Document::GetColumnName at rapidcsv.h:1425:17 in rapidcsv.h
// rapidcsv::Document::RemoveColumn at rapidcsv.h:798:10 in rapidcsv.h
// rapidcsv::Document::GetColumnName at rapidcsv.h:1425:17 in rapidcsv.h
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

static size_t ReadSizeT(const uint8_t* data, size_t size, size_t& offset)
{
  size_t value = 0;
  const size_t n = std::min(sizeof(size_t), size - offset);
  for (size_t i = 0; i < n; ++i)
  {
    value = (value << 8) | static_cast<size_t>(data[offset + i]);
  }
  offset += n;
  return value;
}

static int ReadInt(const uint8_t* data, size_t size, size_t& offset)
{
  int value = 0;
  const size_t n = std::min(sizeof(int), size - offset);
  for (size_t i = 0; i < n; ++i)
  {
    value = (value << 8) | static_cast<int>(data[offset + i]);
  }
  offset += n;
  return value;
}

extern "C" int LLVMFuzzerTestOneInput_6(const uint8_t *Data, size_t Size)
{
  const std::string path = "./dummy_file";

  {
    std::ofstream ofs(path, std::ios::binary);
    if (!ofs)
      return 0;
    ofs.write(reinterpret_cast<const char*>(Data), static_cast<std::streamsize>(Size));
  }

  size_t offset = 0;

  rapidcsv::LabelParams labelParams;
  labelParams.mColumnNameIdx = (Size > 0) ? ((ReadInt(Data, Size, offset) % 5) - 2) : 0;

  rapidcsv::SeparatorParams separatorParams;
  separatorParams.mSeparator = (Size > offset) ? static_cast<char>(Data[offset++]) : ',';
  if (separatorParams.mSeparator == '\0' || separatorParams.mSeparator == '\n' || separatorParams.mSeparator == '\r')
    separatorParams.mSeparator = ',';
  separatorParams.mTrim = (Size > offset) ? ((Data[offset++] & 1) != 0) : false;

  rapidcsv::ConverterParams converterParams;
  converterParams.mHasDefaultConverter = (Size > offset) ? ((Data[offset++] & 1) != 0) : false;
  converterParams.mDefaultFloat = (Size > offset) ? static_cast<long double>(Data[offset++]) : 0.0L;
  converterParams.mDefaultInteger = (Size > offset) ? static_cast<long long>(Data[offset++]) : 0LL;

  rapidcsv::LineReaderParams lineReaderParams;
  lineReaderParams.mSkipCommentLines = (Size > offset) ? ((Data[offset++] & 1) != 0) : false;
  lineReaderParams.mCommentPrefix = (Size > offset) ? static_cast<char>(Data[offset++]) : '#';
  if (lineReaderParams.mCommentPrefix == '\0')
    lineReaderParams.mCommentPrefix = '#';

  try
  {
    rapidcsv::Document doc(path, labelParams, separatorParams, converterParams, lineReaderParams);

    const size_t idx1 = (Size > offset) ? ReadSizeT(Data, Size, offset) : 0;
    const size_t idx2 = (Size > offset) ? ReadSizeT(Data, Size, offset) : 0;
    const size_t idx3 = (Size > offset) ? ReadSizeT(Data, Size, offset) : 0;

    std::string colName1;
    std::string colName2;

    try
    {
      doc.RemoveColumn(idx1);
    }
    catch (...)
    {
    }

    try
    {
      colName1 = doc.GetColumnName(idx2);
    }
    catch (...)
    {
      colName1.clear();
    }

    try
    {
      doc.RemoveColumn(colName1);
    }
    catch (...)
    {
    }

    try
    {
      colName2 = doc.GetColumnName(idx3);
      (void)colName2;
    }
    catch (...)
    {
    }
  }
  catch (...)
  {
  }

  return 0;
}