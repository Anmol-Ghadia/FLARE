// This fuzz driver is generated for library rapidcsv, aiming to fuzz the following functions:
// rapidcsv::Document::GetRowNames at rapidcsv.h:1551:30 in rapidcsv.h
// rapidcsv::Document::RemoveColumn at rapidcsv.h:827:10 in rapidcsv.h
// rapidcsv::Document::GetColumnNames at rapidcsv.h:1479:30 in rapidcsv.h
// rapidcsv::Document::RemoveColumn at rapidcsv.h:827:10 in rapidcsv.h
// rapidcsv::Document::GetColumnName at rapidcsv.h:1425:17 in rapidcsv.h
// rapidcsv::Document::Load at rapidcsv.h:577:10 in rapidcsv.h
// rapidcsv::Document::Load at rapidcsv.h:577:10 in rapidcsv.h
// rapidcsv::Document::GetColumnNames at rapidcsv.h:1479:30 in rapidcsv.h
// rapidcsv::Document::GetColumnName at rapidcsv.h:1425:17 in rapidcsv.h
// rapidcsv::Document::RemoveColumn at rapidcsv.h:827:10 in rapidcsv.h
// rapidcsv::Document::GetRowNames at rapidcsv.h:1551:30 in rapidcsv.h
// rapidcsv::Document::Clear at rapidcsv.h:619:10 in rapidcsv.h
// rapidcsv::Document::GetColumnNames at rapidcsv.h:1479:30 in rapidcsv.h
// rapidcsv::Document::GetRowNames at rapidcsv.h:1551:30 in rapidcsv.h
// rapidcsv::Document::Load at rapidcsv.h:577:10 in rapidcsv.h
// rapidcsv::Document::GetColumnNames at rapidcsv.h:1479:30 in rapidcsv.h
// rapidcsv::Document::RemoveColumn at rapidcsv.h:827:10 in rapidcsv.h
// rapidcsv::Document::RemoveColumn at rapidcsv.h:827:10 in rapidcsv.h
// rapidcsv::Document::GetColumnName at rapidcsv.h:1425:17 in rapidcsv.h
// rapidcsv::Document::Load at rapidcsv.h:577:10 in rapidcsv.h
// rapidcsv::Document::GetColumnNames at rapidcsv.h:1479:30 in rapidcsv.h
// rapidcsv::Document::GetRowNames at rapidcsv.h:1551:30 in rapidcsv.h
// rapidcsv::Document::Clear at rapidcsv.h:619:10 in rapidcsv.h
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
#include <sstream>
#include <fstream>
#include <algorithm>

#include "rapidcsv.h"

static int ToInt(const uint8_t* data, size_t size, size_t& offset, int fallback)
{
  if (offset + sizeof(uint32_t) > size) {
    return fallback;
  }
  uint32_t v = 0;
  v |= static_cast<uint32_t>(data[offset]);
  v |= static_cast<uint32_t>(data[offset + 1]) << 8;
  v |= static_cast<uint32_t>(data[offset + 2]) << 16;
  v |= static_cast<uint32_t>(data[offset + 3]) << 24;
  offset += sizeof(uint32_t);
  return static_cast<int>(v);
}

static bool ToBool(const uint8_t* data, size_t size, size_t& offset, bool fallback)
{
  if (offset >= size) {
    return fallback;
  }
  return (data[offset++] & 1U) != 0;
}

static char ToChar(const uint8_t* data, size_t size, size_t& offset, char fallback)
{
  if (offset >= size) {
    return fallback;
  }
  return static_cast<char>(data[offset++]);
}

static long long ToLongLong(const uint8_t* data, size_t size, size_t& offset, long long fallback)
{
  if (offset + sizeof(uint64_t) > size) {
    return fallback;
  }
  uint64_t v = 0;
  for (size_t i = 0; i < sizeof(uint64_t); ++i) {
    v |= static_cast<uint64_t>(data[offset + i]) << (8 * i);
  }
  offset += sizeof(uint64_t);
  return static_cast<long long>(v);
}

static long double ToLongDouble(const uint8_t* data, size_t size, size_t& offset, long double fallback)
{
  if (offset + sizeof(uint64_t) > size) {
    return fallback;
  }
  uint64_t v = 0;
  for (size_t i = 0; i < sizeof(uint64_t); ++i) {
    v |= static_cast<uint64_t>(data[offset + i]) << (8 * i);
  }
  offset += sizeof(uint64_t);
  return static_cast<long double>(v % 1000000ULL) / 100.0L;
}

static std::string ToString(const uint8_t* data, size_t size, size_t& offset)
{
  if (offset >= size) {
    return std::string();
  }
  size_t remaining = size - offset;
  size_t len = static_cast<size_t>(data[offset++]) % (remaining + 1);
  len = std::min(len, size - offset);
  std::string s(reinterpret_cast<const char*>(data + offset), len);
  offset += len;
  return s;
}

static rapidcsv::LabelParams MakeLabelParams(const uint8_t* data, size_t size, size_t& offset)
{
  rapidcsv::LabelParams params;
  params.mColumnNameIdx = (ToInt(data, size, offset, 0) % 7) - 3;
  return params;
}

static rapidcsv::SeparatorParams MakeSeparatorParams(const uint8_t* data, size_t size, size_t& offset)
{
  rapidcsv::SeparatorParams params;
  char sep = ToChar(data, size, offset, ',');
  if (sep == '\0' || sep == '\n' || sep == '\r') {
    sep = ',';
  }
  params.mSeparator = sep;
  params.mTrim = ToBool(data, size, offset, false);
  return params;
}

