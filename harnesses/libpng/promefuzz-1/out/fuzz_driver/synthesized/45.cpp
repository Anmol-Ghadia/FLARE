// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_IHDR at pngset.c:254:1 in png.h
// png_set_PLTE at pngset.c:572:1 in png.h
// png_set_tIME at pngset.c:968:1 in png.h
// png_set_gAMA at pngset.c:200:1 in png.h
// png_set_cHRM at pngset.c:102:1 in png.h
// png_set_cHRM_XYZ at pngset.c:118:1 in png.h
// png_set_cHRM_fixed at pngset.c:40:1 in png.h
// png_set_cHRM_XYZ_fixed at pngset.c:69:1 in png.h
// png_write_info at pngwrite.c:192:1 in png.h
// png_write_row at pngwrite.c:693:1 in png.h
// png_set_tIME at pngset.c:968:1 in png.h
// png_write_end at pngwrite.c:358:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_write_fn at pngwio.c:122:1 in png.h
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
#include <limits>

static uint32_t ReadU32(const uint8_t *data, size_t size, size_t &off) {
  if (off + 4 > size) return 0;
  uint32_t v = (static_cast<uint32_t>(data[off]) << 24) |
               (static_cast<uint32_t>(data[off + 1]) << 16) |
               (static_cast<uint32_t>(data[off + 2]) << 8) |
               static_cast<uint32_t>(data[off + 3]);
  off += 4;
  return v;
}

static int32_t ReadS32(const uint8_t *data, size_t size, size_t &off) {
  return static_cast<int32_t>(ReadU32(data, size, off));
}

static uint16_t ReadU16(const uint8_t *data, size_t size, size_t &off) {
  if (off + 2 > size) return 0;
  uint16_t v = static_cast<uint16_t>((data[off] << 8) | data[off + 1]);
  off += 2;
  return v;
}

static uint8_t ReadU8(const uint8_t *data, size_t size, size_t &off) {
  if (off >= size) return 0;
  return data[off++];
}

static double ReadDoubleLike(const uint8_t *data, size_t size, size_t &off) {
  int32_t raw = ReadS32(data, size, off);
  return static_cast<double>(raw) / 100000.0;
}

static png_fixed_point ReadFixed(const uint8_t *data, size_t size, size_t &off) {
  return static_cast<png_fixed_point>(ReadS32(data, size, off));
}

static void WriteCallback(png_structp png_ptr, png_bytep data, png_size_t length) {
  FILE *fp = static_cast<FILE*>(png_get_io_ptr(png_ptr));
  if (fp == nullptr) return;
  (void)fwrite(data, 1, length, fp);
}

static void FlushCallback(png_structp png_ptr) {
  FILE *fp = static_cast<FILE*>(png_get_io_ptr(png_ptr));
  if (fp == nullptr) return;
  (void)fflush(fp);
}

