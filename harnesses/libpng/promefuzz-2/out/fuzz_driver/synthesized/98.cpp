// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_write_chunk at pngwutil.c:192:1 in png.h
// png_write_chunk at pngwutil.c:192:1 in png.h
// png_write_chunk at pngwutil.c:192:1 in png.h
// png_write_chunk at pngwutil.c:192:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_save_uint_16 at pngwutil.c:37:1 in png.h
// png_save_uint_32 at pngwutil.c:24:1 in png.h
// png_save_int_32 at png.c:726:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_get_uint_31 at pngrutil.c:22:1 in png.h
// png_get_uint_31 at pngrutil.c:22:1 in png.h
// png_get_uint_31 at pngrutil.c:22:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_init_io at png.c:703:1 in png.h
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

extern "C" int LLVMFuzzerTestOneInput_98(const uint8_t *Data, size_t Size) {
  png_byte buf2[2] = {0, 0};
  png_byte buf4a[4] = {0, 0, 0, 0};
  png_byte buf4b[4] = {0, 0, 0, 0};
  png_byte readbuf[4] = {0, 0, 0, 0};

  unsigned int u16 = 0;
  png_uint_32 u32 = 0;
  png_int_32 i32 = 0;

  if (Size > 0) u16 = static_cast<unsigned int>(Data[0]) |
                      (Size > 1 ? static_cast<unsigned int>(Data[1]) << 8 : 0);

  if (Size >= 4) {
    readbuf[0] = Data[0];
    readbuf[1] = Data[1];
    readbuf[2] = Data[2];
    readbuf[3] = Data[3];
  } else {
    for (size_t i = 0; i < Size && i < 4; ++i) readbuf[i] = Data[i];
  }

  if (Size >= 8) {
    u32 = (static_cast<png_uint_32>(Data[4]) << 24) |
          (static_cast<png_uint_32>(Data[5]) << 16) |
          (static_cast<png_uint_32>(Data[6]) << 8) |
          static_cast<png_uint_32>(Data[7]);
  } else {
    u32 = png_get_uint_32(readbuf);
  }

  i32 = static_cast<png_int_32>(u32);

  png_save_uint_16(buf2, u16);
  png_save_uint_32(buf4a, u32);
  png_save_int_32(buf4b, i32);

  (void)png_get_uint_32(readbuf);
  (void)png_get_uint_32(buf4a);
  (void)png_get_uint_32(buf4b);

  png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr,
                                                PngErrorFn, PngWarningFn);
  if (png_ptr == nullptr) {
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_write_struct(&png_ptr, nullptr);
    return 0;
  }

  (void)png_get_uint_31(png_ptr, readbuf);
  (void)png_get_uint_31(png_ptr, buf4a);
  (void)png_get_uint_31(png_ptr, buf4b);

  FILE *fp = std::fopen("./dummy_file", "wb");
  if (fp == nullptr) {
    png_destroy_write_struct(&png_ptr, nullptr);
    return 0;
  }

  png_init_io(png_ptr, fp);

  png_byte chunk_name[4] = {'f', 'U', 'Z', 'Z'};
  if (Size >= 12) {
    chunk_name[0] = Data[8];
    chunk_name[1] = Data[9];
    chunk_name[2] = Data[10];
    chunk_name[3] = Data[11];
  }

  const png_bytep chunk_data =
      Size > 12 ? const_cast<png_bytep>(reinterpret_cast<const png_byte*>(Data + 12))
                : nullptr;
  size_t chunk_len = Size > 12 ? Size - 12 : 0;

  png_write_chunk(png_ptr, chunk_name, chunk_data, chunk_len);

  png_byte alt_name1[4] = {'I', 'D', 'A', 'T'};
  png_write_chunk(png_ptr, alt_name1, reinterpret_cast<png_const_bytep>(buf2), 2);

  png_byte alt_name2[4] = {'t', 'E', 'S', 'T'};
  png_write_chunk(png_ptr, alt_name2, reinterpret_cast<png_const_bytep>(buf4a), 4);

  png_byte alt_name3[4] = {'n', 'U', 'L', 'L'};
  png_write_chunk(png_ptr, alt_name3, nullptr, 0);

  std::fclose(fp);
  png_destroy_write_struct(&png_ptr, nullptr);
  return 0;
}