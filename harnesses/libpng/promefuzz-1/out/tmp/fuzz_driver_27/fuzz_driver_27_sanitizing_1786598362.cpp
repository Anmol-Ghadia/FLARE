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

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

static png_uint_32 ReadBE32(const uint8_t* p) {
  return (static_cast<png_uint_32>(p[0]) << 24) |
         (static_cast<png_uint_32>(p[1]) << 16) |
         (static_cast<png_uint_32>(p[2]) << 8) |
         (static_cast<png_uint_32>(p[3]));
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE* fp = fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    if (Size > 0) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn, PngWarningFn);
  if (png_ptr == nullptr)
    return 0;

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr) {
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    return 0;
  }

  png_color_16 bg;
  bg.index = (Size > 0) ? Data[0] : 0;
  bg.red = (Size > 2) ? static_cast<png_uint_16>((Data[1] << 8) | Data[2]) : 0;
  bg.green = (Size > 4) ? static_cast<png_uint_16>((Data[3] << 8) | Data[4]) : 0;
  bg.blue = (Size > 6) ? static_cast<png_uint_16>((Data[5] << 8) | Data[6]) : 0;
  bg.gray = (Size > 8) ? static_cast<png_uint_16>((Data[7] << 8) | Data[8]) : 0;

  int option_raw = (Size > 9) ? Data[9] : 0;
  int option = option_raw & 0x0e;
  int onoff1 = (Size > 10) ? (Data[10] & 1) : 0;
  int onoff2 = (Size > 11) ? (Data[11] & 1) : 1;

  int alpha_mode = (Size > 12) ? static_cast<int>(Data[12] % 4) : 0;

  png_fixed_point output_gamma = PNG_DEFAULT_sRGB;
  if (Size > 16) {
    switch (Data[13] % 5) {
      case 0:
        output_gamma = PNG_DEFAULT_sRGB;
        break;
      case 1:
        output_gamma = PNG_GAMMA_MAC_18;
        break;
      case 2:
        output_gamma = PNG_GAMMA_sRGB;
        break;
      case 3:
        output_gamma = PNG_GAMMA_LINEAR;
        break;
      default:
        output_gamma = static_cast<png_fixed_point>(ReadBE32(Data + 13) & 0x7fffffff);
        break;
    }
  }

  int background_gamma_code = PNG_BACKGROUND_GAMMA_UNKNOWN;
  if (Size > 17)
    background_gamma_code = static_cast<int>(Data[17] % 4);

  int need_expand = (Size > 18) ? (Data[18] & 1) : 0;

  png_fixed_point background_gamma = PNG_GAMMA_sRGB;
  if (Size > 23) {
    switch (Data[19] % 5) {
      case 0:
        background_gamma = PNG_DEFAULT_sRGB;
        break;
      case 1:
        background_gamma = PNG_GAMMA_MAC_18;
        break;
      case 2:
        background_gamma = PNG_GAMMA_sRGB;
        break;
      case 3:
        background_gamma = PNG_GAMMA_LINEAR;
        break;
      default:
        background_gamma = static_cast<png_fixed_point>(ReadBE32(Data + 20) & 0x7fffffff);
        break;
    }
  }

  (void)png_set_option(png_ptr, option, onoff1);
  (void)png_set_option(png_ptr, option, onoff2);

  if (Size > 24 && (Data[24] & 1))
    png_set_tRNS_to_alpha(png_ptr);

  if (Size > 25 && (Data[25] & 1))
    png_set_scale_16(png_ptr);

  if (Size > 26 && (Data[26] & 1))
    png_set_background_fixed(png_ptr, &bg, background_gamma_code, need_expand,
                             background_gamma);
  else
    png_set_background_fixed(png_ptr, &bg,
                             (background_gamma_code + 1) % 4,
                             need_expand ^ 1, background_gamma);

  png_set_alpha_mode_fixed(png_ptr, alpha_mode, output_gamma);

  png_read_update_info(png_ptr, info_ptr);

  if (Size > 27 && (Data[27] & 1)) {
    png_read_update_info(png_ptr, info_ptr);
  }

  png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
  return 0;
}