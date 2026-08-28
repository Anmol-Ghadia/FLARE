// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_get_bKGD at pngget.c:488:1 in png.h
// png_get_eXIf at pngget.c:779:1 in png.h
// png_get_eXIf_1 at pngget.c:789:1 in png.h
// png_set_eXIf at pngset.c:139:1 in png.h
// png_set_eXIf_1 at pngset.c:148:1 in png.h
// png_set_eXIf_1 at pngset.c:148:1 in png.h
// png_get_eXIf_1 at pngget.c:789:1 in png.h
// png_get_bKGD at pngget.c:488:1 in png.h
// png_set_invalid at pngset.c:1626:1 in png.h
// png_get_bKGD at pngget.c:488:1 in png.h
// png_get_eXIf_1 at pngget.c:789:1 in png.h
// png_get_eXIf at pngget.c:779:1 in png.h
// png_set_eXIf_1 at pngset.c:148:1 in png.h
// png_get_eXIf_1 at pngget.c:789:1 in png.h
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

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    if (Data != nullptr && Size > 0)
      fwrite(Data, 1, Size, fp);
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

  png_color_16p background = nullptr;
  png_get_bKGD(png_ptr, info_ptr, &background);

  png_bytep exif_stub = nullptr;
  png_get_eXIf(png_ptr, info_ptr, &exif_stub);

  png_uint_32 num_exif = 0;
  png_bytep exif_out = nullptr;
  png_get_eXIf_1(png_ptr, info_ptr, &num_exif, &exif_out);

  png_set_eXIf(png_ptr, info_ptr, exif_stub);

  if (Size > 0 && Data != nullptr) {
    png_uint_32 exif_size = static_cast<png_uint_32>(Size > 0xffffffffu ? 0xffffffffu : Size);
    png_bytep exif_buf = const_cast<png_bytep>(reinterpret_cast<const png_byte*>(Data));
    png_set_eXIf_1(png_ptr, info_ptr, exif_size, exif_buf);
  } else {
    png_set_eXIf_1(png_ptr, info_ptr, 0, nullptr);
  }

  png_get_eXIf_1(png_ptr, info_ptr, &num_exif, &exif_out);
  png_get_bKGD(png_ptr, info_ptr, &background);

  int masks[] = {
      0,
      PNG_INFO_bKGD,
#ifdef PNG_INFO_eXIf
      PNG_INFO_eXIf,
#else
      0,
#endif
      ~0
  };

  for (size_t i = 0; i < sizeof(masks) / sizeof(masks[0]); ++i) {
    png_set_invalid(png_ptr, info_ptr, masks[i]);
    png_get_bKGD(png_ptr, info_ptr, &background);
    png_get_eXIf_1(png_ptr, info_ptr, &num_exif, &exif_out);
    png_get_eXIf(png_ptr, info_ptr, &exif_stub);
  }

  if (Size >= 4) {
    png_uint_32 partial = static_cast<png_uint_32>(
        (static_cast<png_uint_32>(Data[0]) << 24) |
        (static_cast<png_uint_32>(Data[1]) << 16) |
        (static_cast<png_uint_32>(Data[2]) << 8) |
        static_cast<png_uint_32>(Data[3]));
    if (partial > Size)
      partial = static_cast<png_uint_32>(Size);
    png_set_eXIf_1(png_ptr, info_ptr, partial,
                   const_cast<png_bytep>(reinterpret_cast<const png_byte*>(Data)));
    png_get_eXIf_1(png_ptr, info_ptr, &num_exif, &exif_out);
  }

  png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
  return 0;
}