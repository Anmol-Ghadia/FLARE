// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_set_read_fn at pngrio.c:87:1 in png.h
// png_set_sig_bytes at png.c:47:1 in png.h
// png_set_sig_bytes at png.c:47:1 in png.h
// png_set_option at png.c:4332:1 in png.h
// png_set_option at png.c:4332:1 in png.h
// png_reset_zstream at png.c:974:1 in png.h
// png_read_info at pngread.c:92:1 in png.h
// png_set_interlace_handling at pngtrans.c:99:1 in png.h
// png_read_end at pngread.c:766:1 in png.h
// png_reset_zstream at png.c:974:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_set_read_fn at pngrio.c:87:1 in png.h
// png_set_sig_bytes at png.c:47:1 in png.h
// png_set_sig_bytes at png.c:47:1 in png.h
// png_set_sig_bytes at png.c:47:1 in png.h
// png_set_option at png.c:4332:1 in png.h
// png_set_option at png.c:4332:1 in png.h
// png_read_info at pngread.c:92:1 in png.h
// png_set_interlace_handling at pngtrans.c:99:1 in png.h
// png_read_end at pngread.c:766:1 in png.h
// png_reset_zstream at png.c:974:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_get_IHDR at pngget.c:825:1 in png.h
// png_read_update_info at pngread.c:267:1 in png.h
// png_get_rowbytes at pngget.c:30:1 in png.h
// png_set_interlace_handling at pngtrans.c:99:1 in png.h
// png_read_row at pngread.c:383:1 in png.h
// png_set_sig_bytes at png.c:47:1 in png.h
// png_reset_zstream at png.c:974:1 in png.h
// png_set_option at png.c:4332:1 in png.h
// png_set_interlace_handling at pngtrans.c:99:1 in png.h
// png_read_info at pngread.c:92:1 in png.h
// png_read_end at pngread.c:766:1 in png.h
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

static void FuzzReadData(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead) {
  png_voidp io_ptr = png_get_io_ptr(png_ptr);
  if (io_ptr == NULL) {
    png_error(png_ptr, "no io_ptr");
    return;
  }

  struct BufferState {
    const uint8_t* data;
    size_t size;
    size_t offset;
  };

  BufferState* state = reinterpret_cast<BufferState*>(io_ptr);
  if (state->offset > state->size || byteCountToRead > state->size - state->offset) {
    png_error(png_ptr, "read beyond end");
    return;
  }

  if (byteCountToRead > 0) {
    memcpy(outBytes, state->data + state->offset, byteCountToRead);
    state->offset += byteCountToRead;
  }
}

static void FuzzReadDataFile(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead) {
  FILE* fp = reinterpret_cast<FILE*>(png_get_io_ptr(png_ptr));
  if (fp == NULL) {
    png_error(png_ptr, "no file");
    return;
  }

  if (byteCountToRead == 0) return;

  size_t n = fread(outBytes, 1, byteCountToRead, fp);
  if (n != byteCountToRead) {
    png_error(png_ptr, "short read");
  }
}

static void ConsumeRows(png_structp png_ptr, png_infop info_ptr) {
  png_uint_32 width = 0, height = 0;
  int bit_depth = 0, color_type = 0, interlace_type = 0, compression_type = 0, filter_method = 0;

  png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
               &interlace_type, &compression_type, &filter_method);

  png_read_update_info(png_ptr, info_ptr);

  png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);
  if (rowbytes == 0) rowbytes = 1;

  png_bytep row = reinterpret_cast<png_bytep>(malloc(rowbytes));
  if (row == NULL) return;

  int passes = png_set_interlace_handling(png_ptr);
  if (passes <= 0) passes = 1;

  for (int p = 0; p < passes; ++p) {
    for (png_uint_32 y = 0; y < height; ++y) {
      png_read_row(png_ptr, row, NULL);
    }
  }

  free(row);
}

