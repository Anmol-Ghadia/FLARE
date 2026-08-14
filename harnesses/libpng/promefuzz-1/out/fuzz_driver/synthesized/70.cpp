// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_bgr at pngtrans.c:21:1 in png.h
// png_set_filler at pngtrans.c:120:1 in png.h
// png_write_info at pngwrite.c:192:1 in png.h
// png_get_rowbytes at pngget.c:30:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_rows at pngset.c:1546:1 in png.h
// png_write_image at pngwrite.c:596:1 in png.h
// png_write_rows at pngwrite.c:574:1 in png.h
// png_write_row at pngwrite.c:693:1 in png.h
// png_write_end at pngwrite.c:358:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
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
#include <stdint.h>
#include <stddef.h>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cstring>

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarnFn(png_structp, png_const_charp) {
}

static uint32_t ReadU32(const uint8_t *data, size_t size, size_t &off) {
  if (off + 4 > size) return 0;
  uint32_t v = (uint32_t)data[off] |
               ((uint32_t)data[off + 1] << 8) |
               ((uint32_t)data[off + 2] << 16) |
               ((uint32_t)data[off + 3] << 24);
  off += 4;
  return v;
}

extern "C" int LLVMFuzzerTestOneInput_70(const uint8_t *Data, size_t Size) {
  size_t off = 0;

  uint32_t width = (ReadU32(Data, Size, off) % 32) + 1;
  uint32_t height = (ReadU32(Data, Size, off) % 32) + 1;

  int color_modes[] = {
      PNG_COLOR_TYPE_GRAY,
      PNG_COLOR_TYPE_RGB,
      PNG_COLOR_TYPE_PALETTE,
      PNG_COLOR_TYPE_GRAY_ALPHA,
      PNG_COLOR_TYPE_RGB_ALPHA
  };
  int color_type = color_modes[(Size > off ? Data[off++] : 0) % 5];

  int bit_depth_choices[] = {1, 2, 4, 8, 16};
  int bit_depth = bit_depth_choices[(Size > off ? Data[off++] : 0) % 5];

  if (color_type == PNG_COLOR_TYPE_RGB ||
      color_type == PNG_COLOR_TYPE_RGB_ALPHA ||
      color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
    if (bit_depth < 8) bit_depth = 8;
  }
  if (color_type == PNG_COLOR_TYPE_PALETTE && bit_depth == 16) bit_depth = 8;

  int interlace_type = (Size > off && (Data[off++] & 1)) ? PNG_INTERLACE_ADAM7 : PNG_INTERLACE_NONE;
  int compression_type = PNG_COMPRESSION_TYPE_BASE;
  int filter_method = PNG_FILTER_TYPE_BASE;

  FILE *fp = fopen("./dummy_file", "wb");
  if (!fp) return 0;

  png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn, PngWarnFn);
  if (!png_ptr) {
    fclose(fp);
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    png_destroy_write_struct(&png_ptr, nullptr);
    fclose(fp);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
    return 0;
  }

  png_init_io(png_ptr, fp);

  png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, color_type,
               interlace_type, compression_type, filter_method);

  if (color_type == PNG_COLOR_TYPE_PALETTE) {
    png_color palette[256];
    for (int i = 0; i < 256; ++i) {
      palette[i].red = (png_byte)i;
      palette[i].green = (png_byte)(255 - i);
      palette[i].blue = (png_byte)(i ^ 0xAA);
    }
    png_set_PLTE(png_ptr, info_ptr, palette, 1 << bit_depth);
  }

  if (Size > off && (Data[off++] & 1)) {
    png_set_bgr(png_ptr);
  }

  if (Size > off && (Data[off++] & 1)) {
    png_uint_32 filler = ReadU32(Data, Size, off);
    int flags = (Size > off && (Data[off++] & 1)) ? PNG_FILLER_AFTER : PNG_FILLER_BEFORE;
    png_set_filler(png_ptr, filler, flags);
  }

  png_write_info(png_ptr, info_ptr);

  png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);
  if (rowbytes == 0 || rowbytes > (1u << 20)) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
    return 0;
  }

  std::vector<png_byte> image_storage(rowbytes * height);
  std::vector<png_bytep> rows(height);

  for (uint32_t y = 0; y < height; ++y) {
    rows[y] = image_storage.data() + y * rowbytes;
    for (png_size_t x = 0; x < rowbytes; ++x) {
      if (off < Size) {
        rows[y][x] = Data[off++];
      } else {
        rows[y][x] = (png_byte)((x + y) & 0xFF);
      }
    }
  }

  png_set_rows(png_ptr, info_ptr, rows.data());

  uint8_t mode = (Size > off ? Data[off++] : 0) % 3;

  if (mode == 0) {
    png_write_image(png_ptr, rows.data());
  } else if (mode == 1) {
    uint32_t y = 0;
    while (y < height) {
      uint32_t chunk = 1;
      if (off < Size) chunk = (Data[off++] % 4) + 1;
      if (chunk > height - y) chunk = height - y;
      png_write_rows(png_ptr, &rows[y], chunk);
      y += chunk;
    }
  } else {
    for (uint32_t y = 0; y < height; ++y) {
      png_write_row(png_ptr, rows[y]);
    }
  }

  png_write_end(png_ptr, info_ptr);
  png_destroy_write_struct(&png_ptr, &info_ptr);
  fclose(fp);
  return 0;
}