// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_get_gAMA at pngget.c:697:1 in png.h
// png_get_tIME at pngget.c:1075:1 in png.h
// png_get_gAMA at pngget.c:697:1 in png.h
// png_get_tIME at pngget.c:1075:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_set_write_fn at pngwio.c:122:1 in png.h
// png_set_IHDR at pngset.c:254:1 in png.h
// png_set_gAMA at pngset.c:200:1 in png.h
// png_set_tIME at pngset.c:968:1 in png.h
// png_write_info at pngwrite.c:192:1 in png.h
// png_set_tIME at pngset.c:968:1 in png.h
// png_get_gAMA at pngget.c:697:1 in png.h
// png_get_tIME at pngget.c:1075:1 in png.h
// png_write_row at pngwrite.c:693:1 in png.h
// png_write_end at pngwrite.c:358:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_set_read_fn at pngrio.c:87:1 in png.h
// png_read_info at pngread.c:92:1 in png.h
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

static void PngWriteCallback(png_structp png_ptr, png_bytep data, png_size_t length) {
  if (png_ptr == nullptr || data == nullptr) return;
  std::vector<unsigned char>* out =
      reinterpret_cast<std::vector<unsigned char>*>(png_get_io_ptr(png_ptr));
  if (out == nullptr) return;
  out->insert(out->end(), data, data + length);
}

static void PngFlushCallback(png_structp) {
}

static void PngReadCallback(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead) {
  if (png_ptr == nullptr || outBytes == nullptr) return;

  png_voidp io_ptr = png_get_io_ptr(png_ptr);
  if (io_ptr == nullptr) {
    png_error(png_ptr, "no io_ptr");
    return;
  }

  struct ReaderState {
    const uint8_t* data;
    size_t size;
    size_t offset;
  };

  ReaderState* st = reinterpret_cast<ReaderState*>(io_ptr);
  if (st->offset > st->size || byteCountToRead > st->size - st->offset) {
    png_error(png_ptr, "read overflow");
    return;
  }

  std::memcpy(outBytes, st->data + st->offset, byteCountToRead);
  st->offset += byteCountToRead;
}

static png_time MakeTimeFromData(const uint8_t* Data, size_t Size, size_t base) {
  png_time t;
  uint8_t a = Size > base + 0 ? Data[base + 0] : 0;
  uint8_t b = Size > base + 1 ? Data[base + 1] : 0;
  uint8_t c = Size > base + 2 ? Data[base + 2] : 0;
  uint8_t d = Size > base + 3 ? Data[base + 3] : 0;
  uint8_t e = Size > base + 4 ? Data[base + 4] : 0;
  uint8_t f = Size > base + 5 ? Data[base + 5] : 0;

  t.year = static_cast<png_uint_16>(1900 + ((static_cast<unsigned>(a) << 8) | b) % 300);
  t.month = static_cast<png_byte>((c % 15));
  t.day = static_cast<png_byte>((d % 35));
  t.hour = static_cast<png_byte>((e % 30));
  t.minute = static_cast<png_byte>((f % 70));
  t.second = static_cast<png_byte>(((a ^ b ^ c ^ d ^ e ^ f) % 70));
  return t;
}

extern "C" int LLVMFuzzerTestOneInput_30(const uint8_t *Data, size_t Size) {
  {
    FILE* fp = std::fopen("./dummy_file", "wb");
    if (fp != nullptr) {
      if (Data != nullptr && Size > 0) std::fwrite(Data, 1, Size, fp);
      std::fclose(fp);
    }
  }

  if (Data == nullptr) return 0;

  // Reading path: exercise png_read_info, png_get_gAMA, png_get_tIME.
  {
    struct ReaderState {
      const uint8_t* data;
      size_t size;
      size_t offset;
    } reader{Data, Size, 0};

    png_structp read_ptr =
        png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (read_ptr != nullptr) {
      png_infop read_info = png_create_info_struct(read_ptr);
      if (read_info != nullptr) {
        if (setjmp(png_jmpbuf(read_ptr)) == 0) {
          png_set_read_fn(read_ptr, &reader, PngReadCallback);
          png_read_info(read_ptr, read_info);

          double gamma = 0.0;
          (void)png_get_gAMA(read_ptr, read_info, &gamma);

          png_timep mod_time = nullptr;
          (void)png_get_tIME(read_ptr, read_info, &mod_time);

          double gamma2 = -1.0;
          (void)png_get_gAMA(read_ptr, read_info, &gamma2);

          png_timep mod_time2 = nullptr;
          (void)png_get_tIME(read_ptr, read_info, &mod_time2);
        }
        png_destroy_read_struct(&read_ptr, &read_info, nullptr);
      } else {
        png_destroy_read_struct(&read_ptr, nullptr, nullptr);
      }
    }
  }

  // Writing path: exercise png_set_tIME, png_write_info, png_write_end, png_get_gAMA, png_get_tIME.
  {
    png_structp write_ptr =
        png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (write_ptr != nullptr) {
      png_infop write_info = png_create_info_struct(write_ptr);
      if (write_info != nullptr) {
        std::vector<unsigned char> outbuf;
        if (setjmp(png_jmpbuf(write_ptr)) == 0) {
          png_set_write_fn(write_ptr, &outbuf, PngWriteCallback, PngFlushCallback);

          int color_type = (Size > 0 && (Data[0] & 1)) ? PNG_COLOR_TYPE_RGB : PNG_COLOR_TYPE_GRAY;
          int bit_depth = (Size > 1 && (Data[1] & 1)) ? 8 : 8;
          png_uint_32 width = 1;
          png_uint_32 height = 1;

          png_set_IHDR(write_ptr, write_info, width, height, bit_depth, color_type,
                       PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

          if (Size > 2) {
            double gamma = ((Data[2] % 200) + 1) / 100.0;
            png_set_gAMA(write_ptr, write_info, gamma);
          }

          png_time t1 = MakeTimeFromData(Data, Size, 3);
          png_set_tIME(write_ptr, write_info, &t1);

          png_write_info(write_ptr, write_info);

          png_time t2 = MakeTimeFromData(Data, Size, 9);
          png_set_tIME(write_ptr, write_info, &t2);

          double out_gamma = 0.0;
          (void)png_get_gAMA(write_ptr, write_info, &out_gamma);

          png_timep out_time = nullptr;
          (void)png_get_tIME(write_ptr, write_info, &out_time);

          if (Size > 0 && (Data[0] & 2)) {
            png_bytep row = static_cast<png_bytep>(std::malloc(color_type == PNG_COLOR_TYPE_RGB ? 3 : 1));
            if (row != nullptr) {
              if (color_type == PNG_COLOR_TYPE_RGB) {
                row[0] = Size > 3 ? Data[3] : 0;
                row[1] = Size > 4 ? Data[4] : 0;
                row[2] = Size > 5 ? Data[5] : 0;
              } else {
                row[0] = Size > 3 ? Data[3] : 0;
              }
              png_write_row(write_ptr, row);
              std::free(row);
            }
          }

          png_write_end(write_ptr, write_info);
        }
        png_destroy_write_struct(&write_ptr, &write_info);
      } else {
        png_destroy_write_struct(&write_ptr, nullptr);
      }
    }
  }

  return 0;
}