static rapidcsv::ConverterParams MakeConverterParams(const uint8_t* data, size_t size, size_t& offset)
{
  rapidcsv::ConverterParams params;
  params.mHasDefaultConverter = ToBool(data, size, offset, false);
  params.mDefaultInteger = ToLongLong(data, size, offset, 0);
  params.mDefaultFloat = ToLongDouble(data, size, offset, 0.0L);
  return params;
}

static rapidcsv::LineReaderParams MakeLineReaderParams(const uint8_t* data, size_t size, size_t& offset)
{
  rapidcsv::LineReaderParams params;
  params.mSkipCommentLines = ToBool(data, size, offset, false);
  char prefix = ToChar(data, size, offset, '#');
  if (prefix == '\0' || prefix == '\n' || prefix == '\r') {
    prefix = '#';
  }
  params.mCommentPrefix = prefix;
  return params;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
  size_t offset = 0;

  rapidcsv::LabelParams labelParams1 = MakeLabelParams(Data, Size, offset);
  rapidcsv::SeparatorParams separatorParams1 = MakeSeparatorParams(Data, Size, offset);
  rapidcsv::ConverterParams converterParams1 = MakeConverterParams(Data, Size, offset);
  rapidcsv::LineReaderParams lineReaderParams1 = MakeLineReaderParams(Data, Size, offset);

  rapidcsv::LabelParams labelParams2 = MakeLabelParams(Data, Size, offset);
  rapidcsv::SeparatorParams separatorParams2 = MakeSeparatorParams(Data, Size, offset);
  rapidcsv::ConverterParams converterParams2 = MakeConverterParams(Data, Size, offset);
  rapidcsv::LineReaderParams lineReaderParams2 = MakeLineReaderParams(Data, Size, offset);

  std::string csv(reinterpret_cast<const char*>(Data + offset), Size - offset);

  {
    std::ofstream out("./dummy_file", std::ios::binary);
    out.write(csv.data(), static_cast<std::streamsize>(csv.size()));
  }

  try {
    rapidcsv::Document doc("./dummy_file", labelParams1, separatorParams1, converterParams1, lineReaderParams1);

    try {
      std::vector<std::string> rowNames = doc.GetRowNames();
      for (const auto& name : rowNames) {
        if (!name.empty()) {
          try {
            doc.RemoveColumn(name);
          } catch (...) {
          }
        }
      }
    } catch (...) {
    }

    try {
      std::vector<std::string> colNames = doc.GetColumnNames();
      for (const auto& name : colNames) {
        try {
          doc.RemoveColumn(name);
        } catch (...) {
        }
      }
    } catch (...) {
    }

    for (size_t i = 0; i < 8; ++i) {
      try {
        (void)doc.GetColumnName(i);
      } catch (...) {
      }
    }

    try {
      std::ifstream in("./dummy_file", std::ios::binary);
      if (in) {
        doc.Load(in, labelParams2, separatorParams2, converterParams2, lineReaderParams2);
      }
    } catch (...) {
    }

    try {
      std::stringstream ss;
      ss.write(csv.data(), static_cast<std::streamsize>(csv.size()));
      doc.Load(ss, labelParams1, separatorParams1, converterParams1, lineReaderParams1);
    } catch (...) {
    }

    try {
      std::vector<std::string> colNames = doc.GetColumnNames();
      for (size_t i = 0; i < colNames.size(); ++i) {
        try {
          (void)doc.GetColumnName(i);
        } catch (...) {
        }
      }
      for (const auto& name : colNames) {
        try {
          doc.RemoveColumn(name);
        } catch (...) {
        }
      }
    } catch (...) {
    }

    try {
      (void)doc.GetRowNames();
    } catch (...) {
    }

    try {
      doc.Clear();
    } catch (...) {
    }

    try {
      (void)doc.GetColumnNames();
    } catch (...) {
    }

    try {
      (void)doc.GetRowNames();
    } catch (...) {
    }

    try {
      std::stringstream ss2;
      ss2.write(csv.data(), static_cast<std::streamsize>(csv.size()));
      doc.Load(ss2, labelParams2, separatorParams2, converterParams2, lineReaderParams2);
    } catch (...) {
    }

    try {
      std::vector<std::string> colNames = doc.GetColumnNames();
      if (!colNames.empty()) {
        try {
          doc.RemoveColumn(colNames[0]);
        } catch (...) {
        }
      } else {
        std::string arbitrary = ToString(Data, Size, offset);
        try {
          doc.RemoveColumn(arbitrary);
        } catch (...) {
        }
      }
    } catch (...) {
    }

    for (size_t i = 0; i < 4; ++i) {
      try {
        (void)doc.GetColumnName(static_cast<size_t>(ToInt(Data, Size, offset, static_cast<int>(i))));
      } catch (...) {
      }
    }
  } catch (...) {
    try {
      rapidcsv::Document doc;
      try {
        std::stringstream ss;
        ss.write(csv.data(), static_cast<std::streamsize>(csv.size()));
        doc.Load(ss, labelParams1, separatorParams1, converterParams1, lineReaderParams1);
      } catch (...) {
      }
      try {
        (void)doc.GetColumnNames();
      } catch (...) {
      }
      try {
        (void)doc.GetRowNames();
      } catch (...) {
      }
      try {
        doc.Clear();
      } catch (...) {
      }
    } catch (...) {
    }
  }

  return 0;
}