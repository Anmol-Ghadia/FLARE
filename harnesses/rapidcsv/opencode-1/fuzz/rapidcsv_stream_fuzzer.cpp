#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <sstream>
#include <string>
#include <vector>

#include "rapidcsv.h"

namespace
{
  volatile std::size_t gSink = 0;

  template<typename Fn>
  void IgnoreExceptions(const Fn& pFn)
  {
    try
    {
      pFn();
    }
    catch (...)
    {
    }
  }

  template<typename T>
  void Touch(const T& pValue)
  {
    gSink ^= static_cast<std::size_t>(pValue);
  }

  void Touch(const double pValue)
  {
    gSink ^= static_cast<std::size_t>(pValue == pValue);
  }

  void Touch(const std::string& pValue)
  {
    gSink ^= pValue.size();
  }

  template<typename T>
  void Touch(const std::vector<T>& pValue)
  {
    gSink ^= pValue.size();
  }

  struct Config
  {
    Config(const rapidcsv::LabelParams& pLabelParams,
           const rapidcsv::SeparatorParams& pSeparatorParams,
           const rapidcsv::ConverterParams& pConverterParams,
           const rapidcsv::LineReaderParams& pLineReaderParams)
      : mLabelParams(pLabelParams)
      , mSeparatorParams(pSeparatorParams)
      , mConverterParams(pConverterParams)
      , mLineReaderParams(pLineReaderParams)
    {
    }

    rapidcsv::LabelParams mLabelParams;
    rapidcsv::SeparatorParams mSeparatorParams;
    rapidcsv::ConverterParams mConverterParams;
    rapidcsv::LineReaderParams mLineReaderParams;
  };

  static uint8_t GetByte(const uint8_t* pData, size_t pSize, size_t pOffset)
  {
    return (pOffset < pSize) ? pData[pOffset] : 0;
  }

  static bool GetBool(const uint8_t* pData, size_t pSize, size_t pOffset)
  {
    return (GetByte(pData, pSize, pOffset) & 1U) != 0;
  }

  template<size_t N>
  char GetChoice(const uint8_t* pData, size_t pSize, size_t pOffset, const char (&pChoices)[N])
  {
    return pChoices[GetByte(pData, pSize, pOffset) % N];
  }

  static Config GetDefaultConfig()
  {
    return Config(rapidcsv::LabelParams(),
                  rapidcsv::SeparatorParams(),
                  rapidcsv::ConverterParams(),
                  rapidcsv::LineReaderParams());
  }

  static Config GetDerivedConfig(const uint8_t* pData, size_t pSize)
  {
    static const char sSeparators[] = { ',', ';', '\t', '|', ':' };
    static const char sQuotes[] = { '"', '\'', '`' };
    static const char sCommentPrefixes[] = { '#', ';', '%', '/' };

    const char separator = GetChoice(pData, pSize, 2, sSeparators);
    char quote = GetChoice(pData, pSize, 7, sQuotes);
    if (quote == separator)
    {
      quote = (separator == '"') ? '\'' : '"';
    }

    return Config(
      rapidcsv::LabelParams(static_cast<int>(GetByte(pData, pSize, 0) % 2),
                            static_cast<int>(GetByte(pData, pSize, 1) % 2)),
      rapidcsv::SeparatorParams(separator,
                                GetBool(pData, pSize, 3),
                                GetBool(pData, pSize, 4),
                                GetBool(pData, pSize, 5),
                                GetBool(pData, pSize, 6),
                                quote),
      rapidcsv::ConverterParams(GetBool(pData, pSize, 8),
                                static_cast<long double>(static_cast<signed char>(GetByte(pData, pSize, 9))) / 4.0L,
                                static_cast<long long>(static_cast<signed char>(GetByte(pData, pSize, 10))),
                                GetBool(pData, pSize, 11)),
      rapidcsv::LineReaderParams(GetBool(pData, pSize, 12),
                                 GetChoice(pData, pSize, 13, sCommentPrefixes),
                                 GetBool(pData, pSize, 14)));
  }

  static std::string GetCellValue(const std::string& pInput, size_t pOffset)
  {
    if (pInput.empty())
    {
      return "x";
    }

    const size_t start = std::min(pOffset, pInput.size() - 1);
    return pInput.substr(start, std::min<size_t>(8, pInput.size() - start));
  }

  static std::vector<std::string> GetRowOrColumnValues(const std::string& pInput)
  {
    std::vector<std::string> values;
    values.push_back(GetCellValue(pInput, 0));
    values.push_back(GetCellValue(pInput, pInput.size() / 2));
    values.push_back(GetCellValue(pInput, pInput.empty() ? 0 : (pInput.size() - 1)));
    return values;
  }