extern "C" int LLVMFuzzerTestOneInput_45(const uint8_t *Data, size_t Size) {
  FILE *fp = std::fopen("./dummy_file", "wb");
  if (!fp) return 0;

  if (Size > 0) {
    (void)fwrite(Data, 1, Size, fp);
    (void)fflush(fp);
    std::rewind(fp);
  }

  png_structp png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (!png_ptr) {
    std::fclose(fp);
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    png_destroy_write_struct(&png_ptr, nullptr);
    std::fclose(fp);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    std::fclose(fp);
    return 0;
  }

  png_set_write_fn(png_ptr, fp, WriteCallback, FlushCallback);

  size_t off = 0;
  uint32_t width = (ReadU32(Data, Size, off) % 8) + 1;
  uint32_t height = (ReadU32(Data, Size, off) % 8) + 1;

  int bit_depth_choices[] = {1, 2, 4, 8, 16};
  int color_type_choices[] = {
      PNG_COLOR_TYPE_GRAY,
      PNG_COLOR_TYPE_RGB,
      PNG_COLOR_TYPE_PALETTE,
      PNG_COLOR_TYPE_GRAY_ALPHA,
      PNG_COLOR_TYPE_RGB_ALPHA};

  int bit_depth = bit_depth_choices[ReadU8(Data, Size, off) % 5];
  int color_type = color_type_choices[ReadU8(Data, Size, off) % 5];
  int interlace = (ReadU8(Data, Size, off) & 1) ? PNG_INTERLACE_ADAM7 : PNG_INTERLACE_NONE;
  int compression = PNG_COMPRESSION_TYPE_BASE;
  int filter = PNG_FILTER_TYPE_BASE;

  if (color_type == PNG_COLOR_TYPE_PALETTE && bit_depth == 16)
    bit_depth = 8;

  png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, color_type,
               interlace, compression, filter);

  if (color_type == PNG_COLOR_TYPE_PALETTE) {
    png_color palette[256];
    for (int i = 0; i < 256; ++i) {
      palette[i].red = static_cast<png_byte>(i);
      palette[i].green = static_cast<png_byte>(255 - i);
      palette[i].blue = static_cast<png_byte>((i * 37) & 0xff);
    }
    int num_palette = (ReadU8(Data, Size, off) % 256) + 1;
    png_set_PLTE(png_ptr, info_ptr, palette, num_palette);
  }

  png_time mod_time;
  mod_time.year = static_cast<png_uint_16>(ReadU16(Data, Size, off));
  mod_time.month = static_cast<png_byte>(ReadU8(Data, Size, off));
  mod_time.day = static_cast<png_byte>(ReadU8(Data, Size, off));
  mod_time.hour = static_cast<png_byte>(ReadU8(Data, Size, off));
  mod_time.minute = static_cast<png_byte>(ReadU8(Data, Size, off));
  mod_time.second = static_cast<png_byte>(ReadU8(Data, Size, off));
  png_set_tIME(png_ptr, info_ptr, &mod_time);

  double gamma = ReadDoubleLike(Data, Size, off);
  png_set_gAMA(png_ptr, info_ptr, gamma);

  double white_x = ReadDoubleLike(Data, Size, off);
  double white_y = ReadDoubleLike(Data, Size, off);
  double red_x = ReadDoubleLike(Data, Size, off);
  double red_y = ReadDoubleLike(Data, Size, off);
  double green_x = ReadDoubleLike(Data, Size, off);
  double green_y = ReadDoubleLike(Data, Size, off);
  double blue_x = ReadDoubleLike(Data, Size, off);
  double blue_y = ReadDoubleLike(Data, Size, off);
  png_set_cHRM(png_ptr, info_ptr, white_x, white_y, red_x, red_y,
               green_x, green_y, blue_x, blue_y);

  double red_X = ReadDoubleLike(Data, Size, off);
  double red_Y = ReadDoubleLike(Data, Size, off);
  double red_Z = ReadDoubleLike(Data, Size, off);
  double green_X = ReadDoubleLike(Data, Size, off);
  double green_Y = ReadDoubleLike(Data, Size, off);
  double green_Z = ReadDoubleLike(Data, Size, off);
  double blue_X = ReadDoubleLike(Data, Size, off);
  double blue_Y = ReadDoubleLike(Data, Size, off);
  double blue_Z = ReadDoubleLike(Data, Size, off);
  png_set_cHRM_XYZ(png_ptr, info_ptr, red_X, red_Y, red_Z,
                   green_X, green_Y, green_Z, blue_X, blue_Y, blue_Z);

  png_set_cHRM_fixed(
      png_ptr, info_ptr,
      ReadFixed(Data, Size, off), ReadFixed(Data, Size, off),
      ReadFixed(Data, Size, off), ReadFixed(Data, Size, off),
      ReadFixed(Data, Size, off), ReadFixed(Data, Size, off),
      ReadFixed(Data, Size, off), ReadFixed(Data, Size, off));

  png_set_cHRM_XYZ_fixed(
      png_ptr, info_ptr,
      ReadFixed(Data, Size, off), ReadFixed(Data, Size, off), ReadFixed(Data, Size, off),
      ReadFixed(Data, Size, off), ReadFixed(Data, Size, off), ReadFixed(Data, Size, off),
      ReadFixed(Data, Size, off), ReadFixed(Data, Size, off), ReadFixed(Data, Size, off));

  png_write_info(png_ptr, info_ptr);

  int channels = 0;
  switch (color_type) {
    case PNG_COLOR_TYPE_GRAY: channels = 1; break;
    case PNG_COLOR_TYPE_PALETTE: channels = 1; break;
    case PNG_COLOR_TYPE_RGB: channels = 3; break;
    case PNG_COLOR_TYPE_GRAY_ALPHA: channels = 2; break;
    case PNG_COLOR_TYPE_RGB_ALPHA: channels = 4; break;
    default: channels = 1; break;
  }

  size_t bytes_per_sample = (bit_depth == 16) ? 2 : 1;
  size_t rowbytes = 0;
  if (color_type == PNG_COLOR_TYPE_PALETTE || color_type == PNG_COLOR_TYPE_GRAY) {
    rowbytes = (static_cast<size_t>(width) * channels * static_cast<size_t>(bit_depth) + 7) / 8;
  } else {
    rowbytes = static_cast<size_t>(width) * channels * bytes_per_sample;
  }
  if (rowbytes == 0) rowbytes = 1;

  std::vector<png_byte> row(rowbytes);
  for (png_uint_32 y = 0; y < height; ++y) {
    for (size_t i = 0; i < rowbytes; ++i) {
      row[i] = (off < Size) ? Data[off++] : static_cast<uint8_t>(i + y);
    }
    png_write_row(png_ptr, row.data());
  }

  if (ReadU8(Data, Size, off) & 1)
    png_set_tIME(png_ptr, info_ptr, &mod_time);

  png_write_end(png_ptr, info_ptr);

  png_destroy_write_struct(&png_ptr, &info_ptr);
  std::fclose(fp);
  return 0;
}