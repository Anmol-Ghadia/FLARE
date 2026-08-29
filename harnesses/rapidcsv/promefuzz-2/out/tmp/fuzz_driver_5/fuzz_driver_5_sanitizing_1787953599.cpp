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
#include <sstream>
#include <fstream>
#include <vector>
#include <limits>

#include "rapidcsv.h"

static int ToInt(const uint8_t* data, size_t size, size_t offset)
{
  if (offset + 4 > size) return 0;
  int v = static_cast<int>(data[offset]) |
          (static_cast<int>(data[offset + 1]) << 8) |
          (static_cast<int>(data[offset + 2]) << 16) |
          (static_cast<int>(data[offset + 3]) << 24);
  return v;
}

static long long ToLongLong(const uint8_t* data, size_t size, size_t offset)
{
  if (offset + 8 > size) return 0;
  long long v = 0;
  for (size_t i = 0; i < 8; ++i)
  {
    v |= (static_cast<long long>(data[offset + i]) << (i * 8));
  }
  return v;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
  std::string csv(reinterpret_cast<const char*>(Data), Size);

  int columnNameIdx1 = ToInt(Data, Size, 0) % 8 - 4;
  int rowNameIdx1 = ToInt(Data, Size, 4) % 8 - 4;
  char separator1 = (Size > 8) ? static_cast<char>(Data[8]) : ',';
  bool trim1 = (Size > 9) ? ((Data[9] & 1) != 0) : false;
  bool hasDefault1 = (Size > 10) ? ((Data[10] & 1) != 0) : false;
  long long defaultInt1 = ToLongLong(Data, Size, 11);
  long double defaultFloat1 = static_cast<long double>(defaultInt1);
  bool skipComments1 = (Size > 19) ? ((Data[19] & 1) != 0) : false;
  char commentPrefix1 = (Size > 20) ? static_cast<char>(Data[20]) : '#';

  int columnNameIdx2 = ToInt(Data, Size, 21) % 8 - 4;
  int rowNameIdx2 = ToInt(Data, Size, 25) % 8 - 4;
  char separator2 = (Size > 29) ? static_cast<char>(Data[29]) : ';';
  bool trim2 = (Size > 30) ? ((Data[30] & 1) != 0) : true;
  bool hasDefault2 = (Size > 31) ? ((Data[31] & 1) != 0) : true;
  long long defaultInt2 = ToLongLong(Data, Size, 32);
  long double defaultFloat2 = static_cast<long double>(defaultInt2);
  bool skipComments2 = (Size > 40) ? ((Data[40] & 1) != 0) : true;
  char commentPrefix2 = (Size > 41) ? static_cast<char>(Data[41]) : '%';

  rapidcsv::LabelParams labelParams1(columnNameIdx1, rowNameIdx1);
  rapidcsv::SeparatorParams separatorParams1(separator1, trim1);
  rapidcsv::ConverterParams converterParams1(hasDefault1, defaultFloat1, defaultInt1);
  rapidcsv::LineReaderParams lineReaderParams1(skipComments1, commentPrefix1);

  rapidcsv::LabelParams labelParams2(columnNameIdx2, rowNameIdx2);
  rapidcsv::SeparatorParams separatorParams2(separator2, trim2);
  rapidcsv::ConverterParams converterParams2(hasDefault2, defaultFloat2, defaultInt2);
  rapidcsv::LineReaderParams lineReaderParams2(skipComments2, commentPrefix2);

  {
    std::ofstream ofs("./dummy_file", std::ios::binary);
    ofs.write(csv.data(), static_cast<std::streamsize>(csv.size()));
  }

  try
  {
    rapidcsv::Document doc;

    (void)doc.GetRowCount();

    try
    {
      std::stringstream ss1(csv, std::ios::in | std::ios::out | std::ios::binary);
      doc.Load(ss1, labelParams1, separatorParams1, converterParams1, lineReaderParams1);
    }
    catch (...)
    {
    }

    (void)doc.GetRowCount();

    try
    {
      doc.Load(std::string("./dummy_file"), labelParams2, separatorParams2, converterParams2, lineReaderParams2);
    }
    catch (...)
    {
    }

    (void)doc.GetRowCount();
  }
  catch (...)
  {
  }

  return 0;
}