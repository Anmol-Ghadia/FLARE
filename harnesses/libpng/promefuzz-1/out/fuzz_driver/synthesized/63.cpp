// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_palette_to_rgb at pngrtran.c:902:1 in png.h
// png_set_tRNS_to_alpha at pngrtran.c:926:1 in png.h
// png_set_gray_to_rgb at pngrtran.c:955:1 in png.h
// png_set_expand_16 at pngrtran.c:942:1 in png.h
// png_set_strip_alpha at pngrtran.c:210:1 in png.h
// png_set_gray_to_rgb at pngrtran.c:955:1 in png.h
// png_set_palette_to_rgb at pngrtran.c:902:1 in png.h
// png_set_tRNS_to_alpha at pngrtran.c:926:1 in png.h
// png_set_strip_alpha at pngrtran.c:210:1 in png.h
// png_set_expand_16 at pngrtran.c:942:1 in png.h
// png_set_gray_to_rgb at pngrtran.c:955:1 in png.h
// png_set_strip_alpha at pngrtran.c:210:1 in png.h
// png_set_expand_16 at pngrtran.c:942:1 in png.h
// png_set_palette_to_rgb at pngrtran.c:902:1 in png.h
// png_set_tRNS_to_alpha at pngrtran.c:926:1 in png.h
// png_read_update_info at pngread.c:267:1 in png.h
// png_start_read_image at pngread.c:298:1 in png.h
// png_get_image_width at pngget.c:52:1 in png.h
// png_get_image_height at pngget.c:61:1 in png.h
// png_get_rowbytes at pngget.c:30:1 in png.h
// png_read_row at pngread.c:383:1 in png.h
// png_read_end at pngread.c:766:1 in png.h
// png_read_end at pngread.c:766:1 in png.h
// png_read_end at pngread.c:766:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_init_io at png.c:703:1 in png.h
// png_sig_cmp at png.c:74:1 in png.h
// png_set_sig_bytes at png.c:47:1 in png.h
// png_read_info at pngread.c:92:1 in png.h
// png_set_gray_to_rgb at pngrtran.c:955:1 in png.h
// png_set_strip_alpha at pngrtran.c:210:1 in png.h
// png_set_expand_16 at pngrtran.c:942:1 in png.h
// png_set_palette_to_rgb at pngrtran.c:902:1 in png.h
// png_set_tRNS_to_alpha at pngrtran.c:926:1 in png.h
// png_set_strip_alpha at pngrtran.c:210:1 in png.h
// png_set_gray_to_rgb at pngrtran.c:955:1 in png.h
// png_set_palette_to_rgb at pngrtran.c:902:1 in png.h
// png_set_tRNS_to_alpha at pngrtran.c:926:1 in png.h
// png_set_expand_16 at pngrtran.c:942:1 in png.h
// png_set_expand_16 at pngrtran.c:942:1 in png.h
// png_set_palette_to_rgb at pngrtran.c:902:1 in png.h
// png_set_gray_to_rgb at pngrtran.c:955:1 in png.h
// png_set_strip_alpha at pngrtran.c:210:1 in png.h
// png_set_tRNS_to_alpha at pngrtran.c:926:1 in png.h
// png_set_tRNS_to_alpha at pngrtran.c:926:1 in png.h
// png_set_expand_16 at pngrtran.c:942:1 in png.h
// png_set_strip_alpha at pngrtran.c:210:1 in png.h
// png_set_palette_to_rgb at pngrtran.c:902:1 in png.h
// png_set_gray_to_rgb at pngrtran.c:955:1 in png.h
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

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput_63(const uint8_t *Data, size_t Size) {
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

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr) {
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    return 0;
  }

  png_infop end_info = png_create_info_struct(png_ptr);
  if (end_info == nullptr) {
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    return 0;
  }

  fp = std::fopen("./dummy_file", "rb");
  if (fp == nullptr) {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    return 0;
  }

  png_init_io(png_ptr, fp);

  if (Size >= 8 && png_sig_cmp(const_cast<png_bytep>(Data), 0, 8) == 0) {
    png_set_sig_bytes(png_ptr, 0);
  }

  png_read_info(png_ptr, info_ptr);

  if (Size > 0) {
    switch (Data[0] % 6) {
      case 0:
        png_set_gray_to_rgb(png_ptr);
        png_set_strip_alpha(png_ptr);
        png_set_expand_16(png_ptr);
        png_set_palette_to_rgb(png_ptr);
        png_set_tRNS_to_alpha(png_ptr);
        break;
      case 1:
        png_set_strip_alpha(png_ptr);
        png_set_gray_to_rgb(png_ptr);
        png_set_palette_to_rgb(png_ptr);
        png_set_tRNS_to_alpha(png_ptr);
        png_set_expand_16(png_ptr);
        break;
      case 2:
        png_set_expand_16(png_ptr);
        png_set_palette_to_rgb(png_ptr);
        png_set_gray_to_rgb(png_ptr);
        png_set_strip_alpha(png_ptr);
        png_set_tRNS_to_alpha(png_ptr);
        break;
      case 3:
        png_set_tRNS_to_alpha(png_ptr);
        png_set_expand_16(png_ptr);
        png_set_strip_alpha(png_ptr);
        png_set_palette_to_rgb(png_ptr);
        png_set_gray_to_rgb(png_ptr);
        break;
      case 4:
        png_set_palette_to_rgb(png_ptr);
        png_set_tRNS_to_alpha(png_ptr);
        png_set_gray_to_rgb(png_ptr);
        png_set_expand_16(png_ptr);
        png_set_strip_alpha(png_ptr);
        break;
      default:
        png_set_gray_to_rgb(png_ptr);
        png_set_palette_to_rgb(png_ptr);
        png_set_tRNS_to_alpha(png_ptr);
        png_set_strip_alpha(png_ptr);
        png_set_expand_16(png_ptr);
        break;
    }
  } else {
    png_set_gray_to_rgb(png_ptr);
    png_set_strip_alpha(png_ptr);
    png_set_expand_16(png_ptr);
    png_set_palette_to_rgb(png_ptr);
    png_set_tRNS_to_alpha(png_ptr);
  }

  if (Size > 1 && (Data[1] & 1)) {
    png_read_update_info(png_ptr, info_ptr);
  }

  png_start_read_image(png_ptr);

  png_uint_32 width = png_get_image_width(png_ptr, info_ptr);
  png_uint_32 height = png_get_image_height(png_ptr, info_ptr);
  png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);

  if (rowbytes > 0 && width > 0 && height > 0) {
    std::vector<png_byte> row(rowbytes);
    png_uint_32 rows_to_read = height;
    if (Size > 2) {
      png_uint_32 limit = static_cast<png_uint_32>(Data[2]) + 1;
      if (limit < rows_to_read) rows_to_read = limit;
    }

    for (png_uint_32 y = 0; y < rows_to_read; ++y) {
      png_read_row(png_ptr, row.data(), nullptr);
    }

    if (rows_to_read < height) {
      png_read_end(png_ptr, end_info);
    } else {
      png_read_end(png_ptr, end_info);
    }
  } else {
    png_read_end(png_ptr, end_info);
  }

  std::fclose(fp);
  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
  return 0;
}