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
#include <algorithm>

static uint32_t ReadU32(const uint8_t *data, size_t size, size_t offset) {
  if (offset + 4 > size) return 0;
  return (static_cast<uint32_t>(data[offset]) << 24) |
         (static_cast<uint32_t>(data[offset + 1]) << 16) |
         (static_cast<uint32_t>(data[offset + 2]) << 8) |
         static_cast<uint32_t>(data[offset + 3]);
}

static png_int_32 ChooseRowStride(uint32_t selector, png_uint_32 width, png_uint_32 format) {
  png_int_32 natural = 0;
  if (width > 0) {
    png_alloc_size_t row_bytes = PNG_IMAGE_ROW_STRIDE(width, format);
    if (row_bytes <= 0x7fffffffU) natural = static_cast<png_int_32>(row_bytes);
  }

  switch (selector % 8) {
    case 0: return 0;
    case 1: return natural;
    case 2: return natural > 0 ? natural + 1 : 1;
    case 3: return natural > 1 ? natural - 1 : 0;
    case 4: return 1;
    case 5: return -1;
    case 6: return 0x7fffffff;
    default: return static_cast<png_int_32>(0x40000000);
  }
}

static void WriteDummyFile(const uint8_t *data, size_t size) {
  FILE *fp = std::fopen("./dummy_file", "wb");
  if (!fp) return;
  if (size > 0) std::fwrite(data, 1, size, fp);
  std::fclose(fp);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  WriteDummyFile(Data, Size);

  png_color background;
  std::memset(&background, 0, sizeof(background));
  if (Size > 0) background.red = Data[0];
#if defined(PNG_COLOR_TYPE_PALETTE) || 1
  if (sizeof(background) > 1 && Size > 1) {
    reinterpret_cast<uint8_t*>(&background)[1] = Data[1];
  }
  if (sizeof(background) > 2 && Size > 2) {
    reinterpret_cast<uint8_t*>(&background)[2] = Data[2];
  }
#endif

  png_color colormap_storage[256];
  std::memset(colormap_storage, 0, sizeof(colormap_storage));
  for (size_t i = 0; i < 256; ++i) {
    size_t base = 3 * i;
    if (base < Size) colormap_storage[i].red = Data[base];
    if (sizeof(colormap_storage[i]) > 1 && base + 1 < Size)
      reinterpret_cast<uint8_t*>(&colormap_storage[i])[1] = Data[base + 1];
    if (sizeof(colormap_storage[i]) > 2 && base + 2 < Size)
      reinterpret_cast<uint8_t*>(&colormap_storage[i])[2] = Data[base + 2];
  }

  {
    png_image image;
    std::memset(&image, 0, sizeof(image));
    image.version = PNG_IMAGE_VERSION;

    if (png_image_begin_read_from_memory(&image, Data, Size)) {
      png_uint_32 width = image.width;
      png_uint_32 height = image.height;

      uint32_t fmt_sel = ReadU32(Data, Size, 4);
      switch (fmt_sel % 6) {
        case 0: image.format = PNG_FORMAT_RGBA; break;
        case 1: image.format = PNG_FORMAT_RGB; break;
        case 2: image.format = PNG_FORMAT_GA; break;
        case 3: image.format = PNG_FORMAT_GRAY; break;
        case 4: image.format = PNG_FORMAT_BGRA; break;
        default: image.format = PNG_FORMAT_ABGR; break;
      }

      png_alloc_size_t buf_size = 0;
      bool alloc_ok = false;
      if (width > 0 && height > 0) {
        png_alloc_size_t row_bytes = PNG_IMAGE_ROW_STRIDE(width, image.format);
        if (row_bytes > 0) {
          png_alloc_size_t total = row_bytes * height;
          if (height == 0 || total / height == row_bytes) {
            buf_size = total;
            alloc_ok = true;
          }
        }
      }

      std::vector<uint8_t> buffer;
      if (alloc_ok && buf_size <= (1u << 26)) {
        buffer.resize(static_cast<size_t>(buf_size));
      } else {
        buffer.resize(std::min<size_t>(Size + 64, 1u << 20));
      }

      png_int_32 row_stride = ChooseRowStride(ReadU32(Data, Size, 8), width, image.format);
      png_const_colorp bg_ptr = (Size & 1) ? &background : nullptr;
      void *cmap_ptr = (Size & 2) ? static_cast<void*>(colormap_storage) : nullptr;

      (void)png_image_finish_read(&image, bg_ptr,
                                  buffer.empty() ? nullptr : buffer.data(),
                                  row_stride, cmap_ptr);

      png_image_free(&image);
      png_image_free(&image);
    } else {
      png_image_free(&image);
    }
  }

  {
    png_image image;
    std::memset(&image, 0, sizeof(image));
    image.version = PNG_IMAGE_VERSION;

    FILE *fp = std::fopen("./dummy_file", "rb");
    if (fp) {
      if (png_image_begin_read_from_stdio(&image, fp)) {
        png_uint_32 width = image.width;
        png_uint_32 height = image.height;

        uint32_t fmt_sel = ReadU32(Data, Size, 12);
        switch (fmt_sel % 4) {
          case 0: image.format = PNG_FORMAT_RGBA; break;
          case 1: image.format = PNG_FORMAT_RGB; break;
          case 2: image.format = PNG_FORMAT_GA; break;
          default: image.format = PNG_FORMAT_GRAY; break;
        }

        png_alloc_size_t buf_size = 0;
        bool alloc_ok = false;
        if (width > 0 && height > 0) {
          png_alloc_size_t row_bytes = PNG_IMAGE_ROW_STRIDE(width, image.format);
          if (row_bytes > 0) {
            png_alloc_size_t total = row_bytes * height;
            if (height == 0 || total / height == row_bytes) {
              buf_size = total;
              alloc_ok = true;
            }
          }
        }

        std::vector<uint8_t> buffer;
        if (alloc_ok && buf_size <= (1u << 26)) {
          buffer.resize(static_cast<size_t>(buf_size));
        } else {
          buffer.resize(std::min<size_t>(Size + 128, 1u << 20));
        }

        png_int_32 row_stride = ChooseRowStride(ReadU32(Data, Size, 16), width, image.format);
        png_const_colorp bg_ptr = (Size & 4) ? &background : nullptr;
        void *cmap_ptr = (Size & 8) ? static_cast<void*>(colormap_storage) : nullptr;

        (void)png_image_finish_read(&image, bg_ptr,
                                    buffer.empty() ? nullptr : buffer.data(),
                                    row_stride, cmap_ptr);
        png_image_free(&image);
      } else {
        png_image_free(&image);
        std::fclose(fp);
      }
    }
  }

  {
    png_image image;
    std::memset(&image, 0, sizeof(image));
    image.version = PNG_IMAGE_VERSION;

    if (png_image_begin_read_from_file(&image, "./dummy_file")) {
      png_uint_32 width = image.width;
      png_uint_32 height = image.height;

      uint32_t fmt_sel = ReadU32(Data, Size, 20);
      switch (fmt_sel % 5) {
        case 0: image.format = PNG_FORMAT_RGBA; break;
        case 1: image.format = PNG_FORMAT_RGB; break;
        case 2: image.format = PNG_FORMAT_GA; break;
        case 3: image.format = PNG_FORMAT_GRAY; break;
        default: image.format = PNG_FORMAT_BGRA; break;
      }

      png_alloc_size_t buf_size = 0;
      bool alloc_ok = false;
      if (width > 0 && height > 0) {
        png_alloc_size_t row_bytes = PNG_IMAGE_ROW_STRIDE(width, image.format);
        if (row_bytes > 0) {
          png_alloc_size_t total = row_bytes * height;
          if (height == 0 || total / height == row_bytes) {
            buf_size = total;
            alloc_ok = true;
          }
        }
      }

      std::vector<uint8_t> buffer;
      if (alloc_ok && buf_size <= (1u << 26)) {
        buffer.resize(static_cast<size_t>(buf_size));
      } else {
        buffer.resize(std::min<size_t>(Size + 256, 1u << 20));
      }

      png_int_32 row_stride = ChooseRowStride(ReadU32(Data, Size, 24), width, image.format);
      png_const_colorp bg_ptr = (Size & 16) ? &background : nullptr;
      void *cmap_ptr = (Size & 32) ? static_cast<void*>(colormap_storage) : nullptr;

      (void)png_image_finish_read(&image, bg_ptr,
                                  buffer.empty() ? nullptr : buffer.data(),
                                  row_stride, cmap_ptr);
      png_image_free(&image);
    } else {
      png_image_free(&image);
    }
  }

  {
    png_image image;
    std::memset(&image, 0, sizeof(image));
    image.version = PNG_IMAGE_VERSION;

    uint32_t width = (ReadU32(Data, Size, 28) % 64) + 1;
    uint32_t height = (ReadU32(Data, Size, 32) % 64) + 1;
    uint32_t fmt_sel = ReadU32(Data, Size, 36);

    switch (fmt_sel % 6) {
      case 0: image.format = PNG_FORMAT_RGBA; break;
      case 1: image.format = PNG_FORMAT_RGB; break;
      case 2: image.format = PNG_FORMAT_GA; break;
      case 3: image.format = PNG_FORMAT_GRAY; break;
      case 4: image.format = PNG_FORMAT_BGRA; break;
      default: image.format = PNG_FORMAT_ABGR; break;
    }

    image.width = width;
    image.height = height;

    png_alloc_size_t row_bytes = PNG_IMAGE_ROW_STRIDE(width, image.format);
    png_alloc_size_t total = row_bytes * height;
    std::vector<uint8_t> buffer;
    if (height == 0 || (row_bytes > 0 && total / height == row_bytes && total <= (1u << 20))) {
      buffer.resize(static_cast<size_t>(total));
    } else {
      buffer.resize(std::min<size_t>(Size + 512, 1u << 20));
    }

    if (!buffer.empty() && Size > 0) {
      for (size_t i = 0; i < buffer.size(); ++i) {
        buffer[i] = Data[i % Size];
      }
    }

    png_int_32 row_stride = ChooseRowStride(ReadU32(Data, Size, 40), width, image.format);
    const void *cmap_ptr = (Size & 64) ? static_cast<const void*>(colormap_storage) : nullptr;
    int convert_to_8bit = static_cast<int>(ReadU32(Data, Size, 44) & 1);

    (void)png_image_write_to_file(&image, "./dummy_file", convert_to_8bit,
                                  buffer.empty() ? nullptr : buffer.data(),
                                  row_stride, cmap_ptr);
    png_image_free(&image);
  }

  return 0;
}