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

static int ToInt(const uint8_t* data, size_t size, size_t& offset, int fallback)
{
  if (offset + sizeof(uint32_t) > size) {
    offset = size;
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

static size_t ToSizeT(const uint8_t* data, size_t size, size_t& offset, size_t fallback)
{
  if (offset + sizeof(uint32_t) > size) {
    offset = size;
    return fallback;
  }

  uint32_t v = 0;
  v |= static_cast<uint32_t>(data[offset]);
  v |= static_cast<uint32_t>(data[offset + 1]) << 8;
  v |= static_cast<uint32_t>(data[offset + 2]) << 16;
  v |= static_cast<uint32_t>(data[offset + 3]) << 24;
  offset += sizeof(uint32_t);
  return static_cast<size_t>(v);
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

static std::string ToString(const uint8_t* data, size_t size, size_t& offset)
{
  size_t len = ToSizeT(data, size, offset, 0);
  if (offset > size) {
    return std::string();
  }
  len = std::min(len, size - offset);
  std::string s(reinterpret_cast<const char*>(data + offset), len);
  offset += len;
  return s;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
  {
    std::ofstream ofs("./dummy_file", std::ios::binary);
    if (ofs) {
      ofs.write(reinterpret_cast<const char*>(Data), static_cast<std::streamsize>(Size));
    }
  }

  size_t offset = 0;

  rapidcsv::LabelParams labelParams;
  labelParams.mColumnNameIdx = ToInt(Data, Size, offset, 0);

  rapidcsv::SeparatorParams separatorParams;
  separatorParams.mSeparator = ToChar(Data, Size, offset, ',');
  separatorParams.mTrim = ToBool(Data, Size, offset, false);

  rapidcsv::ConverterParams converterParams;
  converterParams.mHasDefaultConverter = ToBool(Data, Size, offset, false);
  converterParams.mDefaultFloat = static_cast<long double>(ToInt(Data, Size, offset, 0));
  converterParams.mDefaultInteger = static_cast<long long>(ToInt(Data, Size, offset, 0));

  rapidcsv::LineReaderParams lineReaderParams;
  lineReaderParams.mSkipCommentLines = ToBool(Data, Size, offset, false);
  lineReaderParams.mCommentPrefix = ToChar(Data, Size, offset, '#');

  try {
    rapidcsv::Document doc("./dummy_file",
                           labelParams,
                           separatorParams,
                           converterParams,
                           lineReaderParams);

    try {
      std::vector<std::string> colNames = doc.GetColumnNames();
      for (const auto& name : colNames) {
        try {
          (void)doc.GetColumnIdx(name);
        } catch (...) {
        }
      }
    } catch (...) {
    }

    try {
      std::vector<std::string> rowNames = doc.GetRowNames();
      for (const auto& name : rowNames) {
        try {
          (void)doc.GetRowName(static_cast<size_t>(&name - &rowNames[0]));
        } catch (...) {
        }
        try {
          doc.RemoveRow(name);
        } catch (...) {
        }
      }
    } catch (...) {
    }

    for (int i = 0; i < 4; ++i) {
      try {
        size_t colIdx = ToSizeT(Data, Size, offset, static_cast<size_t>(i));
        std::string newName = ToString(Data, Size, offset);
        doc.SetColumnName(colIdx, newName);
      } catch (...) {
      }

      try {
        std::vector<std::string> colNames2 = doc.GetColumnNames();
        for (const auto& n : colNames2) {
          try {
            (void)doc.GetColumnIdx(n);
          } catch (...) {
          }
        }
      } catch (...) {
      }

      try {
        std::string queryName = ToString(Data, Size, offset);
        (void)doc.GetColumnIdx(queryName);
      } catch (...) {
      }

      try {
        size_t rowIdx = ToSizeT(Data, Size, offset, static_cast<size_t>(i));
        (void)doc.GetRowName(rowIdx);
      } catch (...) {
      }

      try {
        std::vector<std::string> rowNames2 = doc.GetRowNames();
        if (!rowNames2.empty()) {
          size_t idx = ToSizeT(Data, Size, offset, 0) % rowNames2.size();
          doc.RemoveRow(rowNames2[idx]);
        } else {
          std::string arbitraryRowName = ToString(Data, Size, offset);
          doc.RemoveRow(arbitraryRowName);
        }
      } catch (...) {
      }
    }

    try {
      std::vector<std::string> finalCols = doc.GetColumnNames();
      for (const auto& n : finalCols) {
        try {
          (void)doc.GetColumnIdx(n);
        } catch (...) {
        }
      }
    } catch (...) {
    }

    try {
      (void)doc.GetRowNames();
    } catch (...) {
    }
  } catch (...) {
  }

  return 0;
}