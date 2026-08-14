// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_sRGB at pngset.c:653:1 in png.h
// png_set_sRGB at pngset.c:653:1 in png.h
// png_set_sRGB at pngset.c:653:1 in png.h
// png_set_tIME at pngset.c:968:1 in png.h
// png_set_tIME at pngset.c:968:1 in png.h
// png_set_tIME at pngset.c:968:1 in png.h
// png_set_iCCP at pngset.c:688:1 in png.h
// png_set_iCCP at pngset.c:688:1 in png.h
// png_set_iCCP at pngset.c:688:1 in png.h
// png_set_rows at pngset.c:1546:1 in png.h
// png_write_info at pngwrite.c:192:1 in png.h
// png_write_row at pngwrite.c:693:1 in png.h
// png_write_end at pngwrite.c:358:1 in png.h
// png_write_png at pngwrite.c:1338:1 in png.h
// png_write_info at pngwrite.c:192:1 in png.h
// png_write_row at pngwrite.c:693:1 in png.h
// png_write_end at pngwrite.c:358:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_init_io at png.c:703:1 in png.h
// png_set_IHDR at pngset.c:254:1 in png.h
// png_set_PLTE at pngset.c:572:1 in png.h
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
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

static uint32_t ReadU32(const uint8_t *data, size_t size, size_t &off) {
  uint32_t v = 0;
  for (int i = 0; i < 4; ++i) {
    v <<= 8;
    if (off < size) v |= data[off++];
  }
  return v;
}

static int ReadByte(const uint8_t *data, size_t size, size_t &off) {
  if (off < size) return data[off++];
  return 0;
}

extern "C" int LLVMFuzzerTestOneInput_26(const uint8_t *Data, size_t Size) {
  FILE *fp = std::fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    if (Size > 0) std::fwrite(Data, 1, Size, fp);
    std::fclose(fp);
  }

  size_t off = 0;

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

  fp = std::fopen("./dummy_file", "wb");
  if (fp == nullptr) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    return 0;
  }

  png_init_io(png_ptr, fp);

  png_uint_32 width = (ReadU32(Data, Size, off) % 32) + 1;
  png_uint_32 height = (ReadU32(Data, Size, off) % 32) + 1;

  int color_choice = ReadByte(Data, Size, off) % 4;
  int color_type = PNG_COLOR_TYPE_RGB;
  switch (color_choice) {
    case 0: color_type = PNG_COLOR_TYPE_GRAY; break;
    case 1: color_type = PNG_COLOR_TYPE_RGB; break;
    case 2: color_type = PNG_COLOR_TYPE_RGBA; break;
    case 3: color_type = PNG_COLOR_TYPE_PALETTE; break;
  }

  int bit_depth = (ReadByte(Data, Size, off) & 1) ? 8 : 1;
  if (color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_RGBA)
    bit_depth = 8;

  png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, color_type,
               PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

  png_color palette[256];
  if (color_type == PNG_COLOR_TYPE_PALETTE) {
    int palette_entries = (ReadByte(Data, Size, off) % 256) + 1;
    for (int i = 0; i < palette_entries; ++i) {
      palette[i].red = static_cast<png_byte>(ReadByte(Data, Size, off));
      palette[i].green = static_cast<png_byte>(ReadByte(Data, Size, off));
      palette[i].blue = static_cast<png_byte>(ReadByte(Data, Size, off));
    }
    png_set_PLTE(png_ptr, info_ptr, palette, palette_entries);
  }

  int use_srgb = ReadByte(Data, Size, off) & 1;
  if (use_srgb) {
    int intent = ReadByte(Data, Size, off) % 4;
    png_set_sRGB(png_ptr, info_ptr, intent);
  } else {
    png_set_sRGB(nullptr, info_ptr, 0);
    png_set_sRGB(png_ptr, nullptr, 0);
  }

  png_time mod_time;
  mod_time.year = static_cast<png_uint_16>(ReadU32(Data, Size, off) % 10000);
  mod_time.month = static_cast<png_byte>(ReadByte(Data, Size, off));
  mod_time.day = static_cast<png_byte>(ReadByte(Data, Size, off));
  mod_time.hour = static_cast<png_byte>(ReadByte(Data, Size, off));
  mod_time.minute = static_cast<png_byte>(ReadByte(Data, Size, off));
  mod_time.second = static_cast<png_byte>(ReadByte(Data, Size, off));
  png_set_tIME(png_ptr, info_ptr, &mod_time);
  png_set_tIME(nullptr, info_ptr, &mod_time);
  png_set_tIME(png_ptr, nullptr, &mod_time);

  int use_iccp = ReadByte(Data, Size, off) & 1;
  if (use_iccp) {
    const char *name = "icc";
    png_uint_32 profile_len = static_cast<png_uint_32>(Size > off ? Size - off : 0);
    const png_byte *profile = profile_len ? Data + off : reinterpret_cast<const png_byte*>("");
    int compression_type = (ReadByte(Data, Size, off) & 1) ? PNG_COMPRESSION_TYPE_BASE : 99;
    png_set_iCCP(png_ptr, info_ptr, name, compression_type, profile, profile_len);
  } else {
    png_set_iCCP(nullptr, info_ptr, "icc", PNG_COMPRESSION_TYPE_BASE,
                 reinterpret_cast<const png_byte*>("x"), 1);
    png_set_iCCP(png_ptr, nullptr, "icc", PNG_COMPRESSION_TYPE_BASE,
                 reinterpret_cast<const png_byte*>("x"), 1);
  }

  int channels = 1;
  switch (color_type) {
    case PNG_COLOR_TYPE_GRAY: channels = 1; break;
    case PNG_COLOR_TYPE_RGB: channels = 3; break;
    case PNG_COLOR_TYPE_PALETTE: channels = 1; break;
    case PNG_COLOR_TYPE_RGBA: channels = 4; break;
    default: channels = 1; break;
  }

  size_t rowbytes;
  if (color_type == PNG_COLOR_TYPE_PALETTE || color_type == PNG_COLOR_TYPE_GRAY) {
    rowbytes = (static_cast<size_t>(width) * bit_depth + 7) / 8;
  } else {
    rowbytes = static_cast<size_t>(width) * channels * (bit_depth / 8);
  }
  if (rowbytes == 0) rowbytes = 1;

  std::vector<png_byte> image(rowbytes * height);
  for (size_t i = 0; i < image.size(); ++i) {
    image[i] = static_cast<png_byte>(ReadByte(Data, Size, off));
  }

  std::vector<png_bytep> rows(height);
  for (png_uint_32 y = 0; y < height; ++y) {
    rows[y] = image.data() + y * rowbytes;
  }
  png_set_rows(png_ptr, info_ptr, rows.data());

  int mode = ReadByte(Data, Size, off) % 3;
  if (mode == 0) {
    png_write_info(png_ptr, info_ptr);
    for (png_uint_32 y = 0; y < height; ++y) {
      png_write_row(png_ptr, rows[y]);
    }
    png_write_end(png_ptr, info_ptr);
  } else if (mode == 1) {
    int transforms = static_cast<int>(ReadU32(Data, Size, off));
    png_write_png(png_ptr, info_ptr, transforms, nullptr);
  } else {
    png_write_info(png_ptr, info_ptr);
    if (height > 0) {
      png_write_row(png_ptr, rows[0]);
    }
    png_write_end(png_ptr, info_ptr);
  }

  std::fclose(fp);
  png_destroy_write_struct(&png_ptr, &info_ptr);
  return 0;
}