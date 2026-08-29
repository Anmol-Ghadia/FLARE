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
#include <functional>
#include <limits>
#include "rapidcsv.h"

static int32_t ReadI32(const uint8_t* data, size_t size, size_t& off)
{
  if (off + 4 > size) return 0;
  int32_t v = static_cast<int32_t>(data[off]) |
              (static_cast<int32_t>(data[off + 1]) << 8) |
              (static_cast<int32_t>(data[off + 2]) << 16) |
              (static_cast<int32_t>(data[off + 3]) << 24);
  off += 4;
  return v;
}

static uint8_t ReadU8(const uint8_t* data, size_t size, size_t& off)
{
  if (off >= size) return 0;
  return data[off++];
}

static bool ReadBool(const uint8_t* data, size_t size, size_t& off)
{
  return (ReadU8(data, size, off) & 1U) != 0;
}

static std::string ReadString(const uint8_t* data, size_t size, size_t& off, size_t maxLen)
{
  size_t remaining = (off < size) ? (size - off) : 0;
  size_t len = remaining ? (ReadU8(data, size, off) % (maxLen + 1)) : 0;
  if (len > size - off) len = size - off;
  std::string s(reinterpret_cast<const char*>(data + off), len);
  off += len;
  return s;
}

static void WriteDummyFile(const uint8_t* data, size_t size)
{
  std::ofstream ofs("./dummy_file", std::ios::binary);
  if (!ofs) return;
  ofs.write(reinterpret_cast<const char*>(data), static_cast<std::streamsize>(size));
}

static rapidcsv::LabelParams MakeLabelParams(const uint8_t* data, size_t size, size_t& off)
{
  rapidcsv::LabelParams p;
  p.mColumnNameIdx = ReadI32(data, size, off);
  return p;
}

static rapidcsv::SeparatorParams MakeSeparatorParams(const uint8_t* data, size_t size, size_t& off)
{
  rapidcsv::SeparatorParams p;
  p.mSeparator = static_cast<char>(ReadU8(data, size, off));
  p.mTrim = ReadBool(data, size, off);
  return p;
}

static rapidcsv::ConverterParams MakeConverterParams(const uint8_t* data, size_t size, size_t& off)
{
  rapidcsv::ConverterParams p;
  p.mHasDefaultConverter = ReadBool(data, size, off);
  p.mDefaultFloat = static_cast<long double>(ReadI32(data, size, off));
  p.mDefaultInteger = static_cast<long long>(ReadI32(data, size, off));
  return p;
}

static rapidcsv::LineReaderParams MakeLineReaderParams(const uint8_t* data, size_t size, size_t& off)
{
  rapidcsv::LineReaderParams p;
  p.mSkipCommentLines = ReadBool(data, size, off);
  p.mCommentPrefix = static_cast<char>(ReadU8(data, size, off));
  return p;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
  WriteDummyFile(Data, Size);

  size_t off = 0;

  rapidcsv::LabelParams labelParams1 = MakeLabelParams(Data, Size, off);
  rapidcsv::SeparatorParams sepParams1 = MakeSeparatorParams(Data, Size, off);
  rapidcsv::ConverterParams convParams1 = MakeConverterParams(Data, Size, off);
  rapidcsv::LineReaderParams lineParams1 = MakeLineReaderParams(Data, Size, off);

  rapidcsv::LabelParams labelParams2 = MakeLabelParams(Data, Size, off);
  rapidcsv::SeparatorParams sepParams2 = MakeSeparatorParams(Data, Size, off);
  rapidcsv::ConverterParams convParams2 = MakeConverterParams(Data, Size, off);
  rapidcsv::LineReaderParams lineParams2 = MakeLineReaderParams(Data, Size, off);

  std::string colName = ReadString(Data, Size, off, 32);
  std::string rowName = ReadString(Data, Size, off, 32);

  try
  {
    rapidcsv::Document doc("./dummy_file", labelParams1, sepParams1, convParams1, lineParams1);

    try
    {
      doc.Load("./dummy_file", labelParams2, sepParams2, convParams2, lineParams2);
    }
    catch (...)
    {
    }

    try
    {
      doc.Load("./dummy_file", labelParams1, sepParams1, convParams1, lineParams1);
    }
    catch (...)
    {
    }

    try
    {
      rapidcsv::Converter<int> convInt(convParams1);
      int v = 0;
      std::string s = ReadString(Data, Size, off, 64);
      convInt.ToVal(s, v);
      std::string out;
      convInt.ToStr(v, out);
    }
    catch (...)
    {
    }

    try
    {
      rapidcsv::Converter<long long> convLL(convParams2);
      long long v = 0;
      std::string s = ReadString(Data, Size, off, 64);
      convLL.ToVal(s, v);
      std::string out;
      convLL.ToStr(v, out);
    }
    catch (...)
    {
    }

    try
    {
      rapidcsv::Converter<double> convD(convParams1);
      double v = 0.0;
      std::string s = ReadString(Data, Size, off, 64);
      convD.ToVal(s, v);
      std::string out;
      convD.ToStr(v, out);
    }
    catch (...)
    {
    }

    try
    {
      rapidcsv::Converter<std::string> convS(convParams2);
      std::string in = ReadString(Data, Size, off, 64);
      std::string val;
      convS.ToVal(in, val);
      std::string out;
      convS.ToStr(val, out);
    }
    catch (...)
    {
    }

    try
    {
      std::vector<int> col = doc.GetColumn<int>(colName, [](const std::string& s) -> int {
        rapidcsv::ConverterParams cp;
        rapidcsv::Converter<int> c(cp);
        int v = 0;
        c.ToVal(s, v);
        return v;
      });
      (void)col;
    }
    catch (...)
    {
    }

    try
    {
      std::vector<std::string> col = doc.GetColumn<std::string>(colName, [](const std::string& s) -> std::string {
        rapidcsv::ConverterParams cp;
        rapidcsv::Converter<std::string> c(cp);
        std::string v;
        c.ToVal(s, v);
        return v;
      });
      (void)col;
    }
    catch (...)
    {
    }

    try
    {
      int cell = doc.GetCell<int>(colName, rowName, [](const std::string& s) -> int {
        rapidcsv::ConverterParams cp;
        rapidcsv::Converter<int> c(cp);
        int v = 0;
        c.ToVal(s, v);
        return v;
      });
      (void)cell;
    }
    catch (...)
    {
    }

    try
    {
      std::string cell = doc.GetCell<std::string>(colName, rowName, [](const std::string& s) -> std::string {
        rapidcsv::ConverterParams cp;
        rapidcsv::Converter<std::string> c(cp);
        std::string v;
        c.ToVal(s, v);
        return v;
      });
      (void)cell;
    }
    catch (...)
    {
    }

    try
    {
      rapidcsv::no_converter ex;
      const char* msg = ex.what();
      (void)msg;
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