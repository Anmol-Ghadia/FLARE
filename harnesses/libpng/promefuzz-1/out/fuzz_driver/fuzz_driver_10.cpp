// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_image_begin_read_from_file at pngread.c:1502:1 in png.h
// png_image_finish_read at pngread.c:4108:1 in png.h
// png_image_write_to_memory at pngwrite.c:2207:1 in png.h
// png_image_write_to_memory at pngwrite.c:2207:1 in png.h
// png_image_free at png.c:4582:1 in png.h
// png_image_free at png.c:4582:1 in png.h
// png_image_begin_read_from_stdio at pngread.c:1472:1 in png.h
// png_image_finish_read at pngread.c:4108:1 in png.h
// png_image_write_to_memory at pngwrite.c:2207:1 in png.h
// png_image_write_to_memory at pngwrite.c:2207:1 in png.h
// png_image_free at png.c:4582:1 in png.h
// png_image_free at png.c:4582:1 in png.h
// png_image_begin_read_from_memory at pngread.c:1570:12 in png.h
// png_image_finish_read at pngread.c:4108:1 in png.h
// png_image_write_to_memory at pngwrite.c:2207:1 in png.h
// png_image_write_to_memory at pngwrite.c:2207:1 in png.h
// png_image_write_to_memory at pngwrite.c:2207:1 in png.h
// png_image_free at png.c:4582:1 in png.h
// png_image_free at png.c:4582:1 in png.h
#include <png.h>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>

static uint32_t ReadU32(const uint8_t *data, size_t size, size_t offset) {
  if (offset + 4 > size) return 0;
  return (static_cast<uint32_t>(data[offset]) << 24) |
         (static_cast<uint32_t>(data[offset + 1]) << 16) |
         (static_cast<uint32_t>(data[offset + 2]) << 8) |
         (static_cast<uint32_t>(data[offset + 3]));
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  if (Data == nullptr || Size == 0) return 0;

  FILE *fp = std::fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    std::fwrite(Data, 1, Size, fp);
    std::fclose(fp);
  }

  png_color background;
  std::memset(&background, 0, sizeof(background));
  if (Size > 0) background.red = Data[0];
  if (Size > 1) background.green = Data[1];
  if (Size > 2) background.blue = Data[2];

  png_color cmap[256];
  std::memset(cmap, 0, sizeof(cmap));
  for (size_t i = 0; i < 256; ++i) {
    cmap[i].red = static_cast<png_byte>((i < Size) ? Data[i] : i);
    cmap[i].green = static_cast<png_byte>((i + 1 < Size) ? Data[i + 1] : i);
    cmap[i].blue = static_cast<png_byte>((i + 2 < Size) ? Data[i + 2] : i);
  }

  {
    png_image image;
    std::memset(&image, 0, sizeof(image));
    image.version = PNG_IMAGE_VERSION;

    if (png_image_begin_read_from_memory(&image, Data, Size)) {
      static const png_uint_32 kFormats[] = {
          PNG_FORMAT_GRAY,
          PNG_FORMAT_GA,
          PNG_FORMAT_RGB,
          PNG_FORMAT_RGBA,
          PNG_FORMAT_LINEAR_Y,
          PNG_FORMAT_LINEAR_Y_ALPHA,
          PNG_FORMAT_LINEAR_RGB,
          PNG_FORMAT_RGB_COLORMAP,
          PNG_FORMAT_RGBA_COLORMAP,
      };
      const size_t kNumFormats = sizeof(kFormats) / sizeof(kFormats[0]);
      image.format = kFormats[Data[0] % kNumFormats];

      png_alloc_size_t buffer_size = PNG_IMAGE_SIZE(image);
      if (buffer_size > 0 && buffer_size < (1u << 26)) {
        std::vector<uint8_t> buffer(buffer_size);
        png_int_32 stride = 0;
        if (Size >= 8) {
          stride = static_cast<png_int_32>(ReadU32(Data, Size, 4));
          if (Data[1] & 1) stride = -stride;
        }

        void *colormap_ptr = ((Data[2] & 1) != 0) ? static_cast<void *>(cmap) : nullptr;
        png_const_colorp bg_ptr = ((Data[3] & 1) != 0) ? &background : nullptr;

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
        png_int_32 stride = (Size > 1 && (Data[1] & 1)) ? 0 : static_cast<png_int_32>(PNG_IMAGE_ROW_STRIDE(image));
        png_image_finish_read(&image, nullptr, buffer.data(), stride, nullptr);

        png_alloc_size_t out_size = 0;
        png_image_write_to_memory(&image, nullptr, &out_size, (Size > 2) ? (Data[2] & 1) : 0,
                                  buffer.data(), 0, nullptr);
        if (out_size > 0 && out_size < (1u << 27)) {
          std::vector<uint8_t> out(out_size);
          png_alloc_size_t actual_size = out_size;
          png_image_write_to_memory(&image, out.data(), &actual_size, (Size > 2) ? (Data[2] & 1) : 0,
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
          png_image_write_to_memory(&image, nullptr, &out_size, (Size > 3) ? (Data[3] & 1) : 0,
                                    buffer.data(), 0, nullptr);
          if (out_size > 0 && out_size < (1u << 27)) {
            std::vector<uint8_t> out(out_size);
            png_alloc_size_t actual_size = out_size;
            png_image_write_to_memory(&image, out.data(), &actual_size, (Size > 3) ? (Data[3] & 1) : 0,
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