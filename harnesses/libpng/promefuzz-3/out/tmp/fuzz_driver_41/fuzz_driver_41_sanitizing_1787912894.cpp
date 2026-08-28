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
#include <string.h>
#include <setjmp.h>

static void FuzzErrorFn(png_structp png_ptr, png_const_charp) {
  png_longjmp(png_ptr, 1);
}

static void FuzzWarnFn(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  png_uint_32 ver_num = png_access_version_number();
  (void)ver_num;

  FILE *out = fopen("./dummy_file", "wb");
  if (out != NULL) {
    if (Size > 0) {
      (void)fwrite(Data, 1, Size, out);
    }
    fclose(out);
  }

  png_structp png_ptr = png_create_read_struct_2(
      PNG_LIBPNG_VER_STRING, nullptr, FuzzErrorFn, FuzzWarnFn,
      nullptr, nullptr, nullptr);

  if (png_ptr != nullptr) {
    if (setjmp(png_jmpbuf(png_ptr)) == 0) {
      static const int crit_actions[] = {
          PNG_CRC_DEFAULT, PNG_CRC_ERROR_QUIT, PNG_CRC_WARN_USE,
          PNG_CRC_QUIET_USE
      };
      static const int ancil_actions[] = {
          PNG_CRC_DEFAULT, PNG_CRC_ERROR_QUIT, PNG_CRC_WARN_DISCARD,
          PNG_CRC_WARN_USE, PNG_CRC_QUIET_USE, PNG_CRC_NO_CHANGE
      };

      int crit = crit_actions[(Size > 0 ? Data[0] : 0) %
                              (sizeof(crit_actions) / sizeof(crit_actions[0]))];
      int ancil = ancil_actions[(Size > 1 ? Data[1] : 0) %
                                (sizeof(ancil_actions) / sizeof(ancil_actions[0]))];

      png_set_crc_action(png_ptr, crit, ancil);

      png_const_charp ver = png_get_libpng_ver(png_ptr);
      (void)ver;
    }
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
  }

  png_image image1;
  memset(&image1, 0, sizeof(image1));
  image1.version = (Size > 2 && (Data[2] & 1)) ? PNG_IMAGE_VERSION : 0;
  (void)png_image_begin_read_from_file(&image1, "./dummy_file");
  png_image_free(&image1);

  FILE *in = fopen("./dummy_file", "rb");
  if (in != NULL) {
    png_image image2;
    memset(&image2, 0, sizeof(image2));
    image2.version = (Size > 3 && (Data[3] & 1)) ? PNG_IMAGE_VERSION : 0;
    (void)png_image_begin_read_from_stdio(&image2, in);
    png_image_free(&image2);
    fclose(in);
  }

  return 0;
}