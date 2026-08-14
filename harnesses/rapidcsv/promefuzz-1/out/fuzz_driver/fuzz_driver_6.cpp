// This fuzz driver is generated for library rapidcsv, aiming to fuzz the following functions:
// rapidcsv::Document::RemoveColumn at rapidcsv.h:798:10 in rapidcsv.h
// rapidcsv::Document::GetColumnName at rapidcsv.h:1425:17 in rapidcsv.h
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
#include <sstream>

#include "rapidcsv.h"

static size_t GetIndex(const uint8_t* Data, size_t Size, size_t& Offset)
{
  if (Offset >= Size) return 0;
  size_t value = Data[Offset++];
  if (Offset < Size)
  {
    value = (value << 8) ^ Data[Offset++];
  }
  return value;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
  std::ofstream ofs("./dummy_file", std::ios::binary);
  if (ofs)
  {
    ofs.write(reinterpret_cast<const char*>(Data), static_cast<std::streamsize>(Size));
    ofs.close();
  }

  rapidcsv::LabelParams labelParams;
  rapidcsv::SeparatorParams separatorParams;
  rapidcsv::ConverterParams converterParams;
  rapidcsv::LineReaderParams lineReaderParams;

  size_t offset = 0;

  if (Size > 0)
  {
    labelParams.mColumnNameIdx = static_cast<int>(Data[offset++] % 4) - 1; // [-1, 2]
  }
  else
  {
    labelParams.mColumnNameIdx = 0;
  }

  if (offset < Size)
  {
    separatorParams.mSeparator = static_cast<char>(Data[offset++]);
    if (separatorParams.mSeparator == '\0')
    {
      separatorParams.mSeparator = ',';
    }
  }
  else
  {
    separatorParams.mSeparator = ',';
  }

  if (offset < Size)
  {
    separatorParams.mTrim = (Data[offset++] & 1) != 0;
  }
  else
  {
    separatorParams.mTrim = false;
  }

  if (offset < Size)
  {
    converterParams.mHasDefaultConverter = (Data[offset++] & 1) != 0;
  }
  else
  {
    converterParams.mHasDefaultConverter = false;
  }

  converterParams.mDefaultFloat = (offset < Size) ? static_cast<long double>(static_cast<int8_t>(Data[offset++])) : 0.0L;
  converterParams.mDefaultInteger = (offset < Size) ? static_cast<long long>(static_cast<int8_t>(Data[offset++])) : 0LL;

  if (offset < Size)
  {
    lineReaderParams.mSkipCommentLines = (Data[offset++] & 1) != 0;
  }
  else
  {
    lineReaderParams.mSkipCommentLines = false;
  }

  if (offset < Size)
  {
    lineReaderParams.mCommentPrefix = static_cast<char>(Data[offset++]);
  }
  else
  {
    lineReaderParams.mCommentPrefix = '#';
  }

  std::string csv(reinterpret_cast<const char*>(Data), Size);
  std::istringstream iss(csv, std::ios::binary);

  try
  {
    rapidcsv::Document doc(iss, labelParams, separatorParams, converterParams, lineReaderParams);

    size_t idx1 = GetIndex(Data, Size, offset);
    size_t idx2 = GetIndex(Data, Size, offset);
    size_t idx3 = GetIndex(Data, Size, offset);
    size_t idx4 = GetIndex(Data, Size, offset);

    std::string colName;
    if (offset < Size)
    {
      size_t nameLen = static_cast<size_t>(Data[offset++]) % (Size + 1);
      if (nameLen > Size - offset)
      {
        nameLen = Size - offset;
      }
      colName.assign(reinterpret_cast<const char*>(Data + offset), nameLen);
      offset += nameLen;
    }
    else
    {
      colName = "A";
    }

    try
    {
      doc.RemoveColumn(idx1);
    }
    catch (...)
    {
    }

    try
    {
      (void)doc.GetColumnName(idx2);
    }
    catch (...)
    {
    }

    try
    {
      doc.RemoveColumn(colName);
    }
    catch (...)
    {
    }

    try
    {
      (void)doc.GetColumnName(idx4);
    }
    catch (...)
    {
    }

    try
    {
      doc.RemoveColumn(idx3);
    }
    catch (...)
    {
    }

    try
    {
      (void)doc.GetColumnName(idx1);
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