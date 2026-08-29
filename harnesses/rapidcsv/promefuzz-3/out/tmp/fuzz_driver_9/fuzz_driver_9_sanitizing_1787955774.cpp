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
#include <algorithm>

#include "rapidcsv.h"

static uint64_t ReadU64(const uint8_t* Data, size_t Size, size_t& Offset)
{
  uint64_t v = 0;
  for (size_t i = 0; i < 8; ++i)
  {
    v <<= 8;
    if (Offset < Size)
    {
      v |= Data[Offset++];
    }
  }
  return v;
}

static int ReadInt(const uint8_t* Data, size_t Size, size_t& Offset)
{
  return static_cast<int>(ReadU64(Data, Size, Offset));
}

static size_t ReadSizeT(const uint8_t* Data, size_t Size, size_t& Offset)
{
  return static_cast<size_t>(ReadU64(Data, Size, Offset));
}

static bool ReadBool(const uint8_t* Data, size_t Size, size_t& Offset)
{
  if (Offset >= Size) return false;
  return (Data[Offset++] & 1U) != 0;
}

static char ReadChar(const uint8_t* Data, size_t Size, size_t& Offset, char fallback)
{
  if (Offset >= Size) return fallback;
  return static_cast<char>(Data[Offset++]);
}

static std::string ReadString(const uint8_t* Data, size_t Size, size_t& Offset)
{
  size_t remaining = (Offset < Size) ? (Size - Offset) : 0;
  size_t len = remaining ? (ReadSizeT(Data, Size, Offset) % (remaining + 1)) : 0;
  if (Offset > Size) Offset = Size;
  len = std::min(len, Size - Offset);
  std::string s(reinterpret_cast<const char*>(Data + Offset), len);
  Offset += len;
  return s;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
  size_t offset = 0;

  rapidcsv::LabelParams labelParams;
  labelParams.mColumnNameIdx = (ReadBool(Data, Size, offset) ? 0 : -1);

  rapidcsv::SeparatorParams separatorParams;
  separatorParams.mSeparator = ReadChar(Data, Size, offset, ',');
  if (separatorParams.mSeparator == '\0' || separatorParams.mSeparator == '\n' || separatorParams.mSeparator == '\r')
  {
    separatorParams.mSeparator = ',';
  }
  separatorParams.mTrim = ReadBool(Data, Size, offset);

  rapidcsv::ConverterParams converterParams;
  converterParams.mHasDefaultConverter = ReadBool(Data, Size, offset);
  converterParams.mDefaultFloat = static_cast<long double>(ReadInt(Data, Size, offset));
  converterParams.mDefaultInteger = static_cast<long long>(ReadInt(Data, Size, offset));

  rapidcsv::LineReaderParams lineReaderParams;
  lineReaderParams.mSkipCommentLines = ReadBool(Data, Size, offset);
  lineReaderParams.mCommentPrefix = ReadChar(Data, Size, offset, '#');
  if (lineReaderParams.mCommentPrefix == '\0')
  {
    lineReaderParams.mCommentPrefix = '#';
  }

  std::string initialCsv = ReadString(Data, Size, offset);
  {
    std::ofstream ofs("./dummy_file", std::ios::binary);
    ofs.write(initialCsv.data(), static_cast<std::streamsize>(initialCsv.size()));
  }

  try
  {
    rapidcsv::Document doc("./dummy_file", labelParams, separatorParams, converterParams, lineReaderParams);

    std::string rowName1 = ReadString(Data, Size, offset);
    std::string rowName2 = ReadString(Data, Size, offset);
    std::string rowName3 = ReadString(Data, Size, offset);
    std::string colName1 = ReadString(Data, Size, offset);
    std::string colName2 = ReadString(Data, Size, offset);
    std::string colName3 = ReadString(Data, Size, offset);

    size_t colIdx1 = ReadSizeT(Data, Size, offset) % 16;
    size_t colIdx2 = ReadSizeT(Data, Size, offset) % 16;
    size_t colIdx3 = ReadSizeT(Data, Size, offset) % 16;
    size_t rowIdx1 = ReadSizeT(Data, Size, offset) % 16;
    size_t rowIdx2 = ReadSizeT(Data, Size, offset) % 16;
    size_t rowIdx3 = ReadSizeT(Data, Size, offset) % 16;

    std::string cell1 = ReadString(Data, Size, offset);
    std::string cell2 = ReadString(Data, Size, offset);
    std::string cell3 = ReadString(Data, Size, offset);
    std::string cell4 = ReadString(Data, Size, offset);
    std::string cell5 = ReadString(Data, Size, offset);
    std::string cell6 = ReadString(Data, Size, offset);

    try { doc.SetCell<std::string>(colIdx1, rowName1, cell1); } catch (...) {}
    try { doc.SetCell<std::string>(colName1, rowIdx1, cell2); } catch (...) {}
    try { doc.SetCell<std::string>(colIdx2, rowName2, cell3); } catch (...) {}
    try { doc.SetCell<std::string>(colName2, rowIdx2, cell4); } catch (...) {}
    try { doc.SetCell<std::string>(colIdx3, rowName3, cell5); } catch (...) {}
    try { doc.SetCell<std::string>(colName3, rowIdx3, cell6); } catch (...) {}

    try { (void)doc.GetCell<std::string>(colName1, rowName1); } catch (...) {}
    try { (void)doc.GetCell<std::string>(colIdx1, rowIdx1); } catch (...) {}
    try { (void)doc.GetCell<std::string>(colName2, rowName2); } catch (...) {}
    try { (void)doc.GetCell<std::string>(colIdx2, rowIdx2); } catch (...) {}
    try { (void)doc.GetCell<std::string>(colName3, rowName3); } catch (...) {}
    try { (void)doc.GetCell<std::string>(colIdx3, rowIdx3); } catch (...) {}

    try { doc.Save("./dummy_file"); } catch (...) {}
  }
  catch (...)
  {
  }

  return 0;
}