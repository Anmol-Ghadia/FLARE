// This fuzz driver is generated for library rapidcsv, aiming to fuzz the following functions:
// rapidcsv::Document::SetRowName at rapidcsv.h:1515:10 in rapidcsv.h
// rapidcsv::Document::GetRowName at rapidcsv.h:1499:17 in rapidcsv.h
// rapidcsv::Document::GetRow at rapidcsv.h:991:20 in rapidcsv.h
// rapidcsv::Document::RemoveRow at rapidcsv.h:1097:10 in rapidcsv.h
// rapidcsv::Document::GetRowIdx at rapidcsv.h:926:9 in rapidcsv.h
// rapidcsv::Document::InsertRow at rapidcsv.h:1115:10 in rapidcsv.h
// rapidcsv::Document::InsertRow at rapidcsv.h:1115:10 in rapidcsv.h
// rapidcsv::Document::InsertRow at rapidcsv.h:1115:10 in rapidcsv.h
// rapidcsv::Document::GetRowIdx at rapidcsv.h:926:9 in rapidcsv.h
// rapidcsv::Document::GetRowName at rapidcsv.h:1499:17 in rapidcsv.h
// rapidcsv::Document::SetRowName at rapidcsv.h:1515:10 in rapidcsv.h
// rapidcsv::Document::GetRowName at rapidcsv.h:1499:17 in rapidcsv.h
// rapidcsv::Document::GetRow at rapidcsv.h:991:20 in rapidcsv.h
// rapidcsv::Document::GetRowIdx at rapidcsv.h:926:9 in rapidcsv.h
// rapidcsv::Document::RemoveRow at rapidcsv.h:1097:10 in rapidcsv.h
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

static std::string MakeString(const uint8_t* data, size_t size)
{
  return std::string(reinterpret_cast<const char*>(data), size);
}

static size_t ConsumeSizeT(const uint8_t*& data, size_t& size)
{
  size_t value = 0;
  const size_t n = std::min(size, sizeof(size_t));
  for (size_t i = 0; i < n; ++i)
  {
    value = (value << 8) ^ data[i];
  }
  data += n;
  size -= n;
  return value;
}

static int ConsumeInt(const uint8_t*& data, size_t& size)
{
  int value = 0;
  const size_t n = std::min(size, sizeof(int));
  for (size_t i = 0; i < n; ++i)
  {
    value = (value << 8) ^ data[i];
  }
  data += n;
  size -= n;
  return value;
}

static bool ConsumeBool(const uint8_t*& data, size_t& size)
{
  if (size == 0) return false;
  bool v = ((*data) & 1) != 0;
  ++data;
  --size;
  return v;
}

static char ConsumeChar(const uint8_t*& data, size_t& size, char fallback)
{
  if (size == 0) return fallback;
  char c = static_cast<char>(*data);
  ++data;
  --size;
  return c;
}

static std::string ConsumeString(const uint8_t*& data, size_t& size)
{
  if (size == 0) return std::string();
  size_t len = ConsumeSizeT(data, size);
  if (size == 0) return std::string();
  len %= (size + 1);
  std::string out(reinterpret_cast<const char*>(data), len);
  data += len;
  size -= len;
  return out;
}

