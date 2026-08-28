// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_get_signature at pngget.c:477:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_save_uint_16 at pngwutil.c:37:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_get_uint_31 at pngrutil.c:22:1 in png.h
// png_get_uint_31 at pngrutil.c:22:1 in png.h
// png_get_uint_31 at pngrutil.c:22:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_get_signature at pngget.c:477:1 in png.h
// png_get_signature at pngget.c:477:1 in png.h
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
#include <setjmp.h>

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput_68(const uint8_t *Data, size_t Size) {
  uint8_t local[8] = {0};
  if (Data != nullptr && Size > 0) {
    size_t n = Size < sizeof(local) ? Size : sizeof(local);
    memcpy(local, Data, n);
  }

  uint8_t out16[2] = {0, 0};

  unsigned int v16 = 0;
  if (Size >= 2) {
    v16 = (static_cast<unsigned int>(local[0]) << 8) |
          static_cast<unsigned int>(local[1]);
  } else if (Size == 1) {
    v16 = static_cast<unsigned int>(local[0]);
  }
  png_save_uint_16(out16, v16);
  (void)png_get_uint_16(out16);

  if (Size >= 2) {
    (void)png_get_uint_16(local);
  }

  if (Size >= 4) {
    (void)png_get_uint_32(local);
    (void)png_get_int_32(local);
  } else {
    uint8_t padded4[4] = {0, 0, 0, 0};
    memcpy(padded4, local, Size < 4 ? Size : 4);
    (void)png_get_uint_32(padded4);
    (void)png_get_int_32(padded4);
  }

  uint8_t derived4[4] = {out16[0], out16[1], local[0], local[1]};
  (void)png_get_uint_32(derived4);
  (void)png_get_int_32(derived4);

  FILE *fp = fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    if (Data != nullptr && Size > 0) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn, PngWarningFn);
  if (png_ptr != nullptr) {
    if (setjmp(png_jmpbuf(png_ptr)) == 0) {
      uint8_t buf31[4] = {0, 0, 0, 0};
      if (Size >= 4) {
        memcpy(buf31, Data, 4);
      } else if (Size > 0) {
        memcpy(buf31, Data, Size);
      }

      (void)png_get_uint_31(png_ptr, buf31);

      buf31[0] &= 0x7f;
      (void)png_get_uint_31(png_ptr, buf31);

      buf31[0] |= 0x80;
      (void)png_get_uint_31(png_ptr, buf31);
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr != nullptr) {
      png_const_bytep sig = png_get_signature(png_ptr, info_ptr);
      if (sig != nullptr) {
        volatile png_byte sink = sig[0];
        (void)sink;
      }
    }

    (void)png_get_signature(png_ptr, nullptr);
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
  }

  (void)png_get_signature(nullptr, nullptr);

  return 0;
}