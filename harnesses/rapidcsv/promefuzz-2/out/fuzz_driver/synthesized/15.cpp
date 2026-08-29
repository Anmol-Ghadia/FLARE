// This fuzz driver is generated for library rapidcsv, aiming to fuzz the following functions:
// rapidcsv::Document::Load at rapidcsv.h:554:10 in rapidcsv.h
// rapidcsv::Document::Load at rapidcsv.h:554:10 in rapidcsv.h
// rapidcsv::Converter::ToVal at rapidcsv.h:184:10 in rapidcsv.h
// rapidcsv::Converter::ToStr at rapidcsv.h:132:10 in rapidcsv.h
// rapidcsv::Converter::ToVal at rapidcsv.h:184:10 in rapidcsv.h
// rapidcsv::Converter::ToStr at rapidcsv.h:132:10 in rapidcsv.h
// rapidcsv::Converter::ToVal at rapidcsv.h:184:10 in rapidcsv.h
// rapidcsv::Converter::ToStr at rapidcsv.h:132:10 in rapidcsv.h
// rapidcsv::Converter<std::basic_string<char>>::ToVal at rapidcsv.h:329:39 in rapidcsv.h
// rapidcsv::Converter<std::basic_string<char>>::ToStr at rapidcsv.h:318:39 in rapidcsv.h
// rapidcsv::Document::GetColumn at rapidcsv.h:730:20 in rapidcsv.h
// rapidcsv::Document::GetColumn at rapidcsv.h:691:20 in rapidcsv.h
// rapidcsv::Document::GetColumn at rapidcsv.h:730:20 in rapidcsv.h
// rapidcsv::Document::GetColumn at rapidcsv.h:691:20 in rapidcsv.h
// rapidcsv::Document::GetCell at rapidcsv.h:1233:7 in rapidcsv.h
// rapidcsv::Document::GetCell at rapidcsv.h:1191:7 in rapidcsv.h
// rapidcsv::Document::GetCell at rapidcsv.h:1276:7 in rapidcsv.h
// rapidcsv::Document::GetCell at rapidcsv.h:1313:7 in rapidcsv.h
// rapidcsv::Document::GetCell at rapidcsv.h:1233:7 in rapidcsv.h
// rapidcsv::Document::GetCell at rapidcsv.h:1191:7 in rapidcsv.h
// rapidcsv::no_converter::what at rapidcsv.h:102:17 in rapidcsv.h
// rapidcsv::Converter::ToVal at rapidcsv.h:184:10 in rapidcsv.h
// rapidcsv::Converter<std::basic_string<char>>::ToVal at rapidcsv.h:329:39 in rapidcsv.h
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <functional>
#include <string>
#include <vector>

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

static void ConvToInt(const std::string& s, int& out)
{
  rapidcsv::ConverterParams cp;
  rapidcsv::Converter<int> c(cp);
  c.ToVal(s, out);
}

static void ConvToString(const std::string& s, std::string& out)
{
  rapidcsv::ConverterParams cp;
  rapidcsv::Converter<std::string> c(cp);
  c.ToVal(s, out);
}

extern "C" int LLVMFuzzerTestOneInput_15(const uint8_t* Data, size_t Size)
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
  size_t colIdx = static_cast<size_t>(ReadU8(Data, Size, off));
  size_t rowIdx = static_cast<size_t>(ReadU8(Data, Size, off));

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
      std::vector<int> col = doc.GetColumn<int>(colName, rapidcsv::ConvFunc<int>(ConvToInt));
      (void)col;
    }
    catch (...)
    {
    }

    try
    {
      std::vector<int> col = doc.GetColumn<int>(colIdx, rapidcsv::ConvFunc<int>(ConvToInt));
      (void)col;
    }
    catch (...)
    {
    }

    try
    {
      std::vector<std::string> col = doc.GetColumn<std::string>(colName, rapidcsv::ConvFunc<std::string>(ConvToString));
      (void)col;
    }
    catch (...)
    {
    }

    try
    {
      std::vector<std::string> col = doc.GetColumn<std::string>(colIdx, rapidcsv::ConvFunc<std::string>(ConvToString));
      (void)col;
    }
    catch (...)
    {
    }

    try
    {
      int cell = doc.GetCell<int>(colName, rowName, rapidcsv::ConvFunc<int>(ConvToInt));
      (void)cell;
    }
    catch (...)
    {
    }

    try
    {
      int cell = doc.GetCell<int>(colIdx, rowIdx, rapidcsv::ConvFunc<int>(ConvToInt));
      (void)cell;
    }
    catch (...)
    {
    }

    try
    {
      int cell = doc.GetCell<int>(colName, rowIdx, rapidcsv::ConvFunc<int>(ConvToInt));
      (void)cell;
    }
    catch (...)
    {
    }

    try
    {
      int cell = doc.GetCell<int>(colIdx, rowName, rapidcsv::ConvFunc<int>(ConvToInt));
      (void)cell;
    }
    catch (...)
    {
    }

    try
    {
      std::string cell = doc.GetCell<std::string>(colName, rowName, rapidcsv::ConvFunc<std::string>(ConvToString));
      (void)cell;
    }
    catch (...)
    {
    }

    try
    {
      std::string cell = doc.GetCell<std::string>(colIdx, rowIdx, rapidcsv::ConvFunc<std::string>(ConvToString));
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