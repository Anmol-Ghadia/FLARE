// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_IHDR at pngset.c:254:1 in png.h
// png_set_PLTE at pngset.c:572:1 in png.h
// png_set_text at pngset.c:762:1 in png.h
// png_get_text at pngget.c:1049:1 in png.h
// png_get_text at pngget.c:1049:1 in png.h
// png_get_text at pngget.c:1049:1 in png.h
// png_set_eXIf_1 at pngset.c:148:1 in png.h
// png_write_info at pngwrite.c:192:1 in png.h
// png_write_row at pngwrite.c:693:1 in png.h
// png_write_end at pngwrite.c:358:1 in png.h
// png_free_data at png.c:471:1 in png.h
// png_free_data at png.c:471:1 in png.h
// png_destroy_info_struct at png.c:386:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_info_struct at png.c:386:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_init_io at png.c:703:1 in png.h
// png_destroy_info_struct at png.c:386:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_init_io at png.c:703:1 in png.h
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <cstddef>
#include <png.h>

#include <cstdint>
#include <cstddef>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <setjmp.h>
#include <cstring>

static uint32_t ReadU32(const uint8_t *data, size_t size, size_t &off) {
  uint32_t v = 0;
  for (int i = 0; i < 4; ++i) {
    v <<= 8;
    if (off < size) v |= data[off++];
  }
  return v;
}

static int ReadInt(const uint8_t *data, size_t size, size_t &off) {
  return static_cast<int>(ReadU32(data, size, off));
}