static std::vector<std::string> ConsumeStringVector(const uint8_t*& data, size_t& size)
{
  std::vector<std::string> out;
  size_t count = 0;
  if (size > 0)
  {
    count = ConsumeSizeT(data, size) % 8;
  }
  for (size_t i = 0; i < count && size > 0; ++i)
  {
    out.push_back(ConsumeString(data, size));
  }
  return out;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
  std::ofstream ofs("./dummy_file", std::ios::binary);
  if (ofs)
  {
    ofs.write(reinterpret_cast<const char*>(Data), static_cast<std::streamsize>(Size));
  }
  ofs.close();

  const uint8_t* ptr = Data;
  size_t remaining = Size;

  rapidcsv::LabelParams labelParams;
  labelParams.mColumnNameIdx = (remaining > 0) ? ((ConsumeInt(ptr, remaining) % 5) - 2) : 0;

  rapidcsv::SeparatorParams separatorParams;
  separatorParams.mSeparator = ConsumeChar(ptr, remaining, ',');
  separatorParams.mTrim = ConsumeBool(ptr, remaining);

  rapidcsv::ConverterParams converterParams;
  converterParams.mHasDefaultConverter = ConsumeBool(ptr, remaining);
  converterParams.mDefaultFloat = static_cast<long double>(ConsumeInt(ptr, remaining));
  converterParams.mDefaultInteger = static_cast<long long>(ConsumeInt(ptr, remaining));

  rapidcsv::LineReaderParams lineReaderParams;
  lineReaderParams.mSkipCommentLines = ConsumeBool(ptr, remaining);
  lineReaderParams.mCommentPrefix = ConsumeChar(ptr, remaining, '#');

  try
  {
    rapidcsv::Document doc("./dummy_file", labelParams, separatorParams, converterParams, lineReaderParams);

    for (int iter = 0; iter < 16; ++iter)
    {
      int op = (remaining > 0) ? (ConsumeInt(ptr, remaining) % 10) : iter;
      if (op < 0) op = -op;

      try
      {
        switch (op)
        {
          case 0:
          {
            size_t rowIdx = ConsumeSizeT(ptr, remaining) % 16;
            std::string rowName = ConsumeString(ptr, remaining);
            doc.SetRowName(rowIdx, rowName);
            break;
          }
          case 1:
          {
            size_t rowIdx = ConsumeSizeT(ptr, remaining) % 16;
            (void)doc.GetRowName(rowIdx);
            break;
          }
          case 2:
          {
            std::string rowName = ConsumeString(ptr, remaining);
            (void)doc.GetRow<std::string>(rowName);
            break;
          }
          case 3:
          {
            std::string rowName = ConsumeString(ptr, remaining);
            doc.RemoveRow(rowName);
            break;
          }
          case 4:
          {
            std::string rowName = ConsumeString(ptr, remaining);
            (void)doc.GetRowIdx(rowName);
            break;
          }
          case 5:
          {
            size_t rowIdx = ConsumeSizeT(ptr, remaining) % 16;
            std::vector<std::string> row = ConsumeStringVector(ptr, remaining);
            std::string rowName = ConsumeString(ptr, remaining);
            doc.InsertRow<std::string>(rowIdx, row, rowName);
            break;
          }
          case 6:
          {
            size_t rowIdx = ConsumeSizeT(ptr, remaining) % 16;
            std::vector<int> row;
            size_t count = (remaining > 0) ? (ConsumeSizeT(ptr, remaining) % 8) : 0;
            for (size_t i = 0; i < count && remaining > 0; ++i)
            {
              row.push_back(ConsumeInt(ptr, remaining));
            }
            std::string rowName = ConsumeString(ptr, remaining);
            doc.InsertRow<int>(rowIdx, row, rowName);
            break;
          }
          case 7:
          {
            size_t rowIdx = ConsumeSizeT(ptr, remaining) % 16;
            std::vector<std::string> emptyRow;
            std::string rowName = ConsumeString(ptr, remaining);
            doc.InsertRow<std::string>(rowIdx, emptyRow, rowName);
            break;
          }
          case 8:
          {
            std::string rowName = ConsumeString(ptr, remaining);
            int idx = doc.GetRowIdx(rowName);
            if (idx >= 0)
            {
              try
              {
                (void)doc.GetRowName(static_cast<size_t>(idx));
              }
              catch (...)
              {
              }
            }
            break;
          }
          case 9:
          default:
          {
            size_t rowIdx = ConsumeSizeT(ptr, remaining) % 16;
            std::string rowName = ConsumeString(ptr, remaining);
            try
            {
              doc.SetRowName(rowIdx, rowName);
            }
            catch (...)
            {
            }
            try
            {
              (void)doc.GetRowName(rowIdx);
            }
            catch (...)
            {
            }
            try
            {
              (void)doc.GetRow<std::string>(rowName);
            }
            catch (...)
            {
            }
            try
            {
              (void)doc.GetRowIdx(rowName);
            }
            catch (...)
            {
            }
            try
            {
              doc.RemoveRow(rowName);
            }
            catch (...)
            {
            }
            break;
          }
        }
      }
      catch (...)
      {
      }
    }
  }
  catch (...)
  {
  }

  return 0;
}