// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_write_chunk at pngwutil.c:192:1 in png.h
// png_save_uint_16 at pngwutil.c:37:1 in png.h
// png_save_uint_32 at pngwutil.c:24:1 in png.h
// png_write_chunk at pngwutil.c:192:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_get_error_ptr at pngerror.c:856:1 in png.h
// png_save_uint_16 at pngwutil.c:37:1 in png.h
// png_save_uint_16 at pngwutil.c:37:1 in png.h
// png_save_uint_32 at pngwutil.c:24:1 in png.h
// png_save_uint_32 at pngwutil.c:24:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_init_io at png.c:703:1 in png.h
// png_get_uint_31 at pngrutil.c:22:1 in png.h
// png_write_chunk at pngwutil.c:192:1 in png.h
// png_write_chunk at pngwutil.c:192:1 in png.h
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
#include <string.h>

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  jmp_buf* jb = reinterpret_cast<jmp_buf*>(png_get_error_ptr(png_ptr));
  if (jb) longjmp(*jb, 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput_93(const uint8_t *Data, size_t Size) {
  uint8_t local8[8] = {0};
  uint8_t local4[4] = {0};
  uint8_t local2[2] = {0};

  if (Size > 0) local8[0] = Data[0];
  if (Size > 1) local8[1] = Data[1];
  if (Size > 2) local8[2] = Data[2];
  if (Size > 3) local8[3] = Data[3];
  if (Size > 4) local8[4] = Data[4];
  if (Size > 5) local8[5] = Data[5];
  if (Size > 6) local8[6] = Data[6];
  if (Size > 7) local8[7] = Data[7];

  png_uint_16 v16 = 0;
  png_uint_32 v32 = 0;

  if (Size >= 2) {
    v16 = png_get_uint_16(Data);
  } else {
    v16 = png_get_uint_16(local2);
  }

  if (Size >= 4) {
    v32 = png_get_uint_32(Data);
  } else {
    v32 = png_get_uint_32(local4);
  }

  png_save_uint_16(local2, static_cast<unsigned int>(v16));
  png_save_uint_16(local2, static_cast<unsigned int>(v32 & 0xFFFFu));
  png_save_uint_32(local4, v32);
  png_save_uint_32(local4, static_cast<png_uint_32>(v16));

  FILE* fp = fopen("./dummy_file", "wb");
  if (!fp) return 0;

  jmp_buf jmpbuf;
  png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                                                &jmpbuf,
                                                PngErrorFn,
                                                PngWarningFn);
  if (!png_ptr) {
    fclose(fp);
    return 0;
  }

  if (setjmp(jmpbuf)) {
    png_destroy_write_struct(&png_ptr, nullptr);
    fclose(fp);
    return 0;
  }

  png_init_io(png_ptr, fp);

  png_byte be4_from_input[4] = {0, 0, 0, 0};
  if (Size >= 4) {
    memcpy(be4_from_input, Data, 4);
  } else {
    memcpy(be4_from_input, local4, 4);
  }

  (void)png_get_uint_31(png_ptr, be4_from_input);

  png_byte chunk_name1[4] = {'f', 'U', 'Z', 'Z'};
  png_byte chunk_name2[4] = {'I', 'D', 'A', 'T'};
  png_byte chunk_name3[4] = {'t', 'E', 'S', 't'};

  const png_bytep chunk_data = Size ? const_cast<png_bytep>(reinterpret_cast<const png_byte*>(Data)) : nullptr;
  size_t len1 = Size;
  size_t len2 = Size > 16 ? 16 : Size;
  size_t len3 = 0;

  png_write_chunk(png_ptr, chunk_name1, chunk_data, len1);
  png_write_chunk(png_ptr, chunk_name2, chunk_data, len2);
  png_write_chunk(png_ptr, chunk_name3, nullptr, len3);

  png_byte synthesized[6] = {0};
  png_save_uint_16(synthesized, static_cast<unsigned int>(v16));
  png_save_uint_32(synthesized + 2, v32);
  png_write_chunk(png_ptr, chunk_name1, synthesized, sizeof(synthesized));

  png_destroy_write_struct(&png_ptr, nullptr);
  fclose(fp);
  return 0;
}