// This fuzz driver is generated for library rapidcsv, aiming to fuzz the following functions:
// rapidcsv::Document::RemoveRow at rapidcsv.h:1076:10 in rapidcsv.h
// rapidcsv::Document::RemoveRow at rapidcsv.h:1097:10 in rapidcsv.h
// rapidcsv::Document::RemoveRow at rapidcsv.h:1076:10 in rapidcsv.h
// rapidcsv::Document::Save at rapidcsv.h:597:10 in rapidcsv.h
// rapidcsv::Document::Save at rapidcsv.h:597:10 in rapidcsv.h
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
#include "rapidcsv.h"

static std::string MakeString(const uint8_t* data, size_t size)
{
    return std::string(reinterpret_cast<const char*>(data), size);
}

extern "C" int LLVMFuzzerTestOneInput_10(const uint8_t *Data, size_t Size)
{
    const std::string input = MakeString(Data, Size);

    {
        std::ofstream ofs("./dummy_file", std::ios::binary);
        ofs.write(input.data(), static_cast<std::streamsize>(input.size()));
    }

    rapidcsv::LabelParams labelParams;
    rapidcsv::SeparatorParams separatorParams;
    rapidcsv::ConverterParams converterParams;
    rapidcsv::LineReaderParams lineReaderParams;

    if (Size > 0) {
        labelParams.mColumnNameIdx = static_cast<int>(Data[0] % 4) - 1;
    } else {
        labelParams.mColumnNameIdx = -1;
    }

    separatorParams.mSeparator = (Size > 1 && Data[1] != '\0') ? static_cast<char>(Data[1]) : ',';
    separatorParams.mTrim = (Size > 2) ? ((Data[2] & 1) != 0) : false;

    converterParams.mHasDefaultConverter = (Size > 3) ? ((Data[3] & 1) != 0) : false;
    converterParams.mDefaultFloat = (Size > 4) ? static_cast<long double>(Data[4]) : 0.0L;
    converterParams.mDefaultInteger = (Size > 5) ? static_cast<long long>(Data[5]) : 0LL;

    lineReaderParams.mSkipCommentLines = (Size > 6) ? ((Data[6] & 1) != 0) : false;
    lineReaderParams.mCommentPrefix = (Size > 7 && Data[7] != '\0') ? static_cast<char>(Data[7]) : '#';

    try {
        rapidcsv::Document doc("./dummy_file",
                               labelParams,
                               separatorParams,
                               converterParams,
                               lineReaderParams);

        size_t idx1 = 0;
        size_t idx2 = 0;
        if (Size > 8) {
            idx1 = static_cast<size_t>(Data[8]);
        }
        if (Size > 9) {
            idx2 = static_cast<size_t>(Data[9]);
        }

        std::string rowName;
        if (Size > 10) {
            rowName.assign(reinterpret_cast<const char*>(Data + 10), Size - 10);
        }

        try {
            doc.RemoveRow(idx1);
        } catch (...) {
        }

        try {
            doc.RemoveRow(rowName);
        } catch (...) {
        }

        try {
            doc.RemoveRow(idx2);
        } catch (...) {
        }

        try {
            if (Size > 0 && (Data[0] & 1)) {
                doc.Save("./dummy_file");
            } else {
                doc.Save();
            }
        } catch (...) {
        }
    } catch (...) {
    }

    return 0;
}