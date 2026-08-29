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

#include "rapidcsv.h"

static std::string MakeString(const uint8_t* data, size_t size)
{
  return std::string(reinterpret_cast<const char*>(data), size);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
  std::ofstream ofs("./dummy_file", std::ios::binary);
  if (ofs)
  {
    ofs.write(reinterpret_cast<const char*>(Data), static_cast<std::streamsize>(Size));
  }
  ofs.close();

  std::string input = MakeString(Data, Size);
  std::stringstream ss(input);

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
    rapidcsv::Document doc(ss, labelParams, separatorParams, converterParams, lineReaderParams);

    std::string rowName1 = (Size > 8) ? input.substr(0, Size / 3) : std::string("row");
    std::string rowName2 = (Size > 9) ? input.substr(Size / 3, Size / 3) : std::string("row2");
    std::string colName1 = (Size > 10) ? input.substr((Size / 2), (Size > 1 ? Size / 4 : 0)) : std::string("col");
    std::string colName2 = (Size > 11) ? input.substr((Size > 0 ? Size - 1 : 0) / 2) : std::string("col2");

    size_t rowIdx = (Size > 12) ? static_cast<size_t>(Data[12]) : 0;
    size_t rowIdx2 = (Size > 13) ? static_cast<size_t>(Data[13]) : 1;
    size_t colIdx = (Size > 14) ? static_cast<size_t>(Data[14]) : 0;
    size_t colIdx2 = (Size > 15) ? static_cast<size_t>(Data[15]) : 1;

    std::vector<std::string> insertedRow;
    if (Size > 16)
    {
      insertedRow.push_back(std::to_string(static_cast<int>(Data[16])));
      if (Size > 17) insertedRow.push_back(std::to_string(static_cast<int>(Data[17])));
      if (Size > 18) insertedRow.push_back(input.substr(0, Size / 5));
    }

    try
    {
      (void)doc.GetRow<std::string>(rowName1);
    }
    catch (...)
    {
    }

    try
    {
      doc.InsertRow<std::string>(rowIdx, insertedRow, rowName2);
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
      (void)doc.GetCell<std::string>(colIdx, rowIdx);
    }
    catch (...)
    {
    }

    try
    {
      (void)doc.GetCell<std::string>(colName1, rowIdx);
    }
    catch (...)
    {
    }

    try
    {
      (void)doc.GetCell<std::string>(colName1, rowName2);
    }
    catch (...)
    {
    }

    try
    {
      (void)doc.GetCell<std::string>(colIdx2, rowName2);
    }
    catch (...)
    {
    }

    try
    {
      doc.SetCell<std::string>(colName2, rowName2, input);
    }
    catch (...)
    {
    }

    try
    {
      (void)doc.GetCell<std::string>(colName2, rowName2);
    }
    catch (...)
    {
    }

    try
    {
      doc.SetCell<std::string>(colIdx2, rowIdx2, rowName1);
    }
    catch (...)
    {
    }

    try
    {
      (void)doc.GetCell<std::string>(colIdx2, rowIdx2);
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