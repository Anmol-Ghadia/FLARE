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
#include <stdint.h>
#include <stddef.h>
#include <setjmp.h>
#include <vector>
#include <algorithm>
#include <fstream>

static uint32_t ReadU32(const uint8_t *data, size_t size, size_t &off) {
  uint32_t v = 0;
  for (int i = 0; i < 4; ++i) {
    v <<= 8;
    if (off < size) v |= data[off++];
  }
  return v;
}

static int ReadIntInRange(const uint8_t *data, size_t size, size_t &off, int minv, int maxv) {
  if (minv >= maxv) return minv;
  uint32_t v = ReadU32(data, size, off);
  return minv + static_cast<int>(v % static_cast<uint32_t>(maxv - minv + 1));
}

static png_byte ReadByte(const uint8_t *data, size_t size, size_t &off) {
  if (off < size) return data[off++];
  return 0;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  std::ofstream dummy("./dummy_file", std::ios::binary);
  if (dummy) dummy.write(reinterpret_cast<const char*>(Data), static_cast<std::streamsize>(Size));
  dummy.close();

  png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (!png_ptr) return 0;

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    png_destroy_write_struct(&png_ptr, nullptr);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    return 0;
  }

  size_t off = 0;

  png_set_check_for_invalid_index(png_ptr, ReadIntInRange(Data, Size, off, -2, 2));

  png_color grayscale_palette[16];
  int gray_depths[4] = {1, 2, 4, 8};
  int gray_depth = gray_depths[ReadIntInRange(Data, Size, off, 0, 3)];
  png_build_grayscale_palette(gray_depth, grayscale_palette);

  std::vector<png_color> palette(PNG_MAX_PALETTE_LENGTH);
  for (int i = 0; i < PNG_MAX_PALETTE_LENGTH; ++i) {
    if (i < 16 && (ReadByte(Data, Size, off) & 1)) {
      palette[i] = grayscale_palette[i % 16];
    } else {
      palette[i].red = ReadByte(Data, Size, off);
      palette[i].green = ReadByte(Data, Size, off);
      palette[i].blue = ReadByte(Data, Size, off);
    }
  }

  int bit_depth_choices[4] = {1, 2, 4, 8};
  int bit_depth = bit_depth_choices[ReadIntInRange(Data, Size, off, 0, 3)];
  int color_type = (ReadByte(Data, Size, off) & 1) ? PNG_COLOR_TYPE_PALETTE : PNG_COLOR_TYPE_RGB;
  int max_legal_palette = (color_type == PNG_COLOR_TYPE_PALETTE)
                              ? std::min<int>(PNG_MAX_PALETTE_LENGTH, 1 << bit_depth)
                              : PNG_MAX_PALETTE_LENGTH;
  int num_palette = ReadIntInRange(Data, Size, off, 1, std::max(1, max_legal_palette));

  png_set_IHDR(png_ptr, info_ptr,
               1, 1,
               bit_depth,
               color_type,
               PNG_INTERLACE_NONE,
               PNG_COMPRESSION_TYPE_BASE,
               PNG_FILTER_TYPE_BASE);

  png_set_PLTE(png_ptr, info_ptr, palette.data(), num_palette);

  png_colorp got_palette = nullptr;
  int got_num_palette = 0;
  (void)png_get_PLTE(png_ptr, info_ptr, &got_palette, &got_num_palette);
  (void)png_get_palette_max(png_ptr, info_ptr);

  std::vector<png_uint_16> histogram(static_cast<size_t>(std::max(got_num_palette, num_palette)));
  for (size_t i = 0; i < histogram.size(); ++i) {
    histogram[i] = static_cast<png_uint_16>(ReadU32(Data, Size, off) & 0xFFFFu);
  }

  for (int iter = 0; iter < 3; ++iter) {
    png_colorp qpalette = got_palette ? got_palette : palette.data();
    int qnum_palette = got_palette ? got_num_palette : num_palette;
    if (qnum_palette <= 0) qnum_palette = num_palette;
    if (qnum_palette <= 0) qnum_palette = 1;
    if (qnum_palette > PNG_MAX_PALETTE_LENGTH) qnum_palette = PNG_MAX_PALETTE_LENGTH;

    int maximum_colors = ReadIntInRange(Data, Size, off, 1, qnum_palette);
    int full_quantize = ReadIntInRange(Data, Size, off, 0, 1);
    png_const_uint_16p hist_ptr = (ReadByte(Data, Size, off) & 1) ? histogram.data() : nullptr;

    png_set_quantize(png_ptr, qpalette, qnum_palette, maximum_colors, hist_ptr, full_quantize);
    png_set_check_for_invalid_index(png_ptr, ReadIntInRange(Data, Size, off, -1, 1));
    (void)png_get_palette_max(png_ptr, info_ptr);
    (void)png_get_PLTE(png_ptr, info_ptr, &got_palette, &got_num_palette);
  }

  png_destroy_write_struct(&png_ptr, &info_ptr);
  return 0;
}