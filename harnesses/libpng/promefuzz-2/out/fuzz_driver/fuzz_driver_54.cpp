// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_image_write_to_stdio at pngwrite.c:2274:1 in png.h
// png_image_write_to_file at pngwrite.c:2323:1 in png.h
// png_image_write_to_memory at pngwrite.c:2207:1 in png.h
// png_image_write_to_memory at pngwrite.c:2207:1 in png.h
// png_image_write_to_memory at pngwrite.c:2207:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_init_io at png.c:703:1 in png.h
// png_set_flush at pngwrite.c:903:1 in png.h
// png_write_sig at pngwutil.c:51:1 in png.h
// png_set_sRGB_gAMA_and_cHRM at pngset.c:665:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <algorithm>

static uint32_t ReadU32(const uint8_t *data, size_t size, size_t &off) {
  uint32_t v = 0;
  for (int i = 0; i < 4; ++i) {
    v <<= 8;
    if (off < size) v |= data[off++];
  }
  return v;
}

static int ReadBool(const uint8_t *data, size_t size, size_t &off) {
  if (off < size) return (data[off++] & 1) != 0;
  return 0;
}

static png_uint_32 ChooseFormat(uint8_t selector) {
  switch (selector % 10) {
    case 0: return PNG_FORMAT_GRAY;
    case 1: return PNG_FORMAT_GA;
    case 2: return PNG_FORMAT_RGB;
    case 3: return PNG_FORMAT_RGBA;
    case 4: return PNG_FORMAT_LINEAR_Y;
    case 5: return PNG_FORMAT_LINEAR_Y_ALPHA;
    case 6: return PNG_FORMAT_LINEAR_RGB;
    case 7: return PNG_FORMAT_LINEAR_RGB_ALPHA;
    case 8: return PNG_FORMAT_FLAG_COLOR | PNG_FORMAT_FLAG_COLORMAP;
    default: return PNG_FORMAT_RGB_COLORMAP;
  }
}

static size_t AbsStrideBytes(png_int_32 row_stride, png_uint_32 width, png_uint_32 format) {
  const png_uint_32 channels = PNG_IMAGE_SAMPLE_CHANNELS(format);
  const png_uint_32 sample_size_bits = PNG_IMAGE_SAMPLE_SIZE(format);
  const png_uint_32 bytes_per_pixel = channels * (sample_size_bits / 8);
  size_t min_row = static_cast<size_t>(width) * bytes_per_pixel;
  if (row_stride == 0) return min_row;
  png_int_32 abs_rs = row_stride < 0 ? -row_stride : row_stride;
  return static_cast<size_t>(abs_rs) < min_row ? min_row : static_cast<size_t>(abs_rs);
}

static size_t ComputeBufferSize(const png_image &image, png_int_32 row_stride) {
  if (row_stride == 0) {
    return PNG_IMAGE_SIZE(image);
  }
  size_t stride = AbsStrideBytes(row_stride, image.width, image.format);
  return stride * image.height;
}

static size_t ComputeColormapSize(const png_image &image) {
  if ((image.format & PNG_FORMAT_FLAG_COLORMAP) == 0) return 0;
  const png_uint_32 channels = PNG_IMAGE_SAMPLE_CHANNELS(image.format);
  const png_uint_32 sample_size_bits = PNG_IMAGE_SAMPLE_SIZE(image.format);
  const png_uint_32 bytes_per_entry = channels * (sample_size_bits / 8);
  return 256u * bytes_per_entry;
}

static void PngErrorFn(png_structp, png_const_charp) {
  /* Prevent libpng longjmp from terminating the fuzzer process. */
}