static std::string ReadString(const uint8_t *data, size_t size, size_t &off, size_t max_len) {
  if (off >= size) return std::string();
  size_t remaining = size - off;
  size_t len = remaining ? (data[off++] % (std::min(max_len, remaining - (off <= size ? 0 : 0)) + 1)) : 0;
  len = std::min(len, size - off);
  std::string s(reinterpret_cast<const char*>(data + off), len);
  off += len;
  return s;
}

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput_73(const uint8_t *Data, size_t Size) {
  if (Data == nullptr) return 0;

  {
    std::ofstream ofs("./dummy_file", std::ios::binary);
    if (ofs) ofs.write(reinterpret_cast<const char*>(Data), static_cast<std::streamsize>(Size));
  }

  png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn, PngWarningFn);
  if (!png_ptr) return 0;

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    png_destroy_write_struct(&png_ptr, nullptr);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    if (info_ptr != nullptr) {
      png_destroy_info_struct(png_ptr, &info_ptr);
    }
    png_destroy_write_struct(&png_ptr, nullptr);
    return 0;
  }

  png_init_io(png_ptr, fopen("./dummy_file", "wb"));
  FILE *fp = fopen("./dummy_file", "wb");
  if (fp == nullptr) {
    png_destroy_info_struct(png_ptr, &info_ptr);
    png_destroy_write_struct(&png_ptr, nullptr);
    return 0;
  }
  png_init_io(png_ptr, fp);

  size_t off = 0;

  png_uint_32 width = (Size > 0) ? ((ReadU32(Data, Size, off) % 32) + 1) : 1;
  png_uint_32 height = (Size > 0) ? ((ReadU32(Data, Size, off) % 32) + 1) : 1;
  int color_type_choices[] = {
      PNG_COLOR_TYPE_GRAY,
      PNG_COLOR_TYPE_RGB,
      PNG_COLOR_TYPE_PALETTE,
      PNG_COLOR_TYPE_GRAY_ALPHA,
      PNG_COLOR_TYPE_RGB_ALPHA
  };
  int bit_depth_choices[] = {1, 2, 4, 8, 16};
  int color_type = color_type_choices[(Size > 0 ? Data[off % Size] : 0) % 5];
  if (off < Size) ++off;
  int bit_depth = bit_depth_choices[(Size > 0 ? Data[off % Size] : 0) % 5];
  if (off < Size) ++off;

  if ((color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_RGB_ALPHA ||
       color_type == PNG_COLOR_TYPE_GRAY_ALPHA) &&
      bit_depth < 8) {
    bit_depth = 8;
  }

  png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, color_type,
               PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

  png_color palette[256];
  if (color_type == PNG_COLOR_TYPE_PALETTE) {
    for (int i = 0; i < 256; ++i) {
      palette[i].red = static_cast<png_byte>((i * 3) & 0xff);
      palette[i].green = static_cast<png_byte>((i * 5) & 0xff);
      palette[i].blue = static_cast<png_byte>((i * 7) & 0xff);
    }
    png_set_PLTE(png_ptr, info_ptr, palette, 256);
  }

  std::vector<png_text> texts;
  int num_entries = Size > 0 ? static_cast<int>(Data[off % Size] % 4) : 0;
  if (off < Size) ++off;

  std::vector<std::string> keys;
  std::vector<std::string> values;
  std::vector<std::string> langs;
  std::vector<std::string> lang_keys;

  texts.resize(num_entries);
  keys.reserve(num_entries);
  values.reserve(num_entries);
  langs.reserve(num_entries);
  lang_keys.reserve(num_entries);

  for (int i = 0; i < num_entries; ++i) {
    int compression_modes[] = {
        PNG_TEXT_COMPRESSION_NONE,
        PNG_TEXT_COMPRESSION_zTXt,
#ifdef PNG_iTXt_SUPPORTED
        PNG_ITXT_COMPRESSION_NONE,
        PNG_ITXT_COMPRESSION_zTXt
#else
        PNG_TEXT_COMPRESSION_NONE,
        PNG_TEXT_COMPRESSION_zTXt
#endif
    };
    int mode_count = 4;

    std::string key = ReadString(Data, Size, off, 16);
    if (key.empty()) key = "K";
    key.erase(std::remove(key.begin(), key.end(), '\0'), key.end());
    if (key.empty()) key = "K";

    std::string value = ReadString(Data, Size, off, 64);
    std::string lang = ReadString(Data, Size, off, 8);
    std::string lang_key = ReadString(Data, Size, off, 8);

    keys.push_back(key);
    values.push_back(value);
    langs.push_back(lang);
    lang_keys.push_back(lang_key);

    std::memset(&texts[i], 0, sizeof(texts[i]));
    texts[i].compression = compression_modes[(Size > 0 ? Data[off % Size] : 0) % mode_count];
    if (off < Size) ++off;
    texts[i].key = const_cast<png_charp>(keys.back().c_str());
    texts[i].text = const_cast<png_charp>(values.back().c_str());
    texts[i].text_length = values.back().size();
#ifdef PNG_iTXt_SUPPORTED
    texts[i].lang = const_cast<png_charp>(langs.back().c_str());
    texts[i].lang_key = const_cast<png_charp>(lang_keys.back().c_str());
    texts[i].itxt_length = values.back().size();
#endif
  }

  if (!texts.empty()) {
    png_set_text(png_ptr, info_ptr, texts.data(), static_cast<int>(texts.size()));
  }

  png_textp out_text = nullptr;
  int out_num_text = -1;
  png_get_text(png_ptr, info_ptr, &out_text, &out_num_text);
  png_get_text(png_ptr, info_ptr, nullptr, &out_num_text);
  png_get_text(png_ptr, info_ptr, &out_text, nullptr);

  png_uint_32 exif_len = Size > off ? static_cast<png_uint_32>(std::min<size_t>(Size - off, 256)) : 0;
  std::vector<png_byte> exif(exif_len);
  for (png_uint_32 i = 0; i < exif_len; ++i) exif[i] = Data[off + i];
  png_set_eXIf_1(png_ptr, info_ptr, exif_len, exif.empty() ? nullptr : exif.data());

  png_write_info(png_ptr, info_ptr);

  int channels = 1;
  switch (color_type) {
    case PNG_COLOR_TYPE_GRAY: channels = 1; break;
    case PNG_COLOR_TYPE_PALETTE: channels = 1; break;
    case PNG_COLOR_TYPE_RGB: channels = 3; break;
    case PNG_COLOR_TYPE_GRAY_ALPHA: channels = 2; break;
    case PNG_COLOR_TYPE_RGB_ALPHA: channels = 4; break;
    default: channels = 1; break;
  }

  size_t rowbytes = (static_cast<size_t>(width) * channels * static_cast<size_t>(bit_depth) + 7) / 8;
  if (rowbytes == 0) rowbytes = 1;
  std::vector<png_byte> row(rowbytes);

  bool write_rows = (Size == 0) ? false : ((Data[0] & 1) != 0);
  if (write_rows) {
    for (png_uint_32 y = 0; y < height; ++y) {
      for (size_t i = 0; i < rowbytes; ++i) {
        row[i] = (off + i < Size) ? Data[(off + i) % Size] : static_cast<uint8_t>(i + y);
      }
      png_write_row(png_ptr, row.data());
    }
  }

  png_write_end(png_ptr, info_ptr);

  png_uint_32 masks[] = {
      PNG_FREE_TEXT,
#ifdef PNG_FREE_EXIF
      PNG_FREE_EXIF,
#endif
      PNG_FREE_ALL
  };

  for (size_t i = 0; i < sizeof(masks) / sizeof(masks[0]); ++i) {
    png_free_data(png_ptr, info_ptr, masks[i], -1);
    png_free_data(png_ptr, info_ptr, masks[i], 0);
  }

  png_destroy_info_struct(png_ptr, &info_ptr);
  fclose(fp);
  png_destroy_write_struct(&png_ptr, nullptr);
  return 0;
}