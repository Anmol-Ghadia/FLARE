// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_access_version_number at png.c:986:1 in png.h
// png_get_header_version at png.c:852:1 in png.h
// png_get_copyright at png.c:811:1 in png.h
// png_get_header_ver at png.c:844:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_get_header_version at png.c:852:1 in png.h
// png_get_copyright at png.c:811:1 in png.h
// png_get_libpng_ver at png.c:837:1 in png.h
// png_get_header_ver at png.c:844:1 in png.h
// png_get_header_version at png.c:852:1 in png.h
// png_get_copyright at png.c:811:1 in png.h
// png_get_header_ver at png.c:844:1 in png.h
// png_get_libpng_ver at png.c:837:1 in png.h
// png_access_version_number at png.c:986:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
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
#include <setjmp.h>
#include <stdio.h>
#include <string.h>

static void FuzzPngError(png_structp png_ptr, png_const_charp /*msg*/) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void FuzzPngWarn(png_structp /*png_ptr*/, png_const_charp /*msg*/) {
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    if (Size > 0) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  png_uint_32 ver_num = png_access_version_number();
  (void)ver_num;

  png_const_charp hv_null = png_get_header_version(nullptr);
  png_const_charp cv_null = png_get_copyright(nullptr);
  png_const_charp hver_null = png_get_header_ver(nullptr);
  (void)hv_null;
  (void)cv_null;
  (void)hver_null;

  const char *versions[] = {
      PNG_LIBPNG_VER_STRING,
      "0",
      "",
      "1.6.0",
      "999.999.999",
  };

  size_t choice_count = sizeof(versions) / sizeof(versions[0]);
  size_t idx = 0;
  if (Size > 0) {
    idx = Data[0] % choice_count;
  }

  png_structp png_ptr = png_create_read_struct(
      versions[idx], nullptr, FuzzPngError, FuzzPngWarn);
  if (png_ptr == nullptr) {
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    return 0;
  }

  png_const_charp hv = png_get_header_version(png_ptr);
  png_const_charp cv = png_get_copyright(png_ptr);
  png_const_charp lv = png_get_libpng_ver(png_ptr);
  png_const_charp hver = png_get_header_ver(png_ptr);
  (void)hv;
  (void)cv;
  (void)lv;
  (void)hver;

  if (Size > 1) {
    size_t loops = 1 + (Data[1] % 8);
    for (size_t i = 0; i < loops; ++i) {
      png_const_structrp arg = ((i ^ Size) & 1) ? png_ptr : nullptr;
      (void)png_get_header_version(arg);
      (void)png_get_copyright(arg);
      (void)png_get_header_ver(arg);
      (void)png_get_libpng_ver(arg);
      (void)png_access_version_number();
    }
  }

  png_destroy_read_struct(&png_ptr, nullptr, nullptr);
  return 0;
}