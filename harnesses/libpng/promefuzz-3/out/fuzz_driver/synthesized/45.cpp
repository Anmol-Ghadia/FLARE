// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_gray_to_rgb at pngrtran.c:955:1 in png.h
// png_set_benign_errors at pngset.c:1671:1 in png.h
// png_set_option at png.c:4332:1 in png.h
// png_set_rgb_to_gray at pngrtran.c:1059:1 in png.h
// png_get_rgb_to_gray_status at pngget.c:1154:1 in png.h
// png_read_end at pngread.c:766:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_init_io at png.c:703:1 in png.h
// png_set_benign_errors at pngset.c:1671:1 in png.h
// png_set_option at png.c:4332:1 in png.h
// png_set_gray_to_rgb at pngrtran.c:955:1 in png.h
// png_set_rgb_to_gray at pngrtran.c:1059:1 in png.h
// png_get_rgb_to_gray_status at pngget.c:1154:1 in png.h
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
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <vector>
#include <limits>

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput_45(const uint8_t *Data, size_t Size) {
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
  if (info_ptr == nullptr) {
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    fclose(fp);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    fclose(fp);
    return 0;
  }

  png_init_io(png_ptr, fp);

  int benign_allowed = 0;
  if (Size > 0) benign_allowed = Data[0] & 1;
  png_set_benign_errors(png_ptr, benign_allowed);

  int option = 0;
  int onoff = 0;
  if (Size > 1) option = static_cast<int>(Data[1]);
  if (Size > 2) onoff = static_cast<int>(Data[2] & 1);
  (void)png_set_option(png_ptr, option, onoff);

  int error_action = 0;
  double red = 0.2126;
  double green = 0.7152;
  if (Size > 3) error_action = static_cast<int>(Data[3] % 4);
  if (Size > 11) {
    uint32_t rbits =
        (static_cast<uint32_t>(Data[4]) << 24) |
        (static_cast<uint32_t>(Data[5]) << 16) |
        (static_cast<uint32_t>(Data[6]) << 8) |
        static_cast<uint32_t>(Data[7]);
    uint32_t gbits =
        (static_cast<uint32_t>(Data[8]) << 24) |
        (static_cast<uint32_t>(Data[9]) << 16) |
        (static_cast<uint32_t>(Data[10]) << 8) |
        static_cast<uint32_t>(Data[11]);

    red = (static_cast<int32_t>(rbits) % 2000000) / 1000000.0;
    green = (static_cast<int32_t>(gbits) % 2000000) / 1000000.0;
  }

  if (Size > 12 && (Data[12] & 1)) {
    png_set_gray_to_rgb(png_ptr);
  }

  png_set_rgb_to_gray(png_ptr, error_action, red, green);
  (void)png_get_rgb_to_gray_status(png_ptr);

  png_read_info(png_ptr, info_ptr);

  if (Size > 13 && (Data[13] & 1)) {
    png_set_gray_to_rgb(png_ptr);
  }
  if (Size > 14) {
    png_set_benign_errors(png_ptr, Data[14] & 1);
  }
  if (Size > 15) {
    int option2 = static_cast<int>(Data[15]);
    int onoff2 = (Size > 16) ? static_cast<int>(Data[16] & 1) : 0;
    (void)png_set_option(png_ptr, option2, onoff2);
  }
  if (Size > 17) {
    int ea2 = static_cast<int>(Data[17] % 4);
    double red2 = red;
    double green2 = green;
    if (Size > 25) {
      uint32_t r2 =
          (static_cast<uint32_t>(Data[18]) << 24) |
          (static_cast<uint32_t>(Data[19]) << 16) |
          (static_cast<uint32_t>(Data[20]) << 8) |
          static_cast<uint32_t>(Data[21]);
      uint32_t g2 =
          (static_cast<uint32_t>(Data[22]) << 24) |
          (static_cast<uint32_t>(Data[23]) << 16) |
          (static_cast<uint32_t>(Data[24]) << 8) |
          static_cast<uint32_t>(Data[25]);
      red2 = (static_cast<int32_t>(r2) % 4000000) / 1000000.0;
      green2 = (static_cast<int32_t>(g2) % 4000000) / 1000000.0;
    }
    png_set_rgb_to_gray(png_ptr, ea2, red2, green2);
    (void)png_get_rgb_to_gray_status(png_ptr);
  }

  png_read_end(png_ptr, info_ptr);

  png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
  fclose(fp);
  return 0;
}