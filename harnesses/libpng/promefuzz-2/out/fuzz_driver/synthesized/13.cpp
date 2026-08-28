// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_get_iCCP at pngget.c:736:1 in png.h
// png_get_valid at pngget.c:20:1 in png.h
// png_read_end at pngread.c:766:1 in png.h
// png_get_eXIf_1 at pngget.c:789:1 in png.h
// png_get_sBIT at pngget.c:1031:1 in png.h
// png_get_bKGD at pngget.c:488:1 in png.h
// png_get_cHRM_fixed at pngget.c:641:1 in png.h
// png_get_iCCP at pngget.c:736:1 in png.h
// png_get_valid at pngget.c:20:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_init_io at png.c:703:1 in png.h
// png_read_info at pngread.c:92:1 in png.h
// png_get_IHDR at pngget.c:825:1 in png.h
// png_get_eXIf_1 at pngget.c:789:1 in png.h
// png_get_eXIf_1 at pngget.c:789:1 in png.h
// png_get_eXIf_1 at pngget.c:789:1 in png.h
// png_get_sBIT at pngget.c:1031:1 in png.h
// png_get_bKGD at pngget.c:488:1 in png.h
// png_get_cHRM_fixed at pngget.c:641:1 in png.h
// png_get_cHRM_fixed at pngget.c:641:1 in png.h
// png_get_iCCP at pngget.c:736:1 in png.h
// png_get_iCCP at pngget.c:736:1 in png.h
// png_get_iCCP at pngget.c:736:1 in png.h
// png_get_iCCP at pngget.c:736:1 in png.h
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

extern "C" int LLVMFuzzerTestOneInput_13(const uint8_t *Data, size_t Size) {
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

  png_infop end_info = png_create_info_struct(png_ptr);
  if (end_info == nullptr) {
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    fclose(fp);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    fclose(fp);
    return 0;
  }

  png_init_io(png_ptr, fp);
  png_read_info(png_ptr, info_ptr);

  png_uint_32 width = 0, height = 0;
  int bit_depth = 0, color_type = 0, interlace_type = 0, compression_type = 0, filter_method = 0;
  (void)png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
                     &interlace_type, &compression_type, &filter_method);

  png_uint_32 num_exif = 0;
  png_bytep exif = nullptr;
  (void)png_get_eXIf_1(png_ptr, info_ptr, &num_exif, &exif);
  (void)png_get_eXIf_1(png_ptr, info_ptr, nullptr, &exif);
  (void)png_get_eXIf_1(png_ptr, info_ptr, &num_exif, nullptr);

  png_color_8p sig_bit = nullptr;
  (void)png_get_sBIT(png_ptr, info_ptr, &sig_bit);

  png_color_16p background = nullptr;
  (void)png_get_bKGD(png_ptr, info_ptr, &background);

  png_fixed_point white_x = 0, white_y = 0, red_x = 0, red_y = 0;
  png_fixed_point green_x = 0, green_y = 0, blue_x = 0, blue_y = 0;
  (void)png_get_cHRM_fixed(png_ptr, info_ptr, &white_x, &white_y, &red_x, &red_y,
                           &green_x, &green_y, &blue_x, &blue_y);
  (void)png_get_cHRM_fixed(png_ptr, info_ptr, nullptr, &white_y, nullptr, &red_y,
                           &green_x, nullptr, &blue_x, nullptr);

  png_charp iccp_name = nullptr;
  int iccp_compression = -1;
  png_bytep profile = nullptr;
  png_uint_32 profile_len = 0;
  (void)png_get_iCCP(png_ptr, info_ptr, &iccp_name, &iccp_compression, &profile, &profile_len);
  (void)png_get_iCCP(png_ptr, info_ptr, nullptr, &iccp_compression, &profile, &profile_len);
  (void)png_get_iCCP(png_ptr, info_ptr, &iccp_name, nullptr, &profile, &profile_len);
  (void)png_get_iCCP(png_ptr, info_ptr, &iccp_name, &iccp_compression, nullptr, &profile_len);
  (void)png_get_iCCP(png_ptr, info_ptr, &iccp_name, &iccp_compression, &profile, nullptr);

  const png_uint_32 flags[] = {
      PNG_INFO_gAMA, PNG_INFO_sBIT, PNG_INFO_cHRM, PNG_INFO_PLTE,
      PNG_INFO_tRNS, PNG_INFO_bKGD, PNG_INFO_hIST, PNG_INFO_pHYs,
      PNG_INFO_oFFs, PNG_INFO_tIME, PNG_INFO_pCAL, PNG_INFO_sRGB,
      PNG_INFO_iCCP, PNG_INFO_sPLT, PNG_INFO_sCAL, PNG_INFO_IDAT
#ifdef PNG_INFO_eXIf
      , PNG_INFO_eXIf
#endif
  };

  for (size_t i = 0; i < sizeof(flags) / sizeof(flags[0]); ++i) {
    (void)png_get_valid(png_ptr, info_ptr, flags[i]);
  }

  png_read_end(png_ptr, end_info);

  (void)png_get_eXIf_1(png_ptr, end_info, &num_exif, &exif);
  (void)png_get_sBIT(png_ptr, end_info, &sig_bit);
  (void)png_get_bKGD(png_ptr, end_info, &background);
  (void)png_get_cHRM_fixed(png_ptr, end_info, &white_x, &white_y, &red_x, &red_y,
                           &green_x, &green_y, &blue_x, &blue_y);
  (void)png_get_iCCP(png_ptr, end_info, &iccp_name, &iccp_compression, &profile, &profile_len);

  for (size_t i = 0; i < sizeof(flags) / sizeof(flags[0]); ++i) {
    (void)png_get_valid(png_ptr, end_info, flags[i]);
  }

  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
  fclose(fp);
  return 0;
}