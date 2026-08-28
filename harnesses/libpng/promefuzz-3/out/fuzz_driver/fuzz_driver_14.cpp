// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_image_begin_read_from_file at pngread.c:1502:1 in png.h
// png_image_finish_read at pngread.c:4108:1 in png.h
// png_image_finish_read at pngread.c:4108:1 in png.h
// png_image_free at png.c:4582:1 in png.h
// png_image_begin_read_from_stdio at pngread.c:1472:1 in png.h
// png_image_finish_read at pngread.c:4108:1 in png.h
// png_image_finish_read at pngread.c:4108:1 in png.h
// png_image_free at png.c:4582:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_init_io at png.c:703:1 in png.h
// png_set_sig_bytes at png.c:47:1 in png.h
// png_read_info at pngread.c:92:1 in png.h
// png_set_interlace_handling at pngtrans.c:99:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_sig_bytes at png.c:47:1 in png.h
// png_image_begin_read_from_memory at pngread.c:1570:12 in png.h
// png_image_begin_read_from_memory at pngread.c:1570:12 in png.h
// png_image_finish_read at pngread.c:4108:1 in png.h
// png_image_finish_read at pngread.c:4108:1 in png.h
// png_image_free at png.c:4582:1 in png.h
// png_image_free at png.c:4582:1 in png.h
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
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <stdint.h>

static size_t MinSize(size_t a, size_t b) {
  return a < b ? a : b;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  if (Data == nullptr) return 0;

  if (Size > 0) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != nullptr) {
      fwrite(Data, 1, Size, fp);
      fclose(fp);
    }
  } else {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != nullptr) fclose(fp);
  }

  if (Size >= 1) {
    png_image image;
    memset(&image, 0, sizeof(image));
    image.version = PNG_IMAGE_VERSION;

    if (png_image_begin_read_from_memory(&image, Data, Size)) {
      png_uint_32 width = image.width;
      png_uint_32 height = image.height;

      unsigned int formats[] = {
          PNG_FORMAT_GRAY,
          PNG_FORMAT_GA,
          PNG_FORMAT_RGB,
          PNG_FORMAT_RGBA,
          PNG_FORMAT_LINEAR_RGB,
          PNG_FORMAT_LINEAR_RGB_ALPHA,
          PNG_FORMAT_RGB_COLORMAP,
          PNG_FORMAT_RGBA_COLORMAP
      };

      for (size_t i = 0; i < sizeof(formats) / sizeof(formats[0]); ++i) {
        png_image img2;
        memset(&img2, 0, sizeof(img2));
        img2.version = PNG_IMAGE_VERSION;

        if (!png_image_begin_read_from_memory(&img2, Data, Size))
          continue;

        img2.format = formats[i];
        png_alloc_size_t buf_size = 0;
        if (width != 0 && height != 0)
          buf_size = PNG_IMAGE_SIZE(img2);

        std::vector<unsigned char> buffer;
        if (buf_size > 0 && buf_size < (1u << 26))
          buffer.resize(buf_size);

        png_color background;
        memset(&background, 0, sizeof(background));
        if (Size >= 4) {
          background.red = Data[0];
          background.green = Data[1];
          background.blue = Data[2];
        }

        png_int_32 strides[3] = {
            0,
            (img2.width > 0) ? (png_int_32)PNG_IMAGE_ROW_STRIDE(img2) : 0,
            (img2.width > 0) ? -(png_int_32)PNG_IMAGE_ROW_STRIDE(img2) : 0
        };

        for (int s = 0; s < 3; ++s) {
          if (!buffer.empty()) {
            png_image img3 = img2;
            png_image_finish_read(&img3, &background, buffer.data(), strides[s], nullptr);
          } else {
            png_image img3 = img2;
            png_image_finish_read(&img3, &background, nullptr, strides[s], nullptr);
          }
        }

        png_image_free(&img2);
      }

      png_image_free(&image);
    }
  }

  {
    png_image image;
    memset(&image, 0, sizeof(image));
    image.version = PNG_IMAGE_VERSION;
    if (png_image_begin_read_from_file(&image, "./dummy_file")) {
      image.format = PNG_FORMAT_RGBA;
      png_alloc_size_t buf_size = PNG_IMAGE_SIZE(image);
      if (buf_size > 0 && buf_size < (1u << 26)) {
        std::vector<unsigned char> buffer(buf_size);
        png_image_finish_read(&image, nullptr, buffer.data(), 0, nullptr);
      } else {
        png_image_finish_read(&image, nullptr, nullptr, 0, nullptr);
      }
      png_image_free(&image);
    }
  }

  {
    FILE *fp = fopen("./dummy_file", "rb");
    if (fp != nullptr) {
      png_image image;
      memset(&image, 0, sizeof(image));
      image.version = PNG_IMAGE_VERSION;
      if (png_image_begin_read_from_stdio(&image, fp)) {
        image.format = PNG_FORMAT_RGB;
        png_alloc_size_t buf_size = PNG_IMAGE_SIZE(image);
        if (buf_size > 0 && buf_size < (1u << 26)) {
          std::vector<unsigned char> buffer(buf_size);
          png_image_finish_read(&image, nullptr, buffer.data(), 0, nullptr);
        } else {
          png_image_finish_read(&image, nullptr, nullptr, 0, nullptr);
        }
        png_image_free(&image);
      } else {
        fclose(fp);
      }
    }
  }

  {
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    png_infop info_ptr = nullptr;
    if (png_ptr != nullptr) {
      info_ptr = png_create_info_struct(png_ptr);
      if (info_ptr != nullptr) {
        if (setjmp(png_jmpbuf(png_ptr)) == 0) {
          FILE *fp = fopen("./dummy_file", "rb");
          if (fp != nullptr) {
            png_init_io(png_ptr, fp);

            int sig = 0;
            if (Size > 0)
              sig = static_cast<int>(Data[0] % 10) - 1; // explores negative, valid, and >8
            png_set_sig_bytes(png_ptr, sig);

            png_read_info(png_ptr, info_ptr);
            (void)png_set_interlace_handling(png_ptr);
          }
        }
      }
      png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    }
  }

  {
    png_set_sig_bytes(nullptr, Size ? static_cast<int>(Data[0]) : 0);
  }

  return 0;
}