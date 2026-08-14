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

static void DummyError(png_structp png_ptr, png_const_charp) {
  if (png_ptr) {
    png_longjmp(png_ptr, 1);
  }
}

static void DummyWarn(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    if (Data != nullptr && Size > 0) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  png_access_version_number();

  png_get_copyright(nullptr);
  png_get_header_version(nullptr);
  png_get_header_ver(nullptr);
  png_get_libpng_ver(nullptr);

  const char *versions[] = {
      PNG_LIBPNG_VER_STRING,
      "0",
      "",
      "1.0.0",
      "999.999.999",
  };

  png_voidp error_ptrs[] = {
      nullptr,
      (png_voidp)Data,
      (png_voidp)&Size,
  };

  for (size_t i = 0; i < sizeof(versions) / sizeof(versions[0]); ++i) {
    for (size_t j = 0; j < sizeof(error_ptrs) / sizeof(error_ptrs[0]); ++j) {
      png_error_ptr err_fn = (Size > 0 && (Data[0] & 1)) ? DummyError : nullptr;
      png_error_ptr warn_fn = (Size > 1 && (Data[1] & 1)) ? DummyWarn : nullptr;

      png_structp png_ptr =
          png_create_read_struct(versions[i], error_ptrs[j], err_fn, warn_fn);

      if (png_ptr != nullptr) {
        if (setjmp(png_jmpbuf(png_ptr)) == 0) {
          png_get_copyright(png_ptr);
          png_get_header_version(png_ptr);
          png_get_header_ver(png_ptr);
          png_get_libpng_ver(png_ptr);

          if (Size > 2 && (Data[2] & 1)) {
            png_error(png_ptr, "fuzz");
          } else if (Size > 3 && (Data[3] & 1)) {
            png_warning(png_ptr, "fuzz");
          }
        }

        png_destroy_read_struct(&png_ptr, nullptr, nullptr);
      }
    }
  }

  return 0;
}