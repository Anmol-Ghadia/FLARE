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
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>

static void fuzz_png_error(png_structp png_ptr, png_const_charp) {
  jmp_buf *jb = reinterpret_cast<jmp_buf*>(png_get_error_ptr(png_ptr));
  if (jb != NULL) longjmp(*jb, 1);
  abort();
}

static void fuzz_png_warning(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  uint8_t buf4[4] = {0, 0, 0, 0};
  uint8_t buf2[2] = {0, 0};
  uint8_t out4[4] = {0, 0, 0, 0};
  uint8_t out2[2] = {0, 0};

  if (Size >= 4) {
    memcpy(buf4, Data, 4);
  } else if (Size > 0) {
    memcpy(buf4, Data, Size);
  }

  if (Size >= 2) {
    memcpy(buf2, Data + (Size >= 6 ? 4 : 0), 2);
  } else if (Size == 1) {
    buf2[0] = Data[0];
  }

  png_uint_32 u32 = png_get_uint_32(buf4);
  png_int_32 i32 = png_get_int_32(buf4);
  png_uint_16 u16 = png_get_uint_16(buf2);

  png_save_uint_32(out4, u32);
  png_save_uint_32(out4, static_cast<png_uint_32>(i32));
  png_save_uint_16(out2, u16);
  png_save_uint_16(out2, static_cast<unsigned int>(u32 & 0xFFFFu));

  png_uint_32 roundtrip_u32 = png_get_uint_32(out4);
  png_uint_16 roundtrip_u16 = png_get_uint_16(out2);
  (void)roundtrip_u32;
  (void)roundtrip_u16;

  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL,
                                               fuzz_png_error, fuzz_png_warning);
  if (png_ptr != NULL) {
    jmp_buf jb;
    png_set_error_fn(png_ptr, &jb, fuzz_png_error, fuzz_png_warning);

    if (setjmp(jb) == 0) {
      (void)png_get_uint_31(png_ptr, buf4);

      uint8_t high_buf[4] = {0xFF, 0xFF, 0xFF, 0xFF};
      (void)png_get_uint_31(png_ptr, high_buf);
    }

    if (setjmp(jb) == 0) {
      png_save_uint_32(out4, u32 ^ 0x80000000u);
      (void)png_get_uint_31(png_ptr, out4);
    }

    png_destroy_read_struct(&png_ptr, NULL, NULL);
  }

  return 0;
}