static void PngWarnFn(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  size_t off = 0;

  png_uint_32 width = (ReadU32(Data, Size, off) % 64) + 1;
  png_uint_32 height = (ReadU32(Data, Size, off) % 64) + 1;
  png_uint_32 format = ChooseFormat(off < Size ? Data[off++] : 0);
  int convert_to_8_bit = ReadBool(Data, Size, off);

  png_image image;
  memset(&image, 0, sizeof(image));
  image.version = PNG_IMAGE_VERSION;
  image.width = width;
  image.height = height;
  image.format = format;

  png_int_32 row_stride = 0;
  {
    int mode = off < Size ? (Data[off++] % 3) : 0;
    size_t min_row = AbsStrideBytes(0, width, format);
    if (mode == 1) {
      row_stride = static_cast<png_int_32>(min_row);
    } else if (mode == 2) {
      row_stride = -static_cast<png_int_32>(min_row);
    }
  }

  size_t buffer_size = ComputeBufferSize(image, row_stride);
  std::vector<uint8_t> buffer(buffer_size ? buffer_size : 1);
  for (size_t i = 0; i < buffer.size(); ++i) {
    buffer[i] = (off < Size) ? Data[off++] : static_cast<uint8_t>(i);
  }

  std::vector<uint8_t> colormap;
  const void *colormap_ptr = NULL;
  size_t colormap_size = ComputeColormapSize(image);
  if (colormap_size != 0) {
    colormap.resize(colormap_size);
    for (size_t i = 0; i < colormap.size(); ++i) {
      colormap[i] = (off < Size) ? Data[off++] : static_cast<uint8_t>(255 - i);
    }
    colormap_ptr = colormap.data();
  }

  const void *buffer_ptr = buffer.data();
  if (row_stride < 0 && image.height > 0) {
    size_t stride = AbsStrideBytes(row_stride, width, format);
    size_t start = stride * (image.height - 1);
    if (start < buffer.size()) {
      buffer_ptr = buffer.data() + start;
    }
  }

  {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
      png_image img_stdio = image;
      png_image_write_to_stdio(&img_stdio, fp, convert_to_8_bit, buffer_ptr, row_stride, colormap_ptr);
      fclose(fp);
    }
  }

  {
    png_image img_file = image;
    png_image_write_to_file(&img_file, "./dummy_file", convert_to_8_bit, buffer_ptr, row_stride, colormap_ptr);
  }

  {
    png_alloc_size_t memory_bytes = 0;
    png_image img_mem_count = image;
    png_image_write_to_memory(&img_mem_count, NULL, &memory_bytes, convert_to_8_bit, buffer_ptr, row_stride, colormap_ptr);

    if (memory_bytes > 0 && memory_bytes < (1u << 24)) {
      std::vector<uint8_t> out(memory_bytes);
      png_alloc_size_t out_size = memory_bytes;
      png_image img_mem_write = image;
      png_image_write_to_memory(&img_mem_write, out.data(), &out_size, convert_to_8_bit, buffer_ptr, row_stride, colormap_ptr);
    } else {
      png_alloc_size_t small_size = 1;
      uint8_t small_buf[1] = {0};
      png_image img_mem_small = image;
      png_image_write_to_memory(&img_mem_small, small_buf, &small_size, convert_to_8_bit, buffer_ptr, row_stride, colormap_ptr);
    }
  }

  {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
      png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, PngErrorFn, PngWarnFn);
      if (png_ptr != NULL) {
        png_infop info_ptr = png_create_info_struct(png_ptr);
        if (info_ptr != NULL) {
          if (setjmp(png_jmpbuf(png_ptr)) == 0) {
            png_init_io(png_ptr, fp);
            png_set_flush(png_ptr, static_cast<int>(ReadU32(Data, Size, off) & 0x7fffffff));
            png_write_sig(png_ptr);
            png_set_sRGB_gAMA_and_cHRM(png_ptr, info_ptr, static_cast<int>(ReadU32(Data, Size, off) % 4));
          }
          png_destroy_write_struct(&png_ptr, &info_ptr);
        } else {
          png_destroy_write_struct(&png_ptr, NULL);
        }
      }
      fclose(fp);
    }
  }

  return 0;
}