extern "C" int LLVMFuzzerTestOneInput_54(const uint8_t *Data, size_t Size) {
  if (Data == NULL) return 0;

  {
    png_set_sig_bytes(NULL, static_cast<int>(Size & 0xFF));
    (void)png_reset_zstream(NULL);
    (void)png_set_option(NULL, static_cast<int>(Size & 0x7fffffff), (Size & 1) ? 1 : 0);
    (void)png_set_interlace_handling(NULL);
    png_read_info(NULL, NULL);
    png_read_end(NULL, NULL);
  }

  struct BufferState {
    const uint8_t* data;
    size_t size;
    size_t offset;
  };

  BufferState state = {Data, Size, 0};

  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (png_ptr != NULL) {
    png_infop info_ptr = png_create_info_struct(png_ptr);
    png_infop end_info_ptr = png_create_info_struct(png_ptr);

    if (info_ptr != NULL && end_info_ptr != NULL) {
      if (setjmp(png_jmpbuf(png_ptr)) == 0) {
        png_set_read_fn(png_ptr, &state, FuzzReadData);

        int sig_bytes = 0;
        if (Size > 0) sig_bytes = static_cast<int>(Data[0] % 9);
        if (static_cast<size_t>(sig_bytes) <= Size) {
          state.offset = static_cast<size_t>(sig_bytes);
          png_set_sig_bytes(png_ptr, sig_bytes);
        } else {
          state.offset = 0;
          png_set_sig_bytes(png_ptr, 0);
        }

        (void)png_set_option(png_ptr, PNG_MAXIMUM_INFLATE_WINDOW, (Size > 1) ? (Data[1] & 1) : 0);
#ifdef PNG_SKIP_sRGB_CHECK_PROFILE
        (void)png_set_option(png_ptr, PNG_SKIP_sRGB_CHECK_PROFILE, (Size > 2) ? (Data[2] & 1) : 0);
#endif

        (void)png_reset_zstream(png_ptr);
        png_read_info(png_ptr, info_ptr);
        (void)png_set_interlace_handling(png_ptr);
        ConsumeRows(png_ptr, info_ptr);
        png_read_end(png_ptr, end_info_ptr);
        (void)png_reset_zstream(png_ptr);
      }
    }

    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info_ptr);
  }

  FILE* fp = fopen("./dummy_file", "wb");
  if (fp != NULL) {
    if (Size > 0) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  fp = fopen("./dummy_file", "rb");
  if (fp != NULL) {
    png_structp png_ptr2 = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr2 != NULL) {
      png_infop info_ptr2 = png_create_info_struct(png_ptr2);
      png_infop end_info_ptr2 = png_create_info_struct(png_ptr2);

      if (info_ptr2 != NULL && end_info_ptr2 != NULL) {
        if (setjmp(png_jmpbuf(png_ptr2)) == 0) {
          png_set_read_fn(png_ptr2, fp, FuzzReadDataFile);

          int sig_bytes2 = 0;
          if (Size > 3) sig_bytes2 = static_cast<int>(Data[3] % 9);
          if (sig_bytes2 > 0) {
            uint8_t tmp[8];
            size_t n = fread(tmp, 1, static_cast<size_t>(sig_bytes2), fp);
            if (n == static_cast<size_t>(sig_bytes2)) {
              png_set_sig_bytes(png_ptr2, sig_bytes2);
            } else {
              rewind(fp);
              png_set_sig_bytes(png_ptr2, 0);
            }
          } else {
            png_set_sig_bytes(png_ptr2, 0);
          }

          (void)png_set_option(png_ptr2, PNG_MAXIMUM_INFLATE_WINDOW, (Size > 4) ? (Data[4] & 1) : 0);
#ifdef PNG_SKIP_sRGB_CHECK_PROFILE
          (void)png_set_option(png_ptr2, PNG_SKIP_sRGB_CHECK_PROFILE, (Size > 5) ? (Data[5] & 1) : 0);
#endif

          png_read_info(png_ptr2, info_ptr2);
          (void)png_set_interlace_handling(png_ptr2);
          ConsumeRows(png_ptr2, info_ptr2);
          png_read_end(png_ptr2, end_info_ptr2);
          (void)png_reset_zstream(png_ptr2);
        }
      }

      png_destroy_read_struct(&png_ptr2, &info_ptr2, &end_info_ptr2);
    }
    fclose(fp);
  }

  remove("./dummy_file");
  return 0;
}