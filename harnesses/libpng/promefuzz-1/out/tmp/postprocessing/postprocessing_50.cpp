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
#include <vector>
#include <cstring>
#include <algorithm>

static uint32_t ReadU32(const uint8_t *data, size_t size, size_t &off) {
  uint32_t v = 0;
  for (int i = 0; i < 4; ++i) {
    v <<= 8;
    if (off < size) v |= data[off++];
  }
  return v;
}

static int ReadI32(const uint8_t *data, size_t size, size_t &off) {
  return static_cast<int>(ReadU32(data, size, off));
}

static uint8_t ReadU8(const uint8_t *data, size_t size, size_t &off) {
  if (off < size) return data[off++];
  return 0;
}

static png_bytep MakeImageBuffer(png_image &image, const uint8_t *data, size_t size,
                                 size_t &off, png_alloc_size_t &out_size) {
  if (image.width == 0 || image.height == 0) return nullptr;

  png_alloc_size_t needed = 0;
  if (!png_image_finish_read(&image, nullptr, nullptr, 0, nullptr)) {
    // We only use this to let libpng populate defaults if possible; ignore failure.
  }

  png_image tmp = image;
  needed = PNG_IMAGE_SIZE(tmp);

  if (needed == 0 || needed > (1u << 26)) return nullptr;

  png_bytep buf = static_cast<png_bytep>(malloc(needed));
  if (!buf) return nullptr;

  if (size > off) {
    size_t remain = size - off;
    size_t to_copy = std::min<size_t>(remain, needed);
    memcpy(buf, data + off, to_copy);
    if (to_copy < needed) memset(buf + to_copy, 0, needed - to_copy);
  } else {
    memset(buf, 0, needed);
  }

  out_size = needed;
  return buf;
}

static void InitImage(png_image &image, const uint8_t *data, size_t size, size_t &off) {
  memset(&image, 0, sizeof(image));
  image.version = PNG_IMAGE_VERSION;

  uint32_t w = ReadU32(data, size, off);
  uint32_t h = ReadU32(data, size, off);

  image.width = (w % 64) + 1;
  image.height = (h % 64) + 1;

  static const png_uint_32 formats[] = {
      PNG_FORMAT_GRAY,
      PNG_FORMAT_GA,
      PNG_FORMAT_RGB,
      PNG_FORMAT_RGBA,
      PNG_FORMAT_LINEAR_Y,
      PNG_FORMAT_LINEAR_Y_ALPHA,
      PNG_FORMAT_LINEAR_RGB,
      PNG_FORMAT_LINEAR_RGB_ALPHA
  };
  image.format = formats[ReadU8(data, size, off) % (sizeof(formats) / sizeof(formats[0]))];

  image.flags = 0;
  if (ReadU8(data, size, off) & 1) image.flags |= PNG_IMAGE_FLAG_COLORSPACE_NOT_sRGB;
  if (ReadU8(data, size, off) & 1) image.flags |= PNG_IMAGE_FLAG_FAST;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  size_t off = 0;

  png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  png_infop info_ptr = nullptr;
  if (png_ptr) {
    info_ptr = png_create_info_struct(png_ptr);

    png_set_compression_window_bits(png_ptr, ReadI32(Data, Size, off));
    png_set_compression_window_bits(png_ptr, ReadI32(Data, Size, off));

    png_fixed_point weights[8];
    png_fixed_point costs[8];
    for (int i = 0; i < 8; ++i) weights[i] = static_cast<png_fixed_point>(ReadI32(Data, Size, off));
    for (int i = 0; i < 8; ++i) costs[i] = static_cast<png_fixed_point>(ReadI32(Data, Size, off));

    png_set_filter_heuristics_fixed(
        png_ptr,
        ReadI32(Data, Size, off) % 4,
        ReadI32(Data, Size, off) % 8,
        weights,
        costs);

    png_set_text_compression_strategy(png_ptr, ReadI32(Data, Size, off));
    png_set_text_compression_strategy(png_ptr, ReadI32(Data, Size, off));
  }

  png_image image1;
  InitImage(image1, Data, Size, off);
  png_alloc_size_t buf_size1 = 0;
  png_bytep buffer1 = MakeImageBuffer(image1, Data, Size, off, buf_size1);
  if (buffer1) {
    png_alloc_size_t memory_bytes = 0;
    (void)png_image_write_to_memory(&image1, nullptr, &memory_bytes,
                                    ReadU8(Data, Size, off) & 1,
                                    buffer1,
                                    static_cast<png_int_32>(ReadI32(Data, Size, off)),
                                    nullptr);

    if (memory_bytes > 0 && memory_bytes < (1u << 26)) {
      std::vector<uint8_t> out(memory_bytes);
      png_image image2;
      InitImage(image2, Data, Size, off);
      image2.width = image1.width;
      image2.height = image1.height;
      image2.format = image1.format;
      image2.flags = image1.flags;

      png_alloc_size_t memory_bytes2 = memory_bytes;
      (void)png_image_write_to_memory(&image2, out.data(), &memory_bytes2,
                                      ReadU8(Data, Size, off) & 1,
                                      buffer1,
                                      static_cast<png_int_32>(ReadI32(Data, Size, off)),
                                      nullptr);
    }
    free(buffer1);
  }

  png_image image3;
  InitImage(image3, Data, Size, off);
  png_alloc_size_t buf_size3 = 0;
  png_bytep buffer3 = MakeImageBuffer(image3, Data, Size, off, buf_size3);
  if (buffer3) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
      (void)png_image_write_to_stdio(&image3, fp,
                                     ReadU8(Data, Size, off) & 1,
                                     buffer3,
                                     static_cast<png_int_32>(ReadI32(Data, Size, off)),
                                     nullptr);
      fclose(fp);
    }
    free(buffer3);
  }

  png_image image4;
  InitImage(image4, Data, Size, off);
  png_alloc_size_t buf_size4 = 0;
  png_bytep buffer4 = MakeImageBuffer(image4, Data, Size, off, buf_size4);
  if (buffer4) {
    (void)png_image_write_to_file(&image4, "./dummy_file",
                                  ReadU8(Data, Size, off) & 1,
                                  buffer4,
                                  static_cast<png_int_32>(ReadI32(Data, Size, off)),
                                  nullptr);
    free(buffer4);
  }

  if (info_ptr) png_destroy_info_struct(png_ptr, &info_ptr);
  if (png_ptr) png_destroy_write_struct(&png_ptr, nullptr);

  png_set_compression_window_bits(nullptr, ReadI32(Data, Size, off));
  png_set_filter_heuristics_fixed(nullptr, 0, 0, nullptr, nullptr);
  png_set_text_compression_strategy(nullptr, ReadI32(Data, Size, off));

  return 0;
}