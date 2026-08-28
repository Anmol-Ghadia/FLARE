// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_PLTE at pngset.c:572:1 in png.h
// png_set_tRNS at pngset.c:994:1 in png.h
// png_set_tRNS at pngset.c:994:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_set_tRNS at pngset.c:994:1 in png.h
// png_set_tRNS at pngset.c:994:1 in png.h
// png_set_tRNS at pngset.c:994:1 in png.h
// png_set_bKGD at pngset.c:25:1 in png.h
// png_get_tRNS at pngget.c:1093:1 in png.h
// png_get_valid at pngget.c:20:1 in png.h
// png_get_valid at pngget.c:20:1 in png.h
// png_get_valid at pngget.c:20:1 in png.h
// png_get_valid at pngget.c:20:1 in png.h
// png_write_info at pngwrite.c:192:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_write_fn at pngwio.c:122:1 in png.h
// png_set_IHDR at pngset.c:254:1 in png.h
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

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

static png_uint_32 ReadU32(const uint8_t *data, size_t size, size_t &off) {
  png_uint_32 v = 0;
  for (int i = 0; i < 4; ++i) {
    v <<= 8;
    if (off < size) v |= data[off++];
  }
  return v;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE *fp = std::fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    if (Size > 0) std::fwrite(Data, 1, Size, fp);
    std::fclose(fp);
  }

  png_structp png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn, PngWarningFn);
  if (png_ptr == nullptr) return 0;

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr) {
    png_destroy_write_struct(&png_ptr, nullptr);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    return 0;
  }

  png_set_write_fn(
      png_ptr, nullptr,
      [](png_structp, png_bytep, png_size_t) {},
      [](png_structp) {});

  size_t off = 0;

  int color_type_choices[] = {
      PNG_COLOR_TYPE_GRAY,
      PNG_COLOR_TYPE_RGB,
      PNG_COLOR_TYPE_PALETTE,
      PNG_COLOR_TYPE_GRAY_ALPHA,
      PNG_COLOR_TYPE_RGB_ALPHA};
  int bit_depth_choices[] = {1, 2, 4, 8, 16};

  int color_type = color_type_choices[(off < Size ? Data[off++] : 0) % 5];
  int bit_depth = bit_depth_choices[(off < Size ? Data[off++] : 0) % 5];

  if ((color_type == PNG_COLOR_TYPE_RGB ||
       color_type == PNG_COLOR_TYPE_RGB_ALPHA ||
       color_type == PNG_COLOR_TYPE_GRAY_ALPHA) &&
      bit_depth < 8) {
    bit_depth = 8;
  }

  if (color_type == PNG_COLOR_TYPE_PALETTE && bit_depth == 16) {
    bit_depth = 8;
  }

  png_uint_32 width = (ReadU32(Data, Size, off) % 16) + 1;
  png_uint_32 height = (ReadU32(Data, Size, off) % 16) + 1;
  int interlace = (off < Size ? (Data[off++] & 1) : 0) ? PNG_INTERLACE_ADAM7 : PNG_INTERLACE_NONE;
  int compression = PNG_COMPRESSION_TYPE_BASE;
  int filter = PNG_FILTER_TYPE_BASE;

  png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, color_type,
               interlace, compression, filter);

  png_color palette[PNG_MAX_PALETTE_LENGTH];
  for (int i = 0; i < PNG_MAX_PALETTE_LENGTH; ++i) {
    palette[i].red = (off < Size ? Data[off++] : static_cast<uint8_t>(i));
    palette[i].green = (off < Size ? Data[off++] : static_cast<uint8_t>(i * 3));
    palette[i].blue = (off < Size ? Data[off++] : static_cast<uint8_t>(i * 7));
  }

  int max_palette = PNG_MAX_PALETTE_LENGTH;
  if (color_type == PNG_COLOR_TYPE_PALETTE)
    max_palette = 1 << bit_depth;

  int num_palette = 0;
  if (color_type == PNG_COLOR_TYPE_PALETTE) {
    num_palette = static_cast<int>((off < Size ? Data[off++] : 0) % max_palette) + 1;
    if ((off < Size ? Data[off++] : 0) & 1) {
      num_palette = max_palette;
    } else {
      (void)(off < Size ? Data[off++] : 0);
    }
    png_set_PLTE(png_ptr, info_ptr, palette, num_palette);
  } else {
    if (off < Size) ++off;
    if (off < Size) ++off;
    if (off < Size) ++off;
  }

  png_byte trans_alpha[PNG_MAX_PALETTE_LENGTH];
  for (int i = 0; i < PNG_MAX_PALETTE_LENGTH; ++i) {
    trans_alpha[i] = (off < Size ? Data[off++] : static_cast<uint8_t>(255 - i));
  }

  png_color_16 trans_color;
  std::memset(&trans_color, 0, sizeof(trans_color));
  trans_color.index = (off < Size ? Data[off++] : 0);
  trans_color.red = static_cast<png_uint_16>(ReadU32(Data, Size, off) & 0xFFFFu);
  trans_color.green = static_cast<png_uint_16>(ReadU32(Data, Size, off) & 0xFFFFu);
  trans_color.blue = static_cast<png_uint_16>(ReadU32(Data, Size, off) & 0xFFFFu);
  trans_color.gray = static_cast<png_uint_16>(ReadU32(Data, Size, off) & 0xFFFFu);

  int num_trans = static_cast<int>((off < Size ? Data[off++] : 0) % (PNG_MAX_PALETTE_LENGTH + 1));
  int trns_mode = (off < Size ? Data[off++] : 0) % 4;

  if (color_type == PNG_COLOR_TYPE_PALETTE) {
    if (num_palette > 0 && num_trans > num_palette) num_trans = num_palette;
    switch (trns_mode) {
      case 0:
        png_set_tRNS(png_ptr, info_ptr, trans_alpha, num_trans, &trans_color);
        break;
      case 1:
        png_set_tRNS(png_ptr, info_ptr, trans_alpha, num_trans, nullptr);
        break;
      case 2:
        png_set_tRNS(png_ptr, info_ptr, nullptr, 0, &trans_color);
        break;
      default:
        png_set_tRNS(png_ptr, info_ptr, nullptr, 0, nullptr);
        break;
    }
  } else if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_RGB) {
    png_set_tRNS(png_ptr, info_ptr, nullptr, 0, &trans_color);
  }

  png_color_16 background;
  std::memset(&background, 0, sizeof(background));
  background.index = (off < Size ? Data[off++] : 0);
  background.red = static_cast<png_uint_16>(ReadU32(Data, Size, off) & 0xFFFFu);
  background.green = static_cast<png_uint_16>(ReadU32(Data, Size, off) & 0xFFFFu);
  background.blue = static_cast<png_uint_16>(ReadU32(Data, Size, off) & 0xFFFFu);
  background.gray = static_cast<png_uint_16>(ReadU32(Data, Size, off) & 0xFFFFu);
  png_set_bKGD(png_ptr, info_ptr, &background);

  png_bytep got_alpha = nullptr;
  int got_num_trans = -1;
  png_color_16p got_trans_color = nullptr;
  (void)png_get_tRNS(png_ptr, info_ptr, &got_alpha, &got_num_trans, &got_trans_color);

  (void)png_get_valid(png_ptr, info_ptr, PNG_INFO_PLTE);
  (void)png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS);
  (void)png_get_valid(png_ptr, info_ptr, PNG_INFO_bKGD);
  (void)png_get_valid(png_ptr, info_ptr,
                      PNG_INFO_PLTE | PNG_INFO_tRNS | PNG_INFO_bKGD);

  png_write_info(png_ptr, info_ptr);

  png_destroy_write_struct(&png_ptr, &info_ptr);
  return 0;
}