// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_build_grayscale_palette at png.c:875:1 in png.h
// png_build_grayscale_palette at png.c:875:1 in png.h
// png_build_grayscale_palette at png.c:875:1 in png.h
// png_set_PLTE at pngset.c:572:1 in png.h
// png_set_PLTE at pngset.c:572:1 in png.h
// png_set_hIST at pngset.c:210:1 in png.h
// png_set_sRGB_gAMA_and_cHRM at pngset.c:665:1 in png.h
// png_set_quantize at pngrtran.c:428:1 in png.h
// png_set_quantize at pngrtran.c:428:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_image_write_to_memory at pngwrite.c:2207:1 in png.h
// png_image_write_to_memory at pngwrite.c:2207:1 in png.h
// png_image_write_to_memory at pngwrite.c:2207:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
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
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <algorithm>

static uint32_t ReadU32(const uint8_t *data, size_t size, size_t &off) {
  if (off + 4 > size) return 0;
  uint32_t v = (uint32_t)data[off] |
               ((uint32_t)data[off + 1] << 8) |
               ((uint32_t)data[off + 2] << 16) |
               ((uint32_t)data[off + 3] << 24);
  off += 4;
  return v;
}

static int ReadInt(const uint8_t *data, size_t size, size_t &off) {
  return static_cast<int>(ReadU32(data, size, off));
}

static png_byte ReadByte(const uint8_t *data, size_t size, size_t &off) {
  if (off >= size) return 0;
  return data[off++];
}

static void PngErrorFn(png_structp, png_const_charp) {
  /* Prevent libpng from aborting the process. */
}

