// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_tRNS_to_alpha at pngrtran.c:926:1 in png.h
// png_set_expand_16 at pngrtran.c:942:1 in png.h
// png_set_gray_to_rgb at pngrtran.c:955:1 in png.h
// png_set_strip_alpha at pngrtran.c:210:1 in png.h
// png_set_swap_alpha at pngtrans.c:223:1 in png.h
// png_set_expand at pngrtran.c:872:1 in png.h
// png_set_expand at pngrtran.c:872:1 in png.h
// png_set_tRNS_to_alpha at pngrtran.c:926:1 in png.h
// png_set_gray_to_rgb at pngrtran.c:955:1 in png.h
// png_set_expand_16 at pngrtran.c:942:1 in png.h
// png_set_strip_alpha at pngrtran.c:210:1 in png.h
// png_set_swap_alpha at pngtrans.c:223:1 in png.h
// png_set_gray_to_rgb at pngrtran.c:955:1 in png.h
// png_set_expand at pngrtran.c:872:1 in png.h
// png_set_expand_16 at pngrtran.c:942:1 in png.h
// png_set_strip_alpha at pngrtran.c:210:1 in png.h
// png_set_tRNS_to_alpha at pngrtran.c:926:1 in png.h
// png_set_swap_alpha at pngtrans.c:223:1 in png.h
// png_set_swap_alpha at pngtrans.c:223:1 in png.h
// png_set_strip_alpha at pngrtran.c:210:1 in png.h
// png_set_expand at pngrtran.c:872:1 in png.h
// png_set_tRNS_to_alpha at pngrtran.c:926:1 in png.h
// png_set_tRNS_to_alpha at pngrtran.c:926:1 in png.h
// png_set_expand_16 at pngrtran.c:942:1 in png.h
// png_set_gray_to_rgb at pngrtran.c:955:1 in png.h
// png_set_expand at pngrtran.c:872:1 in png.h
// png_set_expand_16 at pngrtran.c:942:1 in png.h
// png_set_expand at pngrtran.c:872:1 in png.h
// png_set_strip_alpha at pngrtran.c:210:1 in png.h
// png_set_swap_alpha at pngtrans.c:223:1 in png.h
// png_set_gray_to_rgb at pngrtran.c:955:1 in png.h
// png_read_info at pngread.c:92:1 in png.h
// png_set_tRNS_to_alpha at pngrtran.c:926:1 in png.h
// png_set_expand_16 at pngrtran.c:942:1 in png.h
// png_set_gray_to_rgb at pngrtran.c:955:1 in png.h
// png_set_strip_alpha at pngrtran.c:210:1 in png.h
// png_set_swap_alpha at pngtrans.c:223:1 in png.h
// png_set_expand at pngrtran.c:872:1 in png.h
// png_set_expand at pngrtran.c:872:1 in png.h
// png_set_tRNS_to_alpha at pngrtran.c:926:1 in png.h
// png_set_gray_to_rgb at pngrtran.c:955:1 in png.h
// png_set_expand_16 at pngrtran.c:942:1 in png.h
// png_set_strip_alpha at pngrtran.c:210:1 in png.h
// png_set_swap_alpha at pngtrans.c:223:1 in png.h
// png_read_update_info at pngread.c:267:1 in png.h
// png_get_IHDR at pngget.c:825:1 in png.h
// png_get_rowbytes at pngget.c:30:1 in png.h
// png_read_row at pngread.c:383:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_read_end at pngread.c:766:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_set_write_fn at pngwio.c:122:1 in png.h
// png_set_swap_alpha at pngtrans.c:223:1 in png.h
// png_set_IHDR at pngset.c:254:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_write_info at pngwrite.c:192:1 in png.h
// png_write_row at pngwrite.c:693:1 in png.h
// png_write_end at pngwrite.c:358:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_read_fn at pngrio.c:87:1 in png.h
// png_set_sig_bytes at png.c:47:1 in png.h
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
#include <setjmp.h>

static void DummyReadData(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead) {
  png_voidp io_ptr = png_get_io_ptr(png_ptr);
  if (io_ptr == NULL) {
    png_error(png_ptr, "No IO ptr");
    return;
  }

  struct BufferState {
    const uint8_t* data;
    size_t size;
    size_t offset;
  };

  BufferState* st = reinterpret_cast<BufferState*>(io_ptr);
  if (st->offset > st->size || byteCountToRead > st->size - st->offset) {
    png_error(png_ptr, "Read overflow");
    return;
  }

  memcpy(outBytes, st->data + st->offset, byteCountToRead);
  st->offset += byteCountToRead;
}

static void DummyWriteData(png_structp png_ptr, png_bytep data, png_size_t length) {
  (void)png_ptr;
  (void)data;
  (void)length;
}

static void DummyFlush(png_structp png_ptr) {
  (void)png_ptr;
}