  static void ExerciseReaders(rapidcsv::Document& pDoc)
  {
    const size_t columnCount = pDoc.GetColumnCount();
    const size_t rowCount = pDoc.GetRowCount();
    Touch(columnCount);
    Touch(rowCount);

    IgnoreExceptions([&]() { Touch(pDoc.GetColumnNames()); });
    IgnoreExceptions([&]() { Touch(pDoc.GetRowNames()); });

    if (columnCount > 0)
    {
      IgnoreExceptions([&]() { Touch(pDoc.GetColumn<std::string>(0)); });
      IgnoreExceptions([&]() { Touch(pDoc.GetColumn<int>(0)); });
      IgnoreExceptions([&]() { Touch(pDoc.GetColumn<double>(0)); });
      IgnoreExceptions([&]() {
        const std::string columnName = pDoc.GetColumnName(0);
        Touch(columnName);
        Touch(pDoc.GetColumnIdx(columnName));
        Touch(pDoc.GetColumn<std::string>(columnName));
      });
    }

    if (rowCount > 0)
    {
      IgnoreExceptions([&]() { Touch(pDoc.GetRow<std::string>(0)); });
      IgnoreExceptions([&]() { Touch(pDoc.GetRow<long long>(0)); });
      IgnoreExceptions([&]() { Touch(pDoc.GetRow<double>(0)); });
      IgnoreExceptions([&]() {
        const std::string rowName = pDoc.GetRowName(0);
        Touch(rowName);
        Touch(pDoc.GetRowIdx(rowName));
        Touch(pDoc.GetRow<std::string>(rowName));
      });
    }

    if ((columnCount > 0) && (rowCount > 0))
    {
      IgnoreExceptions([&]() { Touch(pDoc.GetCell<std::string>(0, 0)); });
      IgnoreExceptions([&]() { Touch(pDoc.GetCell<int>(0, 0)); });
      IgnoreExceptions([&]() { Touch(pDoc.GetCell<double>(0, 0)); });
      IgnoreExceptions([&]() {
        const std::string columnName = pDoc.GetColumnName(0);
        const std::string rowName = pDoc.GetRowName(0);
        Touch(pDoc.GetCell<std::string>(columnName, rowName));
      });
    }
  }

  static void ExerciseMutators(rapidcsv::Document& pDoc, const std::string& pInput)
  {
    const std::vector<std::string> values = GetRowOrColumnValues(pInput);
    const std::string cellValue = values.front();

    IgnoreExceptions([&]() { pDoc.SetCell<std::string>(0, 0, cellValue); });
    IgnoreExceptions([&]() { pDoc.SetRow<std::string>(0, values); });
    IgnoreExceptions([&]() { pDoc.SetColumn<std::string>(0, values); });
    IgnoreExceptions([&]() { pDoc.InsertRow<std::string>(pDoc.GetRowCount() / 2, values); });
    IgnoreExceptions([&]() { pDoc.InsertColumn<std::string>(pDoc.GetColumnCount() / 2, values); });
    IgnoreExceptions([&]() { pDoc.SetRowName(0, cellValue); });
    IgnoreExceptions([&]() { pDoc.SetColumnName(0, cellValue); });

    if (pDoc.GetRowCount() > 0)
    {
      IgnoreExceptions([&]() { pDoc.RemoveRow(pDoc.GetRowCount() - 1); });
    }

    if (pDoc.GetColumnCount() > 0)
    {
      IgnoreExceptions([&]() { pDoc.RemoveColumn(pDoc.GetColumnCount() - 1); });
    }
  }

  static void RunCase(const std::string& pInput, const Config& pConfig)
  {
    IgnoreExceptions([&]() {
      std::istringstream inputStream(pInput);
      rapidcsv::Document doc(inputStream,
                             pConfig.mLabelParams,
                             pConfig.mSeparatorParams,
                             pConfig.mConverterParams,
                             pConfig.mLineReaderParams);
      ExerciseReaders(doc);
      ExerciseMutators(doc, pInput);
      ExerciseReaders(doc);

      std::ostringstream outputStream;
      doc.Save(outputStream);
      const std::string serialized = outputStream.str();
      Touch(serialized);

      std::istringstream roundTripStream(serialized);
      rapidcsv::Document roundTripDoc;
      roundTripDoc.Load(roundTripStream,
                        pConfig.mLabelParams,
                        pConfig.mSeparatorParams,
                        pConfig.mConverterParams,
                        pConfig.mLineReaderParams);
      ExerciseReaders(roundTripDoc);
    });
  }
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* pData, size_t pSize)
{
  if (pData == nullptr)
  {
    return 0;
  }

  const std::string input(reinterpret_cast<const char*>(pData), pSize);
  RunCase(input, GetDefaultConfig());
  RunCase(input, GetDerivedConfig(pData, pSize));
  return 0;
}
