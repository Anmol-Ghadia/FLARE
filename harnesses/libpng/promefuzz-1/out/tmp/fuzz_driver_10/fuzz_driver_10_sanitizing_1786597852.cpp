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
         (static_cast<uint32_t>(data[offset + 3]));
}

static size_t ClampSize(size_t v, size_t maxv) {
  return v < maxv ? v : maxv;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  if (Data == nullptr || Size == 0)
    return 0;

  FILE *fp = std::fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    std::fwrite(Data, 1, Size, fp);
    std::fclose(fp);
  }

  png_color background;
  std::memset(&background, 0, sizeof(background));
  if (Size > 0) background.red = Data[0];

  png_color cmap[256];
  std::memset(cmap, 0, sizeof(cmap));
  for (size_t i = 0; i < 256; ++i) {
    cmap[i].red = static_cast<png_byte>((i < Size) ? Data[i] : i);
  }

  {
    png_image image;
    std::memset(&image, 0, sizeof(image));
    image.version = PNG_IMAGE_VERSION;

    if (png_image_begin_read_from_memory(&image, Data, Size)) {
      png_uint_32 width = image.width;
      png_uint_32 height = image.height;
      png_uint_32 format_choices[] = {
          PNG_FORMAT_GRAY,
          PNG_FORMAT_GA,
          PNG_FORMAT_RGB,
          PNG_FORMAT_RGBA,
          PNG_FORMAT_LINEAR_RGB,
          PNG_FORMAT_LINEAR_RGBA,
          PNG_FORMAT_RGB_COLORMAP,
          PNG_FORMAT_RGBA_COLORMAP
      };
      image.format = format_choices[Size % (sizeof(format_choices) / sizeof(format_choices[0]))];

      png_alloc_size_t buffer_size = 0;
      if (width > 0 && height > 0) {
        buffer_size = PNG_IMAGE_SIZE(image);
      }

      if (buffer_size > 0 && buffer_size < (1u << 26)) {
        std::vector<uint8_t> buffer(buffer_size);
        png_int_32 stride = 0;
        if (Size >= 8) {
          stride = static_cast<png_int_32>(ReadU32(Data, Size, 4));
          if (Data[1] & 1) stride = -stride;
        }

        void *colormap_ptr = nullptr;
        if ((Data[2] & 1) != 0)
          colormap_ptr = cmap;

        png_const_colorp bg_ptr = nullptr;
        if ((Data[3] & 1) != 0)
          bg_ptr = &background;

        png_image_finish_read(&image, bg_ptr, buffer.data(), stride, colormap_ptr);

        png_alloc_size_t out_size = 0;
        png_image_write_to_memory(&image, nullptr, &out_size, Data[0] & 1,
                                  buffer.data(), stride, colormap_ptr);

        if (out_size > 0 && out_size < (1u << 27)) {
          std::vector<uint8_t> out(out_size);
          png_alloc_size_t actual_size = out_size;
          png_image_write_to_memory(&image, out.data(), &actual_size, Data[0] & 1,
                                    buffer.data(), stride, colormap_ptr);
        }

        png_alloc_size_t tiny_size = (Size > 1) ? static_cast<png_alloc_size_t>(Data[1]) : 1;
        if (tiny_size == 0) tiny_size = 1;
        std::vector<uint8_t> tiny(tiny_size);
        png_image_write_to_memory(&image, tiny.data(), &tiny_size, Data[0] & 1,
                                  buffer.data(), stride, colormap_ptr);
      }

      png_image_free(&image);
    } else {
      png_image_free(&image);
    }
  }

  {
    png_image image;
    std::memset(&image, 0, sizeof(image));
    image.version = PNG_IMAGE_VERSION;

    if (png_image_begin_read_from_file(&image, "./dummy_file")) {
      image.format = (Data[0] & 1) ? PNG_FORMAT_RGBA : PNG_FORMAT_RGB;
      png_alloc_size_t buffer_size = PNG_IMAGE_SIZE(image);
      if (buffer_size > 0 && buffer_size < (1u << 26)) {
        std::vector<uint8_t> buffer(buffer_size);
        png_int_32 stride = (Data[1] & 1) ? 0 : static_cast<png_int_32>(PNG_IMAGE_ROW_STRIDE(image));
        png_image_finish_read(&image, nullptr, buffer.data(), stride, nullptr);

        png_alloc_size_t out_size = 0;
        png_image_write_to_memory(&image, nullptr, &out_size, Data[2] & 1,
                                  buffer.data(), 0, nullptr);
        if (out_size > 0 && out_size < (1u << 27)) {
          std::vector<uint8_t> out(out_size);
          png_alloc_size_t actual_size = out_size;
          png_image_write_to_memory(&image, out.data(), &actual_size, Data[2] & 1,
                                    buffer.data(), 0, nullptr);
        }
      }
      png_image_free(&image);
    } else {
      png_image_free(&image);
    }
  }

  {
    FILE *rf = std::fopen("./dummy_file", "rb");
    if (rf != nullptr) {
      png_image image;
      std::memset(&image, 0, sizeof(image));
      image.version = PNG_IMAGE_VERSION;

      if (png_image_begin_read_from_stdio(&image, rf)) {
        image.format = (Data[0] & 2) ? PNG_FORMAT_GA : PNG_FORMAT_GRAY;
        png_alloc_size_t buffer_size = PNG_IMAGE_SIZE(image);
        if (buffer_size > 0 && buffer_size < (1u << 26)) {
          std::vector<uint8_t> buffer(buffer_size);
          png_image_finish_read(&image, nullptr, buffer.data(), 0, nullptr);

          png_alloc_size_t out_size = 0;
          png_image_write_to_memory(&image, nullptr, &out_size, Data[3] & 1,
                                    buffer.data(), 0, nullptr);
          if (out_size > 0 && out_size < (1u << 27)) {
            std::vector<uint8_t> out(out_size);
            png_alloc_size_t actual_size = out_size;
            png_image_write_to_memory(&image, out.data(), &actual_size, Data[3] & 1,
                                      buffer.data(), 0, nullptr);
          }
        }
        png_image_free(&image);
      } else {
        png_image_free(&image);
      }

      std::fclose(rf);
    }
  }

  return 0;
}