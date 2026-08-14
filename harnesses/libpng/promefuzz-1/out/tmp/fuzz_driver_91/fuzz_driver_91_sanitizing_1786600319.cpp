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

static png_uint_32 ReadU32(const uint8_t *p) {
  return (static_cast<png_uint_32>(p[0]) << 24) |
         (static_cast<png_uint_32>(p[1]) << 16) |
         (static_cast<png_uint_32>(p[2]) << 8) |
         static_cast<png_uint_32>(p[3]);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  uint8_t buf4[4] = {0, 0, 0, 0};
  uint8_t buf2[2] = {0, 0};

  if (Size >= 4) {
    png_uint_32 v32 = ReadU32(Data);
    png_save_uint_32(buf4, v32);
  } else {
    png_save_uint_32(buf4, 0);
  }

  if (Size >= 2) {
    unsigned int v16 = (static_cast<unsigned int>(Data[0]) << 8) |
                       static_cast<unsigned int>(Data[1]);
    png_save_uint_16(buf2, v16);
  } else {
    png_save_uint_16(buf2, 0);
  }

  (void)png_get_uint_16(buf2);
  if (Size >= 2) {
    (void)png_get_uint_16(reinterpret_cast<png_const_bytep>(Data));
  }

  FILE *fp = fopen("./dummy_file", "wb");
  if (!fp) {
    return 0;
  }

  png_structp png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn, PngWarningFn);
  if (!png_ptr) {
    fclose(fp);
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    png_destroy_write_struct(&png_ptr, nullptr);
    fclose(fp);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
    return 0;
  }

  png_init_io(png_ptr, fp);

  png_set_IHDR(png_ptr, info_ptr,
               1, 1,
               8,
               PNG_COLOR_TYPE_GRAY,
               PNG_INTERLACE_NONE,
               PNG_COMPRESSION_TYPE_BASE,
               PNG_FILTER_TYPE_BASE);
  png_write_info(png_ptr, info_ptr);

  uint8_t row[1] = { static_cast<uint8_t>(Size ? Data[0] : 0) };
  png_write_row(png_ptr, row);

  png_byte chunk_name[4] = {'f', 'U', 'Z', 'z'};
  if (Size >= 4) {
    memcpy(chunk_name, Data, 4);
  }

  const png_bytep chunk_data =
      Size > 4 ? const_cast<png_bytep>(reinterpret_cast<png_const_bytep>(Data + 4)) : nullptr;
  size_t remaining = Size > 4 ? Size - 4 : 0;

  png_uint_32 len1 = remaining > 1024 ? 1024U : static_cast<png_uint_32>(remaining);
  png_write_chunk(png_ptr, chunk_name, chunk_data, len1);

  png_uint_32 len2 = 0;
  if (remaining > len1) {
    size_t extra = remaining - len1;
    len2 = extra > 1024 ? 1024U : static_cast<png_uint_32>(extra);
  }

  png_write_chunk_start(png_ptr, chunk_name, len2);
  if (len2 > 0) {
    png_write_chunk_data(png_ptr, chunk_data + len1, len2);
  }
  png_write_chunk_end(png_ptr);

  png_write_chunk_start(png_ptr, reinterpret_cast<png_const_bytep>("tEXt"), 0);
  png_write_chunk_end(png_ptr);

  png_write_end(png_ptr, info_ptr);

  png_destroy_write_struct(&png_ptr, &info_ptr);
  fclose(fp);
  return 0;
}