// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_save_uint_16 at pngwutil.c:37:1 in png.h
// png_save_uint_32 at pngwutil.c:24:1 in png.h
// png_save_uint_16 at pngwutil.c:37:1 in png.h
// png_save_uint_32 at pngwutil.c:24:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_init_io at png.c:703:1 in png.h
// png_write_chunk at pngwutil.c:192:1 in png.h
// png_get_uint_31 at pngrutil.c:22:1 in png.h
// png_save_uint_32 at pngwutil.c:24:1 in png.h
// png_get_uint_31 at pngrutil.c:22:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
#include <cstdio>
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <setjmp.h>
#include <png.h>

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  uint8_t local8[8] = {0};
  uint8_t out4[4] = {0};
  uint8_t out2[2] = {0};

  if (Size > 0) {
    size_t copy = Size < sizeof(local8) ? Size : sizeof(local8);
    memcpy(local8, Data, copy);
  }

  png_uint_16 v16 = png_get_uint_16(local8);
  png_uint_32 v32 = png_get_uint_32(local8);
  png_save_uint_16(out2, static_cast<unsigned int>(v16));
  png_save_uint_32(out4, v32);

  if (Size >= 2) {
    png_uint_16 v16b = png_get_uint_16(reinterpret_cast<png_const_bytep>(Data));
    png_save_uint_16(out2, static_cast<unsigned int>(v16b));
  }

  if (Size >= 4) {
    png_uint_32 v32b = png_get_uint_32(reinterpret_cast<png_const_bytep>(Data));
    png_save_uint_32(out4, v32b);
  }

  FILE *fp = fopen("./dummy_file", "wb");
  if (fp != NULL) {
    if (Size > 0) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  fp = fopen("./dummy_file", "wb");
  if (fp == NULL) {
    return 0;
  }

  png_structp png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, PngErrorFn, PngWarningFn);
  if (png_ptr == NULL) {
    fclose(fp);
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == NULL) {
    png_destroy_write_struct(&png_ptr, NULL);
    fclose(fp);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
    return 0;
  }

  png_init_io(png_ptr, fp);

  png_byte chunk_name[4] = {'f', 'U', 'Z', 'Z'};
  if (Size >= 4) {
    memcpy(chunk_name, Data, 4);
  }

  png_const_bytep chunk_data = Size > 4 ? reinterpret_cast<png_const_bytep>(Data + 4)
                                        : reinterpret_cast<png_const_bytep>(Data);
  size_t chunk_len = Size > 4 ? (Size - 4) : Size;

  png_write_chunk(png_ptr, chunk_name, chunk_data, chunk_len);

  png_byte be31[4] = {0, 0, 0, 0};
  if (Size >= 4) {
    memcpy(be31, Data, 4);
  } else {
    memcpy(be31, out4, 4);
  }

  (void)png_get_uint_31(png_ptr, be31);

  png_byte valid31[4];
  png_save_uint_32(valid31, v32 & 0x7fffffffU);
  (void)png_get_uint_31(png_ptr, valid31);

  png_destroy_write_struct(&png_ptr, &info_ptr);
  fclose(fp);
  return 0;
}