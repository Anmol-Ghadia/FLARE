#include <png.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>

static void PngWriteCallback(png_structp png_ptr, png_bytep data, png_size_t length) {
  FILE* fp = reinterpret_cast<FILE*>(png_get_io_ptr(png_ptr));
  if (fp != NULL && data != NULL && length > 0) {
    fwrite(data, 1, length, fp);
  }
}

static void PngFlushCallback(png_structp png_ptr) {
  FILE* fp = reinterpret_cast<FILE*>(png_get_io_ptr(png_ptr));
  if (fp != NULL) {
    fflush(fp);
  }
}

static uint32_t ReadU32(const uint8_t* Data, size_t Size, size_t Off) {
  uint32_t v = 0;
  if (Off < Size) v |= (uint32_t)Data[Off] << 24;
  if (Off + 1 < Size) v |= (uint32_t)Data[Off + 1] << 16;
  if (Off + 2 < Size) v |= (uint32_t)Data[Off + 2] << 8;
  if (Off + 3 < Size) v |= (uint32_t)Data[Off + 3];
  return v;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE* fp = fopen("./dummy_file", "wb");
  if (fp == NULL) {
    return 0;
  }

  png_structp png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (png_ptr == NULL) {
    fclose(fp);
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == NULL) {
    png_destroy_write_struct(&png_ptr, NULL);
    fclose(fp);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
    return 0;
  }

  png_set_write_fn(png_ptr, fp, PngWriteCallback, PngFlushCallback);

  uint32_t width = (ReadU32(Data, Size, 0) % 32) + 1;
  uint32_t height = (ReadU32(Data, Size, 4) % 32) + 1;

  int color_type;
  switch ((Size > 8 ? Data[8] : 0) % 5) {
    case 0: color_type = PNG_COLOR_TYPE_GRAY; break;
    case 1: color_type = PNG_COLOR_TYPE_GRAY_ALPHA; break;
    case 2: color_type = PNG_COLOR_TYPE_RGB; break;
    case 3: color_type = PNG_COLOR_TYPE_RGB_ALPHA; break;
    default: color_type = PNG_COLOR_TYPE_PALETTE; break;
  }

  int bit_depth;
  switch ((Size > 9 ? Data[9] : 0) % 4) {
    case 0: bit_depth = 1; break;
    case 1: bit_depth = 2; break;
    case 2: bit_depth = 4; break;
    default: bit_depth = 8; break;
  }

  if (color_type == PNG_COLOR_TYPE_RGB ||
      color_type == PNG_COLOR_TYPE_RGB_ALPHA ||
      color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
    bit_depth = 8;
  }

  png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, color_type,
               PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE,
               PNG_FILTER_TYPE_BASE);

  png_color palette[256];
  if (color_type == PNG_COLOR_TYPE_PALETTE) {
    int palette_entries = 1 << bit_depth;
    for (int i = 0; i < palette_entries; ++i) {
      uint8_t b = (Size > 0) ? Data[(10 + (size_t)i) % Size] : (uint8_t)i;
      palette[i].red = b;
      palette[i].green = (png_byte)(b ^ 0x55);
      palette[i].blue = (png_byte)(b ^ 0xaa);
    }
    png_set_PLTE(png_ptr, info_ptr, palette, palette_entries);
  }

  png_write_info(png_ptr, info_ptr);

  png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);
  if (rowbytes == 0) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
    return 0;
  }

  size_t total_size = (size_t)rowbytes * (size_t)height;
  if (height != 0 && total_size / (size_t)height != (size_t)rowbytes) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
    return 0;
  }

  png_bytep image_data = (png_bytep)malloc(total_size);
  png_bytep* rows = (png_bytep*)malloc(sizeof(png_bytep) * height);
  if (image_data == NULL || rows == NULL) {
    free(image_data);
    free(rows);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
    return 0;
  }

  for (uint32_t y = 0; y < height; ++y) {
    rows[y] = image_data + (size_t)y * rowbytes;
    for (png_size_t x = 0; x < rowbytes; ++x) {
      rows[y][x] = (Size > 0) ? Data[(10 + ((size_t)y * rowbytes + x)) % Size] : 0;
    }
  }

  png_set_rows(png_ptr, info_ptr, rows);

  int compression_level = (Size > 10) ? (int)(int8_t)Data[10] : 0;
  int compression_strategy = (Size > 11) ? (int)(int8_t)Data[11] : 0;
  int flush_rows = (Size > 12) ? (int)(int8_t)Data[12] : 0;
  int heuristic_method = (Size > 13) ? (Data[13] % PNG_FILTER_HEURISTIC_LAST) : PNG_FILTER_HEURISTIC_DEFAULT;
  int num_weights = (Size > 14) ? (Data[14] % 8) : 0;
  int transforms = (Size > 15) ? Data[15] : 0;

  png_set_compression_level(png_ptr, compression_level);
  png_set_compression_strategy(png_ptr, compression_strategy);
  png_set_flush(png_ptr, flush_rows);

  double dweights[8];
  double dcosts[8];
  png_fixed_point fweights[8];
  png_fixed_point fcosts[8];
  for (int i = 0; i < 8; ++i) {
    uint8_t b1 = (Size > 0) ? Data[(16 + (size_t)i) % Size] : 0;
    uint8_t b2 = (Size > 0) ? Data[(24 + (size_t)i) % Size] : 0;
    dweights[i] = (double)b1 / 255.0;
    dcosts[i] = (double)b2 / 255.0;
    fweights[i] = (png_fixed_point)b1 * 1000;
    fcosts[i] = (png_fixed_point)b2 * 1000;
  }

  png_set_filter_heuristics(png_ptr, heuristic_method, num_weights, dweights, dcosts);
  png_set_filter_heuristics_fixed(png_ptr, heuristic_method, num_weights, fweights, fcosts);

  png_set_compression_level(NULL, compression_level);
  png_set_compression_strategy(NULL, compression_strategy);
  png_set_flush(NULL, flush_rows);
  png_set_filter_heuristics(NULL, heuristic_method, num_weights, dweights, dcosts);
  png_set_filter_heuristics_fixed(NULL, heuristic_method, num_weights, fweights, fcosts);

  png_write_png(png_ptr, info_ptr, transforms, NULL);

  free(rows);
  free(image_data);
  png_destroy_write_struct(&png_ptr, &info_ptr);
  fclose(fp);
  return 0;
}