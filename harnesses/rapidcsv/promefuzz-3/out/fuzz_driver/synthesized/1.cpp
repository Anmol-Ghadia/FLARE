// This fuzz driver is generated for library rapidcsv, aiming to fuzz the following functions:
// rapidcsv::Document::GetRow at rapidcsv.h:991:20 in rapidcsv.h
// rapidcsv::Document::InsertRow at rapidcsv.h:1115:10 in rapidcsv.h
// rapidcsv::Document::GetRow at rapidcsv.h:991:20 in rapidcsv.h
// rapidcsv::Document::GetCell at rapidcsv.h:1208:7 in rapidcsv.h
// rapidcsv::Document::GetCell at rapidcsv.h:1257:7 in rapidcsv.h
// rapidcsv::Document::GetCell at rapidcsv.h:1294:7 in rapidcsv.h
// rapidcsv::Document::GetCell at rapidcsv.h:1172:7 in rapidcsv.h
// rapidcsv::Document::SetCell at rapidcsv.h:1331:10 in rapidcsv.h
// rapidcsv::Document::GetCell at rapidcsv.h:1172:7 in rapidcsv.h
// rapidcsv::Document::SetCell at rapidcsv.h:1367:10 in rapidcsv.h
// rapidcsv::Document::GetCell at rapidcsv.h:1208:7 in rapidcsv.h
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

static size_t ReadSizeT(const uint8_t* data, size_t size, size_t& offset)
{
  if (offset >= size) return 0;
  size_t value = 0;
  const size_t n = std::min(sizeof(size_t), size - offset);
  for (size_t i = 0; i < n; ++i)
  {
    value = (value << 8) | static_cast<size_t>(data[offset++]);
  }
  return value;
}

static std::string ReadString(const uint8_t* data, size_t size, size_t& offset)
{
  if (offset >= size) return std::string();
  size_t len = ReadSizeT(data, size, offset);
  if (offset >= size) return std::string();
  len %= (size - offset + 1);
  const size_t actual = std::min(len, size - offset);
  std::string s(reinterpret_cast<const char*>(data + offset), actual);
  offset += actual;
  return s;
}

extern "C" int LLVMFuzzerTestOneInput_1(const uint8_t *Data, size_t Size)
{
  std::ofstream ofs("./dummy_file", std::ios::binary);
  if (ofs)
  {
    ofs.write(reinterpret_cast<const char*>(Data), static_cast<std::streamsize>(Size));
  }
  ofs.close();

  size_t offset = 0;

  rapidcsv::LabelParams labelParams;
  labelParams.mColumnNameIdx = (Size > 0) ? static_cast<int>(Data[offset++]) % 3 - 1 : 0;

  rapidcsv::SeparatorParams separatorParams;
  separatorParams.mSeparator = (Size > offset) ? static_cast<char>(Data[offset++]) : ',';
  if (separatorParams.mSeparator == '\0' || separatorParams.mSeparator == '\n' || separatorParams.mSeparator == '\r')
  {
    separatorParams.mSeparator = ',';
  }
  separatorParams.mTrim = (Size > offset) ? ((Data[offset++] & 1) != 0) : false;

  rapidcsv::ConverterParams converterParams;
  converterParams.mHasDefaultConverter = (Size > offset) ? ((Data[offset++] & 1) != 0) : false;
  converterParams.mDefaultFloat = (Size > offset) ? static_cast<long double>(static_cast<int8_t>(Data[offset++])) : 0.0L;
  converterParams.mDefaultInteger = (Size > offset) ? static_cast<long long>(static_cast<int8_t>(Data[offset++])) : 0LL;

  rapidcsv::LineReaderParams lineReaderParams;
  lineReaderParams.mSkipCommentLines = (Size > offset) ? ((Data[offset++] & 1) != 0) : false;
  lineReaderParams.mCommentPrefix = (Size > offset) ? static_cast<char>(Data[offset++]) : '#';

  std::string csv(reinterpret_cast<const char*>(Data), Size);
  std::stringstream ss(csv);

  try
  {
    rapidcsv::Document doc(ss, labelParams, separatorParams, converterParams, lineReaderParams);

    std::string rowName1 = ReadString(Data, Size, offset);
    std::string rowName2 = ReadString(Data, Size, offset);
    std::string colName1 = ReadString(Data, Size, offset);
    std::string colName2 = ReadString(Data, Size, offset);
    std::string setValue1 = ReadString(Data, Size, offset);
    std::string setValue2 = ReadString(Data, Size, offset);

    size_t rowIdx1 = ReadSizeT(Data, Size, offset) % 16;
    size_t rowIdx2 = ReadSizeT(Data, Size, offset) % 16;
    size_t colIdx1 = ReadSizeT(Data, Size, offset) % 16;
    size_t colIdx2 = ReadSizeT(Data, Size, offset) % 16;

    std::vector<std::string> insertRow;
    size_t insertCount = ReadSizeT(Data, Size, offset) % 8;
    for (size_t i = 0; i < insertCount; ++i)
    {
      insertRow.push_back(ReadString(Data, Size, offset));
    }
    std::string insertRowName = ReadString(Data, Size, offset);

    try
    {
      (void)doc.GetRow<std::string>(rowName1);
    }
    catch (...)
    {
    }

    try
    {
      doc.InsertRow<std::string>(rowIdx1, insertRow, insertRowName);
    }
    catch (...)
    {
    }

    try
    {
      (void)doc.GetRow<std::string>(rowName2);
    }
    catch (...)
    {
    }

    try
    {
      (void)doc.GetCell<std::string>(colName1, rowName1);
    }
    catch (...)
    {
    }

    try
    {
      (void)doc.GetCell<std::string>(colName2, rowIdx2);
    }
    catch (...)
    {
    }

    try
    {
      (void)doc.GetCell<std::string>(colIdx1, rowName2);
    }
    catch (...)
    {
    }

    try
    {
      (void)doc.GetCell<std::string>(colIdx2, rowIdx1);
    }
    catch (...)
    {
    }

    try
    {
      doc.SetCell<std::string>(colIdx1, rowIdx2, setValue1);
    }
    catch (...)
    {
    }

    try
    {
      (void)doc.GetCell<std::string>(colIdx1, rowIdx2);
    }
    catch (...)
    {
    }

    try
    {
      doc.SetCell<std::string>(colName1, rowName1, setValue2);
    }
    catch (...)
    {
    }

    try
    {
      (void)doc.GetCell<std::string>(colName1, rowName1);
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