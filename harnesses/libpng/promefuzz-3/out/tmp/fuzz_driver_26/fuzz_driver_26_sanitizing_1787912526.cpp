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

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  {
    png_get_io_state(nullptr);
    png_get_current_pass_number(nullptr);
    png_get_io_chunk_type(nullptr);
    png_read_rows(nullptr, nullptr, nullptr, 0);
    png_set_interlace_handling(nullptr);
    png_read_png(nullptr, nullptr, 0, nullptr);
  }

  FILE *fp = std::fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    if (Size > 0) {
      std::fwrite(Data, 1, Size, fp);
    }
    std::fclose(fp);
  }

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn, PngWarningFn);
  if (png_ptr == nullptr) {
    return 0;
  }

  png_inforp info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr) {
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    return 0;
  }

  fp = std::fopen("./dummy_file", "rb");
  if (fp == nullptr) {
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    return 0;
  }

  png_init_io(png_ptr, fp);

  if (Size >= 8) {
    png_set_sig_bytes(png_ptr, 0);
  }

  png_get_io_state(png_ptr);
  png_get_current_pass_number(png_ptr);
  png_get_io_chunk_type(png_ptr);

  int transforms = 0;
#ifdef PNG_TRANSFORM_STRIP_16
  if (Size > 0 && (Data[0] & 1)) transforms |= PNG_TRANSFORM_STRIP_16;
#endif
#ifdef PNG_TRANSFORM_STRIP_ALPHA
  if (Size > 1 && (Data[1] & 1)) transforms |= PNG_TRANSFORM_STRIP_ALPHA;
#endif
#ifdef PNG_TRANSFORM_PACKING
  if (Size > 2 && (Data[2] & 1)) transforms |= PNG_TRANSFORM_PACKING;
#endif
#ifdef PNG_TRANSFORM_PACKSWAP
  if (Size > 3 && (Data[3] & 1)) transforms |= PNG_TRANSFORM_PACKSWAP;
#endif
#ifdef PNG_TRANSFORM_EXPAND
  if (Size > 4 && (Data[4] & 1)) transforms |= PNG_TRANSFORM_EXPAND;
#endif
#ifdef PNG_TRANSFORM_INVERT_MONO
  if (Size > 5 && (Data[5] & 1)) transforms |= PNG_TRANSFORM_INVERT_MONO;
#endif
#ifdef PNG_TRANSFORM_SHIFT
  if (Size > 6 && (Data[6] & 1)) transforms |= PNG_TRANSFORM_SHIFT;
#endif
#ifdef PNG_TRANSFORM_BGR
  if (Size > 7 && (Data[7] & 1)) transforms |= PNG_TRANSFORM_BGR;
#endif
#ifdef PNG_TRANSFORM_SWAP_ALPHA
  if (Size > 8 && (Data[8] & 1)) transforms |= PNG_TRANSFORM_SWAP_ALPHA;
#endif
#ifdef PNG_TRANSFORM_SWAP_ENDIAN
  if (Size > 9 && (Data[9] & 1)) transforms |= PNG_TRANSFORM_SWAP_ENDIAN;
#endif
#ifdef PNG_TRANSFORM_INVERT_ALPHA
  if (Size > 10 && (Data[10] & 1)) transforms |= PNG_TRANSFORM_INVERT_ALPHA;
#endif
#ifdef PNG_TRANSFORM_GRAY_TO_RGB
  if (Size > 11 && (Data[11] & 1)) transforms |= PNG_TRANSFORM_GRAY_TO_RGB;
#endif
#ifdef PNG_TRANSFORM_EXPAND_16
  if (Size > 12 && (Data[12] & 1)) transforms |= PNG_TRANSFORM_EXPAND_16;
#endif
#ifdef PNG_TRANSFORM_SCALE_16
  if (Size > 13 && (Data[13] & 1)) transforms |= PNG_TRANSFORM_SCALE_16;
#endif

  if (Size > 0 && (Data[0] & 0x80)) {
    int passes = png_set_interlace_handling(png_ptr);
    (void)passes;
    png_get_current_pass_number(png_ptr);
  }

  if (Size > 1 && (Data[1] & 0x80)) {
    png_read_png(png_ptr, info_ptr, transforms, nullptr);
    png_get_io_state(png_ptr);
    png_get_current_pass_number(png_ptr);
    png_get_io_chunk_type(png_ptr);
  } else {
    png_read_info(png_ptr, info_ptr);
    png_get_io_state(png_ptr);
    png_get_io_chunk_type(png_ptr);

    int passes = 1;
    if (Size > 2 && (Data[2] & 0x80)) {
      passes = png_set_interlace_handling(png_ptr);
    }

    png_read_update_info(png_ptr, info_ptr);

    png_uint_32 width = png_get_image_width(png_ptr, info_ptr);
    png_uint_32 height = png_get_image_height(png_ptr, info_ptr);
    png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);

    if (rowbytes == 0) {
      rowbytes = 1;
    }

    const png_uint_32 max_rows = 32;
    png_uint_32 rows_to_read = std::min(height, max_rows);

    std::vector<std::vector<png_byte>> row_storage(rows_to_read);
    std::vector<std::vector<png_byte>> display_storage(rows_to_read);
    std::vector<png_bytep> row_ptrs(rows_to_read, nullptr);
    std::vector<png_bytep> display_ptrs(rows_to_read, nullptr);

    for (png_uint_32 i = 0; i < rows_to_read; ++i) {
      row_storage[i].resize(rowbytes);
      row_ptrs[i] = row_storage[i].data();
      if (Size > 3 && (Data[3] & 1)) {
        display_storage[i].resize(rowbytes);
        display_ptrs[i] = display_storage[i].data();
      }
    }

    for (int pass = 0; pass < std::max(1, passes); ++pass) {
      png_get_current_pass_number(png_ptr);

      png_uint_32 consumed = 0;
      while (consumed < rows_to_read) {
        png_uint_32 chunk = 1;
        if (Size > 4) {
          chunk = 1 + (Data[(4 + consumed) % Size] % 4);
        }
        if (chunk > rows_to_read - consumed) {
          chunk = rows_to_read - consumed;
        }

        png_bytepp rows = row_ptrs.empty() ? nullptr : &row_ptrs[consumed];
        png_bytepp displays = nullptr;
        if (!display_ptrs.empty() && display_ptrs[0] != nullptr) {
          displays = &display_ptrs[consumed];
        }

        png_read_rows(png_ptr, rows, displays, chunk);
        png_get_io_state(png_ptr);
        png_get_io_chunk_type(png_ptr);
        consumed += chunk;
      }

      for (png_uint_32 y = rows_to_read; y < height; ++y) {
        png_read_rows(png_ptr, nullptr, nullptr, 1);
        if ((y & 7u) == 0) {
          png_get_io_state(png_ptr);
          png_get_current_pass_number(png_ptr);
        }
      }
    }

    if (Size > 5 && (Data[5] & 1)) {
      png_read_end(png_ptr, info_ptr);
      png_get_io_chunk_type(png_ptr);
    }
    (void)width;
  }

  std::fclose(fp);
  png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
  return 0;
}