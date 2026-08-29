// This fuzz driver is generated for library rapidcsv, aiming to fuzz the following functions:
// rapidcsv::Document::GetRowCount at rapidcsv.h:1158:12 in rapidcsv.h
// rapidcsv::Document::Load at rapidcsv.h:554:10 in rapidcsv.h
// rapidcsv::Document::GetRowCount at rapidcsv.h:1158:12 in rapidcsv.h
// rapidcsv::Document::GetRowCount at rapidcsv.h:1158:12 in rapidcsv.h
// rapidcsv::Document::Load at rapidcsv.h:577:10 in rapidcsv.h
#include <cstdint>
#include <cstddef>
#include <fstream>
#include <limits>
#include <sstream>
#include <string>

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

static int SafeColumnNameIdx(int raw)
{
  if (raw < -1) return -1;
  if (raw > 16) return 16;
  return raw;
}

static int SafeRowNameIdx(int raw)
{
  if (raw < -1) return -1;
  if (raw > 16) return 16;
  return raw;
}

extern "C" int LLVMFuzzerTestOneInput_5(const uint8_t* Data, size_t Size)
{
  std::string csv(reinterpret_cast<const char*>(Data), Size);

  int columnNameIdx1 = SafeColumnNameIdx((ToInt(Data, Size, 0) % 18));
  int rowNameIdx1 = SafeRowNameIdx((ToInt(Data, Size, 4) % 18) - 1);
  char separator1 = (Size > 8) ? static_cast<char>(Data[8]) : ',';
  bool trim1 = (Size > 9) ? ((Data[9] & 1) != 0) : false;
  bool hasDefault1 = (Size > 10) ? ((Data[10] & 1) != 0) : false;
  long long defaultInt1 = ToLongLong(Data, Size, 11);
  long double defaultFloat1 = static_cast<long double>(defaultInt1);
  bool skipComments1 = (Size > 19) ? ((Data[19] & 1) != 0) : false;
  char commentPrefix1 = (Size > 20) ? static_cast<char>(Data[20]) : '#';

  int columnNameIdx2 = SafeColumnNameIdx((ToInt(Data, Size, 21) % 18));
  int rowNameIdx2 = SafeRowNameIdx((ToInt(Data, Size, 25) % 18) - 1);
  char separator2 = (Size > 29) ? static_cast<char>(Data[29]) : ';';
  bool trim2 = (Size > 30) ? ((Data[30] & 1) != 0) : true;
  bool hasDefault2 = (Size > 31) ? ((Data[31] & 1) != 0) : true;
  long long defaultInt2 = ToLongLong(Data, Size, 32);
  long double defaultFloat2 = static_cast<long double>(defaultInt2);
  bool skipComments2 = (Size > 40) ? ((Data[40] & 1) != 0) : true;
  char commentPrefix2 = (Size > 41) ? static_cast<char>(Data[41]) : '%';

  {
    std::ofstream ofs("./dummy_file", std::ios::binary);
    ofs.write(csv.data(), static_cast<std::streamsize>(csv.size()));
  }

  try
  {
    rapidcsv::Document doc;

    try
    {
      (void)doc.GetRowCount();
    }
    catch (...)
    {
    }

    try
    {
      rapidcsv::LabelParams labelParams1(columnNameIdx1, rowNameIdx1);
      rapidcsv::SeparatorParams separatorParams1(separator1, trim1);
      rapidcsv::ConverterParams converterParams1(hasDefault1, defaultFloat1, defaultInt1);
      rapidcsv::LineReaderParams lineReaderParams1(skipComments1, commentPrefix1);
      std::stringstream ss1(csv, std::ios::in | std::ios::out | std::ios::binary);
      doc.Load(ss1, labelParams1, separatorParams1, converterParams1, lineReaderParams1);
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
      rapidcsv::LabelParams labelParams2(columnNameIdx2, rowNameIdx2);
      rapidcsv::SeparatorParams separatorParams2(separator2, trim2);
      rapidcsv::ConverterParams converterParams2(hasDefault2, defaultFloat2, defaultInt2);
      rapidcsv::LineReaderParams lineReaderParams2(skipComments2, commentPrefix2);
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
  }

  return 0;
}