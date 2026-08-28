// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_IHDR at pngset.c:254:1 in png.h
// png_set_PLTE at pngset.c:572:1 in png.h
// png_set_rows at pngset.c:1546:1 in png.h
// png_write_info at pngwrite.c:192:1 in png.h
// png_write_row at pngwrite.c:693:1 in png.h
// png_write_end at pngwrite.c:358:1 in png.h
// png_write_png at pngwrite.c:1338:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_set_gray_to_rgb at pngrtran.c:955:1 in png.h
// png_set_packing at pngtrans.c:50:1 in png.h
// png_set_packswap at pngtrans.c:70:1 in png.h
// png_set_gray_to_rgb at pngrtran.c:955:1 in png.h
// png_set_packing at pngtrans.c:50:1 in png.h
// png_set_packswap at pngtrans.c:70:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_set_write_fn at pngwio.c:122:1 in png.h
// png_set_packing at pngtrans.c:50:1 in png.h
// png_set_packswap at pngtrans.c:70:1 in png.h
// png_set_sig_bytes at png.c:47:1 in png.h
// png_write_sig at pngwutil.c:51:1 in png.h
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

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

static void WriteDataFn(png_structp png_ptr, png_bytep data, png_size_t length) {
  FILE* fp = static_cast<FILE*>(png_get_io_ptr(png_ptr));
  if (fp != nullptr && data != nullptr && length > 0) {
    (void)fwrite(data, 1, length, fp);
  }
}

static void FlushDataFn(png_structp png_ptr) {
  FILE* fp = static_cast<FILE*>(png_get_io_ptr(png_ptr));
  if (fp != nullptr) {
    (void)fflush(fp);
  }
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE* fp = fopen("./dummy_file", "wb+");
  if (fp == nullptr) {
    return 0;
  }

  if (Size > 0) {
    (void)fwrite(Data, 1, Size, fp);
    (void)fflush(fp);
  }
  rewind(fp);

  png_structp read_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn, PngWarningFn);
  png_infop read_info = nullptr;

  if (read_ptr != nullptr) {
    read_info = png_create_info_struct(read_ptr);
    if (read_info != nullptr) {
      if (setjmp(png_jmpbuf(read_ptr)) == 0) {
        png_set_gray_to_rgb(read_ptr);
        png_set_packing(read_ptr);
        png_set_packswap(read_ptr);

        if (Size & 1) {
          png_set_gray_to_rgb(read_ptr);
        }
        if (Size & 2) {
          png_set_packing(read_ptr);
        }
        if (Size & 4) {
          png_set_packswap(read_ptr);
        }
      }
    }
    png_destroy_read_struct(&read_ptr, &read_info, nullptr);
  }

  png_structp write_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn, PngWarningFn);
  png_infop write_info = nullptr;

  if (write_ptr != nullptr) {
    write_info = png_create_info_struct(write_ptr);
    if (write_info != nullptr) {
      if (setjmp(png_jmpbuf(write_ptr)) == 0) {
        png_set_write_fn(write_ptr, fp, WriteDataFn, FlushDataFn);

        png_set_packing(write_ptr);
        png_set_packswap(write_ptr);

        png_byte sig_bytes = (Size > 0) ? static_cast<png_byte>(Data[0] % 8) : 0;
        png_set_sig_bytes(write_ptr, sig_bytes);
        png_write_sig(write_ptr);

        png_uint_32 width = 1;
        png_uint_32 height = 1;
        if (Size > 1) width = static_cast<png_uint_32>((Data[1] % 8) + 1);
        if (Size > 2) height = static_cast<png_uint_32>((Data[2] % 8) + 1);

        int color_type = PNG_COLOR_TYPE_GRAY;
        if (Size > 3) {
          switch (Data[3] % 4) {
            case 0: color_type = PNG_COLOR_TYPE_GRAY; break;
            case 1: color_type = PNG_COLOR_TYPE_RGB; break;
            case 2: color_type = PNG_COLOR_TYPE_PALETTE; break;
            default: color_type = PNG_COLOR_TYPE_GRAY_ALPHA; break;
          }
        }

        int bit_depths[] = {1, 2, 4, 8, 16};
        int bit_depth = bit_depths[(Size > 4) ? (Data[4] % 5) : 0];

        if (color_type == PNG_COLOR_TYPE_RGB && bit_depth < 8) {
          bit_depth = 8;
        }
        if (color_type == PNG_COLOR_TYPE_GRAY_ALPHA && bit_depth < 8) {
          bit_depth = 8;
        }
        if (color_type == PNG_COLOR_TYPE_PALETTE && bit_depth == 16) {
          bit_depth = 8;
        }

        png_set_IHDR(write_ptr, write_info, width, height, bit_depth, color_type,
                     PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

        png_size_t channels = 1;
        switch (color_type) {
          case PNG_COLOR_TYPE_GRAY: channels = 1; break;
          case PNG_COLOR_TYPE_PALETTE: channels = 1; break;
          case PNG_COLOR_TYPE_RGB: channels = 3; break;
          case PNG_COLOR_TYPE_GRAY_ALPHA: channels = 2; break;
          case PNG_COLOR_TYPE_RGB_ALPHA: channels = 4; break;
          default: channels = 1; break;
        }

        png_size_t bytes_per_sample = (bit_depth <= 8) ? 1 : 2;
        png_size_t rowbytes = width * channels * bytes_per_sample;
        if (rowbytes == 0) rowbytes = 1;

        std::vector<png_byte> row(rowbytes);
        for (png_size_t i = 0; i < rowbytes; ++i) {
          row[i] = (Size > 0) ? Data[i % Size] : 0;
        }

        std::vector<png_bytep> rows(height);
        std::vector<png_byte> image(rowbytes * height);
        for (png_uint_32 y = 0; y < height; ++y) {
          for (png_size_t x = 0; x < rowbytes; ++x) {
            image[y * rowbytes + x] = (Size > 0) ? Data[(y * rowbytes + x) % Size] : 0;
          }
          rows[y] = image.data() + y * rowbytes;
        }

        if (color_type == PNG_COLOR_TYPE_PALETTE) {
          png_color palette[256];
          for (int i = 0; i < 256; ++i) {
            palette[i].red = static_cast<png_byte>(i);
            palette[i].green = static_cast<png_byte>(255 - i);
            palette[i].blue = static_cast<png_byte>(i ^ 0xAA);
          }
          png_set_PLTE(write_ptr, write_info, palette, 256);
        }

        png_set_rows(write_ptr, write_info, rows.data());

        if (Size & 8) {
          png_write_info(write_ptr, write_info);
          for (png_uint_32 y = 0; y < height; ++y) {
            png_write_row(write_ptr, rows[y]);
          }
          png_write_end(write_ptr, write_info);
        } else {
          int transforms = 0;
          if (Size > 5) {
            if (Data[5] & 0x01) transforms |= PNG_TRANSFORM_PACKING;
            if (Data[5] & 0x02) transforms |= PNG_TRANSFORM_PACKSWAP;
            if (Data[5] & 0x04) transforms |= PNG_TRANSFORM_SWAP_ENDIAN;
            if (Data[5] & 0x08) transforms |= PNG_TRANSFORM_INVERT_ALPHA;
            if (Data[5] & 0x10) transforms |= PNG_TRANSFORM_BGR;
            if (Data[5] & 0x20) transforms |= PNG_TRANSFORM_SWAP_ALPHA;
          }
          png_write_png(write_ptr, write_info, transforms, nullptr);
        }
      }
    }
    png_destroy_write_struct(&write_ptr, &write_info);
  }

  fclose(fp);
  remove("./dummy_file");
  return 0;
}