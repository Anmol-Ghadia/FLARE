// This fuzz driver is generated for library rapidcsv, aiming to fuzz the following functions:
// rapidcsv::Document::GetColumnNames at rapidcsv.h:1479:30 in rapidcsv.h
// rapidcsv::Document::GetRow at rapidcsv.h:944:20 in rapidcsv.h
// rapidcsv::Document::GetRow at rapidcsv.h:944:20 in rapidcsv.h
// rapidcsv::Document::RemoveRow at rapidcsv.h:1076:10 in rapidcsv.h
// rapidcsv::Document::SetColumnName at rapidcsv.h:1441:10 in rapidcsv.h
// rapidcsv::Document::GetColumnCount at rapidcsv.h:910:12 in rapidcsv.h
// rapidcsv::Document::GetRowNames at rapidcsv.h:1551:30 in rapidcsv.h
// rapidcsv::Document::GetColumnNames at rapidcsv.h:1479:30 in rapidcsv.h
// rapidcsv::Document::GetColumnCount at rapidcsv.h:910:12 in rapidcsv.h
// rapidcsv::Document::GetRowNames at rapidcsv.h:1551:30 in rapidcsv.h
// rapidcsv::Document::GetColumnNames at rapidcsv.h:1479:30 in rapidcsv.h
// rapidcsv::Document::GetRow at rapidcsv.h:944:20 in rapidcsv.h
// rapidcsv::Document::RemoveRow at rapidcsv.h:1076:10 in rapidcsv.h
// rapidcsv::Document::SetColumnName at rapidcsv.h:1441:10 in rapidcsv.h
// rapidcsv::Document::GetColumnCount at rapidcsv.h:910:12 in rapidcsv.h
// rapidcsv::Document::GetRowNames at rapidcsv.h:1551:30 in rapidcsv.h
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

static int ToSmallInt(uint8_t b, int minv, int maxv)
{
  if (maxv <= minv) return minv;
  return minv + (b % (maxv - minv + 1));
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* Data, size_t Size)
{
  std::ofstream ofs("./dummy_file", std::ios::binary);
  if (ofs)
  {
    ofs.write(reinterpret_cast<const char*>(Data), static_cast<std::streamsize>(Size));
    ofs.close();
  }

  rapidcsv::LabelParams labelParams;
  rapidcsv::SeparatorParams sepParams;
  rapidcsv::ConverterParams convParams;
  rapidcsv::LineReaderParams lineParams;

  if (Size > 0)
  {
    labelParams.mColumnNameIdx = ToSmallInt(Data[0], -1, 4);
  }
  if (Size > 1)
  {
    sepParams.mSeparator = static_cast<char>(Data[1]);
    if (sepParams.mSeparator == '\0' || sepParams.mSeparator == '\n' || sepParams.mSeparator == '\r')
      sepParams.mSeparator = ',';
  }
  if (Size > 2)
  {
    sepParams.mTrim = (Data[2] & 1) != 0;
  }
  if (Size > 3)
  {
    convParams.mHasDefaultConverter = (Data[3] & 1) != 0;
  }
  if (Size > 4)
  {
    convParams.mDefaultInteger = static_cast<long long>(static_cast<int8_t>(Data[4]));
  }
  if (Size > 5)
  {
    convParams.mDefaultFloat = static_cast<long double>(static_cast<int8_t>(Data[5])) / 3.0L;
  }
  if (Size > 6)
  {
    lineParams.mSkipCommentLines = (Data[6] & 1) != 0;
  }
  if (Size > 7)
  {
    lineParams.mCommentPrefix = static_cast<char>(Data[7]);
    if (lineParams.mCommentPrefix == '\0' || lineParams.mCommentPrefix == '\n' || lineParams.mCommentPrefix == '\r')
      lineParams.mCommentPrefix = '#';
  }

  try
  {
    rapidcsv::Document doc("./dummy_file", labelParams, sepParams, convParams, lineParams);

    try
    {
      (void)doc.GetColumnCount();
    }
    catch (...)
    {
    }

    try
    {
      std::vector<std::string> rowNames = doc.GetRowNames();
      if (!rowNames.empty())
      {
        volatile size_t sink = rowNames.size();
        (void)sink;
      }
    }
    catch (...)
    {
    }

    try
    {
      std::vector<std::string> colNames = doc.GetColumnNames();
      if (!colNames.empty())
      {
        volatile size_t sink = colNames.size();
        (void)sink;
      }
    }
    catch (...)
    {
    }

    size_t idx1 = 0;
    size_t idx2 = 0;
    if (Size > 8) idx1 = static_cast<size_t>(Data[8]);
    if (Size > 9) idx2 = static_cast<size_t>(Data[9]);

    try
    {
      auto row = doc.GetRow<std::string>(idx1);
      volatile size_t sink = row.size();
      (void)sink;
    }
    catch (...)
    {
    }

    try
    {
      auto row = doc.GetRow<int>(idx2);
      volatile size_t sink = row.size();
      (void)sink;
    }
    catch (...)
    {
    }

    try
    {
      doc.RemoveRow(idx1);
    }
    catch (...)
    {
    }

    try
    {
      std::string newName;
      if (Size > 10)
      {
        size_t nameLen = std::min<size_t>(Size - 10, 32);
        newName.assign(reinterpret_cast<const char*>(Data + 10), nameLen);
      }
      doc.SetColumnName(idx2, newName);
    }
    catch (...)
    {
    }

    try
    {
      (void)doc.GetColumnCount();
      (void)doc.GetRowNames();
      (void)doc.GetColumnNames();
    }
    catch (...)
    {
    }

    try
    {
      if (Size > 0)
      {
        rapidcsv::LabelParams altLabel = labelParams;
        altLabel.mColumnNameIdx = ToSmallInt(Data[0], -1, 8);
        rapidcsv::Document doc2("./dummy_file", altLabel, sepParams, convParams, lineParams);

        try { (void)doc2.GetColumnCount(); } catch (...) {}
        try { (void)doc2.GetRowNames(); } catch (...) {}
        try { (void)doc2.GetColumnNames(); } catch (...) {}
        try { (void)doc2.GetRow<std::string>(0); } catch (...) {}
        try { doc2.RemoveRow(0); } catch (...) {}
        try { doc2.SetColumnName(0, "fuzz"); } catch (...) {}
      }
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