static void PngWarnFn(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput_36(const uint8_t *Data, size_t Size) {
  size_t off = 0;

  int width = (ReadInt(Data, Size, off) % 32);
  int height = (ReadInt(Data, Size, off) % 32);
  if (width <= 0) width = 1;
  if (height <= 0) height = 1;

  const int color_types[] = {
      PNG_COLOR_TYPE_GRAY,
      PNG_COLOR_TYPE_RGB,
      PNG_COLOR_TYPE_PALETTE,
      PNG_COLOR_TYPE_GRAY_ALPHA,
      PNG_COLOR_TYPE_RGB_ALPHA
  };
  const int bit_depths[] = {1, 2, 4, 8, 16};

  int color_type = color_types[ReadByte(Data, Size, off) % (sizeof(color_types) / sizeof(color_types[0]))];
  int bit_depth = bit_depths[ReadByte(Data, Size, off) % (sizeof(bit_depths) / sizeof(bit_depths[0]))];

  if (color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_GRAY_ALPHA ||
      color_type == PNG_COLOR_TYPE_RGB_ALPHA) {
    if (bit_depth < 8) bit_depth = 8;
  }
  if (color_type == PNG_COLOR_TYPE_PALETTE && bit_depth == 16) bit_depth = 8;

  png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr,
                                                PngErrorFn, PngWarnFn);
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

  png_set_IHDR(png_ptr, info_ptr,
               static_cast<png_uint_32>(width),
               static_cast<png_uint_32>(height),
               bit_depth, color_type,
               PNG_INTERLACE_NONE,
               PNG_COMPRESSION_TYPE_BASE,
               PNG_FILTER_TYPE_BASE);

  png_color palette[PNG_MAX_PALETTE_LENGTH];
  png_uint_16 hist[PNG_MAX_PALETTE_LENGTH];
  memset(palette, 0, sizeof(palette));
  memset(hist, 0, sizeof(hist));

  for (int i = 0; i < PNG_MAX_PALETTE_LENGTH; ++i) {
    palette[i].red = ReadByte(Data, Size, off);
    palette[i].green = ReadByte(Data, Size, off);
    palette[i].blue = ReadByte(Data, Size, off);
    hist[i] = static_cast<png_uint_16>(ReadByte(Data, Size, off) |
                                       (static_cast<png_uint_16>(ReadByte(Data, Size, off)) << 8));
  }

  int gray_bd_choices[] = {1, 2, 4, 8, 3, 16, 0};
  int gray_bd = gray_bd_choices[ReadByte(Data, Size, off) % (sizeof(gray_bd_choices) / sizeof(gray_bd_choices[0]))];
  png_build_grayscale_palette(gray_bd, palette);
  png_build_grayscale_palette(bit_depth, palette);
  png_build_grayscale_palette(8, nullptr);

  int max_palette_for_type = PNG_MAX_PALETTE_LENGTH;
  if (color_type == PNG_COLOR_TYPE_PALETTE) {
    max_palette_for_type = 1 << bit_depth;
    if (max_palette_for_type > PNG_MAX_PALETTE_LENGTH)
      max_palette_for_type = PNG_MAX_PALETTE_LENGTH;
  }

  int num_palette = 0;
  if (max_palette_for_type > 0)
    num_palette = ReadByte(Data, Size, off) % (max_palette_for_type + 1);

  int maybe_invalid_num_palette = ReadInt(Data, Size, off) % (PNG_MAX_PALETTE_LENGTH + 32);
  if (ReadByte(Data, Size, off) & 1)
    png_set_PLTE(png_ptr, info_ptr, palette, num_palette);
  else
    png_set_PLTE(png_ptr, info_ptr, palette, maybe_invalid_num_palette);

  png_set_hIST(png_ptr, info_ptr, hist);

  int srgb_intent = ReadByte(Data, Size, off) % 4;
  png_set_sRGB_gAMA_and_cHRM(png_ptr, info_ptr, srgb_intent);

  int quant_num_palette = (ReadByte(Data, Size, off) % PNG_MAX_PALETTE_LENGTH) + 1;
  int maximum_colors = (ReadByte(Data, Size, off) % PNG_MAX_PALETTE_LENGTH) + 1;
  int full_quantize = ReadByte(Data, Size, off) & 1;
  png_set_quantize(png_ptr, palette, quant_num_palette, maximum_colors, hist, full_quantize);
  png_set_quantize(png_ptr, palette, quant_num_palette, maximum_colors, nullptr, full_quantize ^ 1);

  png_image image;
  memset(&image, 0, sizeof(image));
  image.version = PNG_IMAGE_VERSION;
  image.width = static_cast<png_uint_32>(width);
  image.height = static_cast<png_uint_32>(height);

  int format_choices[] = {
      PNG_FORMAT_GRAY,
      PNG_FORMAT_GA,
      PNG_FORMAT_RGB,
      PNG_FORMAT_RGBA
  };
  image.format = format_choices[ReadByte(Data, Size, off) % (sizeof(format_choices) / sizeof(format_choices[0]))];

  png_uint_32 pixel_size = PNG_IMAGE_PIXEL_COMPONENT_SIZE(image.format) *
                           PNG_IMAGE_PIXEL_CHANNELS(image.format);
  if (pixel_size == 0) pixel_size = 1;

  size_t buffer_size = static_cast<size_t>(width) * static_cast<size_t>(height) * pixel_size;
  if (buffer_size == 0 || buffer_size > (1u << 20)) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    return 0;
  }

  std::vector<uint8_t> image_buffer(buffer_size);
  for (size_t i = 0; i < buffer_size; ++i) {
    image_buffer[i] = ReadByte(Data, Size, off);
  }

  png_alloc_size_t memory_bytes = 0;
  png_image_write_to_memory(&image, nullptr, &memory_bytes,
                            ReadByte(Data, Size, off) & 1,
                            image_buffer.data(),
                            ReadInt(Data, Size, off),
                            nullptr);

  if (memory_bytes > 0 && memory_bytes < (1u << 22)) {
    std::vector<uint8_t> out(memory_bytes);
    png_alloc_size_t out_size = memory_bytes;
    png_image_write_to_memory(&image, out.data(), &out_size,
                              ReadByte(Data, Size, off) & 1,
                              image_buffer.data(),
                              ReadInt(Data, Size, off),
                              nullptr);

    if (!out.empty()) {
      png_alloc_size_t smaller = out_size > 0 ? out_size - 1 : 0;
      png_image_write_to_memory(&image, out.data(), &smaller,
                                ReadByte(Data, Size, off) & 1,
                                image_buffer.data(),
                                ReadInt(Data, Size, off),
                                nullptr);
    }
  }

  png_destroy_write_struct(&png_ptr, &info_ptr);
  return 0;
}