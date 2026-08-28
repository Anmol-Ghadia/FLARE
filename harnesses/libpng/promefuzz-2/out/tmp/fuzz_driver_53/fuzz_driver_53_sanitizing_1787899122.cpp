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
#include <algorithm>

static uint32_t ReadU32(const uint8_t *data, size_t size, size_t offset) {
  if (offset + 4 > size) return 0;
  return (static_cast<uint32_t>(data[offset]) << 24) |
         (static_cast<uint32_t>(data[offset + 1]) << 16) |
         (static_cast<uint32_t>(data[offset + 2]) << 8) |
         static_cast<uint32_t>(data[offset + 3]);
}

static int SafeSetJmp(png_structp png_ptr) {
  return setjmp(png_jmpbuf(png_ptr));
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  if (Data == nullptr) return 0;

  uint32_t w = (ReadU32(Data, Size, 0) % 64) + 1;
  uint32_t h = (ReadU32(Data, Size, 4) % 64) + 1;
  uint8_t format_selector = Size > 8 ? Data[8] : 0;
  uint8_t flags = Size > 9 ? Data[9] : 0;
  int srgb_intent = Size > 10 ? (Data[10] % 8) - 2 : 0;
  int flush_rows = Size > 11 ? static_cast<int>(static_cast<int8_t>(Data[11])) : 0;
  png_int_32 row_stride = Size > 15 ? static_cast<png_int_32>(ReadU32(Data, Size, 12) % 4096) : 0;

  png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (png_ptr != nullptr) {
    if (!SafeSetJmp(png_ptr)) {
      png_infop info_ptr = png_create_info_struct(png_ptr);
      if (info_ptr != nullptr) {
        png_set_flush(png_ptr, flush_rows);
        png_set_sRGB_gAMA_and_cHRM(png_ptr, info_ptr, srgb_intent);

        FILE *sigfile = std::fopen("./dummy_file", "wb");
        if (sigfile != nullptr) {
          png_init_io(png_ptr, sigfile);
          png_write_sig(png_ptr);
          std::fclose(sigfile);
        }
      }
      png_destroy_write_struct(&png_ptr, &info_ptr);
    } else {
      png_destroy_write_struct(&png_ptr, nullptr);
    }
  }

  png_image image;
  std::memset(&image, 0, sizeof(image));
  image.version = PNG_IMAGE_VERSION;
  image.opaque = nullptr;
  image.width = w;
  image.height = h;

  switch (format_selector % 6) {
    case 0:
      image.format = PNG_FORMAT_GRAY;
      break;
    case 1:
      image.format = PNG_FORMAT_GA;
      break;
    case 2:
      image.format = PNG_FORMAT_RGB;
      break;
    case 3:
      image.format = PNG_FORMAT_RGBA;
      break;
    case 4:
      image.format = PNG_FORMAT_LINEAR_RGB;
      break;
    default:
      image.format = PNG_FORMAT_LINEAR_RGBA;
      break;
  }

  png_alloc_size_t image_size = 0;
  if (image.width > 0 && image.height > 0) {
    image_size = PNG_IMAGE_SIZE(image);
  }

  std::vector<uint8_t> buffer;
  if (image_size > 0 && image_size < (1u << 24)) {
    buffer.resize(image_size);
    if (Size > 0) {
      for (png_alloc_size_t i = 0; i < image_size; ++i) {
        buffer[i] = Data[i % Size];
      }
    }
  }

  const void *buffer_ptr = buffer.empty() ? nullptr : buffer.data();
  const void *colormap_ptr = nullptr;

  png_alloc_size_t memory_bytes = 0;
  png_image image_mem_probe = image;
  png_image_write_to_memory(&image_mem_probe, nullptr, &memory_bytes,
                            flags & 1, buffer_ptr, row_stride, colormap_ptr);

  if (memory_bytes > 0 && memory_bytes < (1u << 26) && buffer_ptr != nullptr) {
    std::vector<uint8_t> out_mem(memory_bytes);
    png_alloc_size_t out_size = memory_bytes;
    png_image image_mem_write = image;
    png_image_write_to_memory(&image_mem_write, out_mem.data(), &out_size,
                              (flags >> 1) & 1, buffer_ptr, row_stride, colormap_ptr);
  } else {
    png_alloc_size_t tiny_size = 1;
    uint8_t tiny_buf[1] = {0};
    png_image image_mem_small = image;
    png_image_write_to_memory(&image_mem_small, tiny_buf, &tiny_size,
                              (flags >> 1) & 1, buffer_ptr, row_stride, colormap_ptr);
  }

  if (buffer_ptr != nullptr) {
    FILE *fp = std::fopen("./dummy_file", "wb");
    if (fp != nullptr) {
      png_image image_stdio = image;
      png_image_write_to_stdio(&image_stdio, fp, (flags >> 2) & 1,
                               buffer_ptr, row_stride, colormap_ptr);
      std::fclose(fp);
    }

    png_image image_file = image;
    png_image_write_to_file(&image_file, "./dummy_file", (flags >> 3) & 1,
                            buffer_ptr, row_stride, colormap_ptr);
  }

  return 0;
}