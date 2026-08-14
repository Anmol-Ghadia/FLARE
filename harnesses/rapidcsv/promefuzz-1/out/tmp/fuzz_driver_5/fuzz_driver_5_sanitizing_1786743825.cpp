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
#include <limits>

#include "rapidcsv.h"

static uint32_t ReadU32(const uint8_t* data, size_t size, size_t& offset)
{
  uint32_t v = 0;
  for (int i = 0; i < 4; ++i)
  {
    v <<= 8;
    if (offset < size)
    {
      v |= data[offset++];
    }
  }
  return v;
}

static bool ReadBool(const uint8_t* data, size_t size, size_t& offset)
{
  if (offset >= size) return false;
  return (data[offset++] & 1) != 0;
}

static char ReadChar(const uint8_t* data, size_t size, size_t& offset, char fallback)
{
  if (offset >= size) return fallback;
  return static_cast<char>(data[offset++]);
}

static int ReadIntRange(const uint8_t* data, size_t size, size_t& offset, int minv, int maxv)
{
  if (minv >= maxv) return minv;
  uint32_t v = ReadU32(data, size, offset);
  return minv + static_cast<int>(v % static_cast<uint32_t>(maxv - minv + 1));
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
  size_t offset = 0;

  rapidcsv::LabelParams labelParams1(
      ReadIntRange(Data, Size, offset, -1, 8),
      ReadIntRange(Data, Size, offset, -1, 8));

  rapidcsv::SeparatorParams separatorParams1(
      ReadChar(Data, Size, offset, ','),
      ReadBool(Data, Size, offset),
      ReadBool(Data, Size, offset),
      ReadBool(Data, Size, offset),
      ReadBool(Data, Size, offset),
      ReadChar(Data, Size, offset, '"'));

  rapidcsv::ConverterParams converterParams1(
      ReadBool(Data, Size, offset),
      static_cast<long double>(ReadU32(Data, Size, offset)),
      static_cast<long long>(ReadU32(Data, Size, offset)),
      ReadBool(Data, Size, offset));

  rapidcsv::LineReaderParams lineReaderParams1(
      ReadBool(Data, Size, offset),
      ReadChar(Data, Size, offset, '#'),
      ReadBool(Data, Size, offset));

  rapidcsv::LabelParams labelParams2(
      ReadIntRange(Data, Size, offset, -1, 8),
      ReadIntRange(Data, Size, offset, -1, 8));

  rapidcsv::SeparatorParams separatorParams2(
      ReadChar(Data, Size, offset, ';'),
      ReadBool(Data, Size, offset),
      ReadBool(Data, Size, offset),
      ReadBool(Data, Size, offset),
      ReadBool(Data, Size, offset),
      ReadChar(Data, Size, offset, '\''));

  rapidcsv::ConverterParams converterParams2(
      ReadBool(Data, Size, offset),
      static_cast<long double>(ReadU32(Data, Size, offset)) / 3.0L,
      static_cast<long long>(ReadU32(Data, Size, offset)),
      ReadBool(Data, Size, offset));

  rapidcsv::LineReaderParams lineReaderParams2(
      ReadBool(Data, Size, offset),
      ReadChar(Data, Size, offset, '%'),
      ReadBool(Data, Size, offset));

  std::string csv(reinterpret_cast<const char*>(Data + offset), Size - offset);

  {
    std::ofstream ofs("./dummy_file", std::ios::binary);
    ofs.write(csv.data(), static_cast<std::streamsize>(csv.size()));
  }

  std::stringstream ss(csv, std::ios::in | std::ios::out | std::ios::binary);

  try
  {
    rapidcsv::Document doc(ss, labelParams1, separatorParams1, converterParams1, lineReaderParams1);

    try
    {
      (void)doc.GetRowCount();
    }
    catch (...)
    {
    }

    try
    {
      ss.clear();
      ss.seekg(0, std::ios::beg);
      doc.Load(ss, labelParams1, separatorParams1, converterParams1, lineReaderParams1);
    }
    catch (...)
    {
    }

    try
    {
      (void)doc.GetRowCount();
    }
    catch (...)
    {
    }

    try
    {
      doc.Load(std::string("./dummy_file"), labelParams2, separatorParams2, converterParams2, lineReaderParams2);
    }
    catch (...)
    {
    }

    try
    {
      (void)doc.GetRowCount();
    }
    catch (...)
    {
    }
  }
  catch (...)
  {
    try
    {
      rapidcsv::LabelParams fallbackLabel(-1, -1);
      rapidcsv::Document doc2(ss, fallbackLabel, separatorParams1, converterParams1, lineReaderParams1);

      try
      {
        (void)doc2.GetRowCount();
      }
      catch (...)
      {
      }

      try
      {
        ss.clear();
        ss.seekg(0, std::ios::beg);
        doc2.Load(ss, labelParams1, separatorParams1, converterParams1, lineReaderParams1);
      }
      catch (...)
      {
      }

      try
      {
        (void)doc2.GetRowCount();
      }
      catch (...)
      {
      }

      try
      {
        doc2.Load(std::string("./dummy_file"), labelParams2, separatorParams2, converterParams2, lineReaderParams2);
      }
      catch (...)
      {
      }

      try
      {
        (void)doc2.GetRowCount();
      }
      catch (...)
      {
      }
    }
    catch (...)
    {
    }
  }

  return 0;
}