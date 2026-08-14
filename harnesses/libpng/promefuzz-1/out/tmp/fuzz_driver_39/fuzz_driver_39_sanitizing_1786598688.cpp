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
#include <cstring>

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE *fp = std::fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    if (Size > 0) {
      std::fwrite(Data, 1, Size, fp);
    }
    std::fclose(fp);
  }

  fp = std::fopen("./dummy_file", "rb");
  if (fp == nullptr) {
    return 0;
  }

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn, PngWarningFn);
  if (png_ptr == nullptr) {
    std::fclose(fp);
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr) {
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    std::fclose(fp);
    return 0;
  }

  png_infop end_info = png_create_info_struct(png_ptr);
  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    std::fclose(fp);
    return 0;
  }

  png_init_io(png_ptr, fp);

  if (Size >= 8) {
    png_set_sig_bytes(png_ptr, 0);
  }

  png_read_info(png_ptr, info_ptr);

  png_uint_32 width = 0, height = 0;
  int bit_depth = 0, color_type = 0, interlace_method = 0;
  int compression_method = 0, filter_method = 0;
  (void)png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
                     &interlace_method, &compression_method, &filter_method);
  (void)png_get_IHDR(png_ptr, info_ptr, nullptr, nullptr, nullptr, nullptr,
                     nullptr, nullptr, nullptr);

  png_charp iccp_name = nullptr;
  int iccp_compression_type = 0;
  png_bytep iccp_profile = nullptr;
  png_uint_32 iccp_proflen = 0;
  (void)png_get_iCCP(png_ptr, info_ptr, &iccp_name, &iccp_compression_type,
                     &iccp_profile, &iccp_proflen);
  (void)png_get_iCCP(png_ptr, info_ptr, &iccp_name, nullptr,
                     &iccp_profile, &iccp_proflen);

  int scal_unit = 0;
  png_charp scal_width = nullptr;
  png_charp scal_height = nullptr;
  (void)png_get_sCAL_s(png_ptr, info_ptr, &scal_unit, &scal_width, &scal_height);

  png_charp pcal_purpose = nullptr;
  png_int_32 pcal_X0 = 0, pcal_X1 = 0;
  int pcal_type = 0, pcal_nparams = 0;
  png_charp pcal_units = nullptr;
  png_charpp pcal_params = nullptr;
  (void)png_get_pCAL(png_ptr, info_ptr, &pcal_purpose, &pcal_X0, &pcal_X1,
                     &pcal_type, &pcal_nparams, &pcal_units, &pcal_params);

  const png_uint_32 flags[] = {
      PNG_INFO_iCCP,
      PNG_INFO_sCAL,
      PNG_INFO_pCAL,
      PNG_INFO_PLTE,
      PNG_INFO_tRNS,
      PNG_INFO_gAMA,
      PNG_INFO_sRGB,
      PNG_INFO_tIME,
      PNG_INFO_pHYs
  };

  for (size_t i = 0; i < sizeof(flags) / sizeof(flags[0]); ++i) {
    (void)png_get_valid(png_ptr, info_ptr, flags[i]);
  }

  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
  std::fclose(fp);
  return 0;
}