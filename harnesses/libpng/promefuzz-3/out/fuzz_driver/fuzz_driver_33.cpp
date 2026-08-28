// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_PLTE at pngset.c:572:1 in png.h
// png_set_PLTE at pngset.c:572:1 in png.h
// png_get_PLTE at pngget.c:1012:1 in png.h
// png_set_hIST at pngset.c:210:1 in png.h
// png_set_hIST at pngset.c:210:1 in png.h
// png_destroy_info_struct at png.c:386:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_info_struct at png.c:386:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_destroy_info_struct at png.c:386:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_read_fn at pngrio.c:87:1 in png.h
// png_set_sig_bytes at png.c:47:1 in png.h
// png_read_info at pngread.c:92:1 in png.h
// png_get_PLTE at pngget.c:1012:1 in png.h
// png_get_PLTE at pngget.c:1012:1 in png.h
// png_get_PLTE at pngget.c:1012:1 in png.h
// png_get_PLTE at pngget.c:1012:1 in png.h
// png_get_PLTE at pngget.c:1012:1 in png.h
// png_get_palette_max at pngget.c:1239:1 in png.h
// png_get_palette_max at pngget.c:1239:1 in png.h
// png_get_palette_max at pngget.c:1239:1 in png.h
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

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <vector>

static void PngReadCallback(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead) {
  png_voidp io_ptr = png_get_io_ptr(png_ptr);
  if (io_ptr == nullptr) {
    png_error(png_ptr, "no io_ptr");
    return;
  }

  FILE* fp = static_cast<FILE*>(io_ptr);
  if (byteCountToRead == 0) return;

  size_t n = fread(outBytes, 1, byteCountToRead, fp);
  if (n != byteCountToRead) {
    png_error(png_ptr, "read error");
  }
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE* fp = std::fopen("./dummy_file", "wb");
  if (fp == nullptr) {
    return 0;
  }

  if (Size > 0) {
    (void)std::fwrite(Data, 1, Size, fp);
  }
  std::fclose(fp);

  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (png_ptr == nullptr) {
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr) {
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_info_struct(png_ptr, &info_ptr);
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    return 0;
  }

  fp = std::fopen("./dummy_file", "rb");
  if (fp == nullptr) {
    png_destroy_info_struct(png_ptr, &info_ptr);
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    return 0;
  }

  png_set_read_fn(png_ptr, fp, PngReadCallback);

  if (Size >= 8) {
    png_set_sig_bytes(png_ptr, 0);
  }

  png_read_info(png_ptr, info_ptr);

  png_colorp got_palette = nullptr;
  int got_num_palette = -1;
  (void)png_get_PLTE(png_ptr, info_ptr, &got_palette, &got_num_palette);
  (void)png_get_PLTE(png_ptr, info_ptr, nullptr, &got_num_palette);
  (void)png_get_PLTE(png_ptr, info_ptr, &got_palette, nullptr);
  (void)png_get_PLTE(nullptr, info_ptr, &got_palette, &got_num_palette);
  (void)png_get_PLTE(png_ptr, nullptr, &got_palette, &got_num_palette);

  (void)png_get_palette_max(png_ptr, info_ptr);
  (void)png_get_palette_max(nullptr, info_ptr);
  (void)png_get_palette_max(png_ptr, nullptr);

  std::vector<png_color> palette;
  int palette_entries = 0;
  if (Size > 0) {
    palette_entries = static_cast<int>(Data[0] % 32);
  }

  palette.resize(static_cast<size_t>(palette_entries));
  for (int i = 0; i < palette_entries; ++i) {
    size_t base = 1 + static_cast<size_t>(i) * 3;
    png_color c;
    c.red = (base + 0 < Size) ? Data[base + 0] : 0;
    c.green = (base + 1 < Size) ? Data[base + 1] : 0;
    c.blue = (base + 2 < Size) ? Data[base + 2] : 0;
    palette[static_cast<size_t>(i)] = c;
  }

  png_set_PLTE(png_ptr, info_ptr, palette.empty() ? nullptr : palette.data(), palette_entries);
  png_set_PLTE(png_ptr, info_ptr, palette.empty() ? nullptr : palette.data(), 0);

  png_colorp after_set_palette = nullptr;
  int after_set_num_palette = -1;
  (void)png_get_PLTE(png_ptr, info_ptr, &after_set_palette, &after_set_num_palette);

  int hist_count = after_set_num_palette;
  if (hist_count < 0) hist_count = 0;

  std::vector<png_uint_16> hist(static_cast<size_t>(hist_count));
  for (int i = 0; i < hist_count; ++i) {
    size_t idx = 1 + static_cast<size_t>(palette_entries) * 3 + static_cast<size_t>(i) * 2;
    png_uint_16 v = 0;
    if (idx < Size) v = static_cast<png_uint_16>(Data[idx]);
    if (idx + 1 < Size) v = static_cast<png_uint_16>((v << 8) | Data[idx + 1]);
    hist[static_cast<size_t>(i)] = v;
  }

  png_set_hIST(png_ptr, info_ptr, hist.empty() ? nullptr : hist.data());
  if (!hist.empty()) {
    png_set_hIST(png_ptr, info_ptr, hist.data());
  }

  std::fclose(fp);
  png_destroy_info_struct(png_ptr, &info_ptr);
  png_destroy_read_struct(&png_ptr, nullptr, nullptr);
  return 0;
}