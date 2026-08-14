// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_read_update_info at pngread.c:267:1 in png.h
// png_get_image_width at pngget.c:52:1 in png.h
// png_get_image_height at pngget.c:61:1 in png.h
// png_get_rowbytes at pngget.c:30:1 in png.h
// png_read_row at pngread.c:383:1 in png.h
// png_read_row at pngread.c:383:1 in png.h
// png_read_end at pngread.c:766:1 in png.h
// png_get_sBIT at pngget.c:1031:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_init_io at png.c:703:1 in png.h
// png_set_sBIT at pngset.c:638:1 in png.h
// png_set_shift at pngtrans.c:84:1 in png.h
// png_read_info at pngread.c:92:1 in png.h
// png_get_sBIT at pngget.c:1031:1 in png.h
// png_set_sBIT at pngset.c:638:1 in png.h
// png_get_sBIT at pngget.c:1031:1 in png.h
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
#include <setjmp.h>
#include <vector>
#include <algorithm>

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    if (Size > 0) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  fp = fopen("./dummy_file", "rb");
  if (fp == nullptr) {
    return 0;
  }

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn, PngWarningFn);
  if (png_ptr == nullptr) {
    fclose(fp);
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  png_infop end_info = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr || end_info == nullptr) {
    if (info_ptr != nullptr || end_info != nullptr) {
      png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    } else {
      png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    }
    fclose(fp);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    fclose(fp);
    return 0;
  }

  png_init_io(png_ptr, fp);

  png_color_8 sig_bit;
  sig_bit.red = Size > 0 ? Data[0] : 0;
  sig_bit.green = Size > 1 ? Data[1] : 0;
  sig_bit.blue = Size > 2 ? Data[2] : 0;
  sig_bit.gray = Size > 3 ? Data[3] : 0;
  sig_bit.alpha = Size > 4 ? Data[4] : 0;

  png_set_sBIT(png_ptr, info_ptr, &sig_bit);

  png_color_8 shift_bits;
  shift_bits.red = Size > 5 ? Data[5] : sig_bit.red;
  shift_bits.green = Size > 6 ? Data[6] : sig_bit.green;
  shift_bits.blue = Size > 7 ? Data[7] : sig_bit.blue;
  shift_bits.gray = Size > 8 ? Data[8] : sig_bit.gray;
  shift_bits.alpha = Size > 9 ? Data[9] : sig_bit.alpha;

  png_set_shift(png_ptr, &shift_bits);

  png_read_info(png_ptr, info_ptr);

  png_color_8p out_sig = nullptr;
  (void)png_get_sBIT(png_ptr, info_ptr, &out_sig);

  if (Size > 10 && (Data[10] & 1)) {
    png_color_8 alt_sig;
    alt_sig.red = Size > 11 ? Data[11] : 0;
    alt_sig.green = Size > 12 ? Data[12] : 0;
    alt_sig.blue = Size > 13 ? Data[13] : 0;
    alt_sig.gray = Size > 14 ? Data[14] : 0;
    alt_sig.alpha = Size > 15 ? Data[15] : 0;
    png_set_sBIT(png_ptr, info_ptr, &alt_sig);
    (void)png_get_sBIT(png_ptr, info_ptr, &out_sig);
  }

  png_read_update_info(png_ptr, info_ptr);

  png_uint_32 width = png_get_image_width(png_ptr, info_ptr);
  png_uint_32 height = png_get_image_height(png_ptr, info_ptr);
  png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);

  if (width > 0 && height > 0 && rowbytes > 0) {
    const png_uint_32 max_rows = 64;
    png_uint_32 rows_to_read = std::min(height, max_rows);

    std::vector<png_byte> image_storage;
    std::vector<png_bytep> row_ptrs;

    if (rowbytes <= (1u << 20) && rows_to_read <= max_rows) {
      image_storage.resize(static_cast<size_t>(rowbytes) * rows_to_read);
      row_ptrs.resize(rows_to_read);

      for (png_uint_32 i = 0; i < rows_to_read; ++i) {
        row_ptrs[i] = image_storage.data() + static_cast<size_t>(i) * rowbytes;
      }

      for (png_uint_32 i = 0; i < rows_to_read; ++i) {
        png_read_row(png_ptr, row_ptrs[i], nullptr);
      }

      for (png_uint_32 i = rows_to_read; i < height; ++i) {
        png_read_row(png_ptr, nullptr, nullptr);
      }
    }
  }

  png_read_end(png_ptr, end_info);

  if (Size > 16 && (Data[16] & 1)) {
    png_color_8p end_sig = nullptr;
    (void)png_get_sBIT(png_ptr, end_info, &end_sig);
  }

  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
  fclose(fp);
  return 0;
}