// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_swap at pngtrans.c:35:1 in png.h
// png_set_filler at pngtrans.c:120:1 in png.h
// png_read_update_info at pngread.c:267:1 in png.h
// png_get_image_width at pngget.c:52:1 in png.h
// png_get_image_height at pngget.c:61:1 in png.h
// png_get_rowbytes at pngget.c:30:1 in png.h
// png_read_image at pngread.c:700:1 in png.h
// png_read_end at pngread.c:766:1 in png.h
// png_read_end at pngread.c:766:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_bgr at pngtrans.c:21:1 in png.h
// png_set_swap_alpha at pngtrans.c:223:1 in png.h
// png_set_invert_alpha at pngtrans.c:237:1 in png.h
// png_set_palette_to_rgb at pngrtran.c:902:1 in png.h
// png_set_swap at pngtrans.c:35:1 in png.h
// png_set_filler at pngtrans.c:120:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_init_io at png.c:703:1 in png.h
// png_sig_cmp at png.c:74:1 in png.h
// png_set_sig_bytes at png.c:47:1 in png.h
// png_set_bgr at pngtrans.c:21:1 in png.h
// png_set_swap_alpha at pngtrans.c:223:1 in png.h
// png_set_invert_alpha at pngtrans.c:237:1 in png.h
// png_set_palette_to_rgb at pngrtran.c:902:1 in png.h
// png_set_swap at pngtrans.c:35:1 in png.h
// png_set_filler at pngtrans.c:120:1 in png.h
// png_set_filler at pngtrans.c:120:1 in png.h
// png_read_info at pngread.c:92:1 in png.h
// png_set_bgr at pngtrans.c:21:1 in png.h
// png_set_swap_alpha at pngtrans.c:223:1 in png.h
// png_set_invert_alpha at pngtrans.c:237:1 in png.h
// png_set_palette_to_rgb at pngrtran.c:902:1 in png.h
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
#include <setjmp.h>

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput_29(const uint8_t *Data, size_t Size) {
  FILE *fp = std::fopen("./dummy_file", "wb");
  if (fp) {
    if (Size > 0) {
      std::fwrite(Data, 1, Size, fp);
    }
    std::fclose(fp);
  }

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn, PngWarningFn);
  if (!png_ptr) {
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    return 0;
  }

  FILE *rfp = std::fopen("./dummy_file", "rb");
  if (!rfp) {
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    return 0;
  }

  png_init_io(png_ptr, rfp);

  if (Size >= 8 && png_sig_cmp(const_cast<png_bytep>(Data), 0, 8) == 0) {
    png_set_sig_bytes(png_ptr, 0);
  }

  if (Size > 0) {
    if (Data[0] & 0x01) png_set_bgr(png_ptr);
    if (Data[0] & 0x02) png_set_swap_alpha(png_ptr);
    if (Data[0] & 0x04) png_set_invert_alpha(png_ptr);
    if (Data[0] & 0x08) png_set_palette_to_rgb(png_ptr);
    if (Data[0] & 0x10) png_set_swap(png_ptr);
  }

  png_uint_32 filler = 0;
  int filler_flags = PNG_FILLER_AFTER;
  if (Size >= 5) {
    filler = (static_cast<png_uint_32>(Data[1]) << 24) |
             (static_cast<png_uint_32>(Data[2]) << 16) |
             (static_cast<png_uint_32>(Data[3]) << 8) |
             static_cast<png_uint_32>(Data[4]);
    filler_flags = (Size > 5 && (Data[5] & 1)) ? PNG_FILLER_BEFORE : PNG_FILLER_AFTER;
    png_set_filler(png_ptr, filler, filler_flags);
  } else {
    png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);
  }

  png_read_info(png_ptr, info_ptr);

  if (Size > 6) {
    switch (Data[6] % 6) {
      case 0:
        png_set_bgr(png_ptr);
        break;
      case 1:
        png_set_swap_alpha(png_ptr);
        break;
      case 2:
        png_set_invert_alpha(png_ptr);
        break;
      case 3:
        png_set_palette_to_rgb(png_ptr);
        break;
      case 4:
        png_set_swap(png_ptr);
        break;
      case 5:
        png_set_filler(png_ptr, filler ^ 0xffffffffu,
                       (filler_flags == PNG_FILLER_AFTER) ? PNG_FILLER_BEFORE
                                                          : PNG_FILLER_AFTER);
        break;
    }
  }

  png_read_update_info(png_ptr, info_ptr);

  png_uint_32 width = png_get_image_width(png_ptr, info_ptr);
  png_uint_32 height = png_get_image_height(png_ptr, info_ptr);
  png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);

  if (width > 0 && height > 0 && rowbytes > 0 && height < 100000 && rowbytes < (1u << 26)) {
    std::vector<png_byte> image;
    if (rowbytes <= SIZE_MAX / height) {
      image.resize(rowbytes * height);
      std::vector<png_bytep> rows(height);
      for (png_uint_32 y = 0; y < height; ++y) {
        rows[y] = image.data() + y * rowbytes;
      }
      png_read_image(png_ptr, rows.data());
      png_read_end(png_ptr, info_ptr);
    }
  } else {
    png_read_end(png_ptr, info_ptr);
  }

  std::fclose(rfp);
  png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);

  png_set_bgr(nullptr);
  png_set_swap_alpha(nullptr);
  png_set_invert_alpha(nullptr);
  png_set_palette_to_rgb(nullptr);
  png_set_swap(nullptr);
  png_set_filler(nullptr, 0, PNG_FILLER_AFTER);

  return 0;
}