extern "C" int LLVMFuzzerTestOneInput_51(const uint8_t *Data, size_t Size) {
  struct BufferState {
    const uint8_t* data;
    size_t size;
    size_t offset;
  };

  BufferState st = {Data, Size, 0};

  FILE* fp = fopen("./dummy_file", "wb");
  if (fp != NULL) {
    if (Size > 0) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (png_ptr == NULL) {
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == NULL) {
    png_destroy_read_struct(&png_ptr, NULL, NULL);
    return 0;
  }

  png_infop end_info = png_create_info_struct(png_ptr);
  if (end_info == NULL) {
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    return 0;
  }

  png_set_read_fn(png_ptr, &st, DummyReadData);

  if (Size > 0) {
    png_byte sig_bytes = (Size >= 8) ? 8 : (png_byte)Size;
    if (sig_bytes > 0) {
      png_set_sig_bytes(png_ptr, 0);
    }
  }

  switch (Size > 0 ? (Data[0] % 8) : 0) {
    case 0:
      png_set_tRNS_to_alpha(png_ptr);
      png_set_expand_16(png_ptr);
      png_set_gray_to_rgb(png_ptr);
      png_set_strip_alpha(png_ptr);
      png_set_swap_alpha(png_ptr);
      png_set_expand(png_ptr);
      break;
    case 1:
      png_set_expand(png_ptr);
      png_set_tRNS_to_alpha(png_ptr);
      png_set_gray_to_rgb(png_ptr);
      break;
    case 2:
      png_set_expand_16(png_ptr);
      png_set_strip_alpha(png_ptr);
      png_set_swap_alpha(png_ptr);
      break;
    case 3:
      png_set_gray_to_rgb(png_ptr);
      png_set_expand(png_ptr);
      png_set_expand_16(png_ptr);
      break;
    case 4:
      png_set_strip_alpha(png_ptr);
      png_set_tRNS_to_alpha(png_ptr);
      png_set_swap_alpha(png_ptr);
      break;
    case 5:
      png_set_swap_alpha(png_ptr);
      png_set_strip_alpha(png_ptr);
      png_set_expand(png_ptr);
      break;
    case 6:
      png_set_tRNS_to_alpha(png_ptr);
      png_set_tRNS_to_alpha(png_ptr);
      png_set_expand_16(png_ptr);
      png_set_gray_to_rgb(png_ptr);
      break;
    case 7:
      png_set_expand(png_ptr);
      png_set_expand_16(png_ptr);
      png_set_expand(png_ptr);
      png_set_strip_alpha(png_ptr);
      png_set_swap_alpha(png_ptr);
      png_set_gray_to_rgb(png_ptr);
      break;
  }

  png_read_info(png_ptr, info_ptr);

  switch (Size > 1 ? (Data[1] % 8) : 0) {
    case 0:
      png_set_tRNS_to_alpha(png_ptr);
      break;
    case 1:
      png_set_expand_16(png_ptr);
      break;
    case 2:
      png_set_gray_to_rgb(png_ptr);
      break;
    case 3:
      png_set_strip_alpha(png_ptr);
      break;
    case 4:
      png_set_swap_alpha(png_ptr);
      break;
    case 5:
      png_set_expand(png_ptr);
      break;
    case 6:
      png_set_expand(png_ptr);
      png_set_tRNS_to_alpha(png_ptr);
      png_set_gray_to_rgb(png_ptr);
      break;
    case 7:
      png_set_expand_16(png_ptr);
      png_set_strip_alpha(png_ptr);
      png_set_swap_alpha(png_ptr);
      break;
  }

  png_read_update_info(png_ptr, info_ptr);

  png_uint_32 width = 0, height = 0;
  int bit_depth = 0, color_type = 0, interlace_type = 0, compression_type = 0, filter_method = 0;
  png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
               &interlace_type, &compression_type, &filter_method);

  png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);
  if (rowbytes > 0 && height > 0 && rowbytes < (1u << 20) && height < 4096) {
    png_bytep row = (png_bytep)malloc(rowbytes);
    if (row != NULL) {
      png_uint_32 rows_to_read = height;
      if (rows_to_read > 32) rows_to_read = 32;
      for (png_uint_32 y = 0; y < rows_to_read; ++y) {
        png_read_row(png_ptr, row, NULL);
      }
      free(row);
    }
  }

  if (!setjmp(png_jmpbuf(png_ptr))) {
    png_read_end(png_ptr, end_info);
  }

  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);

  png_structp write_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (write_ptr != NULL) {
    png_infop write_info = png_create_info_struct(write_ptr);
    if (write_info != NULL) {
      if (!setjmp(png_jmpbuf(write_ptr))) {
        png_set_write_fn(write_ptr, NULL, DummyWriteData, DummyFlush);
        png_set_swap_alpha(write_ptr);

        png_uint_32 w = (Size > 2 ? Data[2] : 1) + 1;
        png_uint_32 h = (Size > 3 ? Data[3] : 1) + 1;
        int ct = (Size > 4 && (Data[4] & 1)) ? PNG_COLOR_TYPE_RGBA : PNG_COLOR_TYPE_RGB;
        png_set_IHDR(write_ptr, write_info, w, h, 8, ct,
                     PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
        png_write_info(write_ptr, write_info);

        png_size_t wb = (ct == PNG_COLOR_TYPE_RGBA) ? (size_t)w * 4 : (size_t)w * 3;
        if (wb > 0 && wb < (1u << 20)) {
          png_bytep row = (png_bytep)malloc(wb);
          if (row != NULL) {
            memset(row, 0, wb);
            png_uint_32 rows = h > 8 ? 8 : h;
            for (png_uint_32 y = 0; y < rows; ++y) {
              if (Size > 5) {
                for (png_size_t i = 0; i < wb; ++i) {
                  row[i] = Data[(5 + i + y) % Size];
                }
              }
              png_write_row(write_ptr, row);
            }
            free(row);
          }
        }

        png_write_end(write_ptr, write_info);
      }
      png_destroy_write_struct(&write_ptr, &write_info);
    } else {
      png_destroy_write_struct(&write_ptr, NULL);
    }
  }

  return 0;
}