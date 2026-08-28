// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_write_fn at pngwio.c:122:1 in png.h
// png_write_chunk_start at pngwutil.c:115:1 in png.h
// png_write_chunk_data at pngwutil.c:127:1 in png.h
// png_write_chunk_end at pngwutil.c:146:1 in png.h
// png_save_uint_32 at pngwutil.c:24:1 in png.h
// png_write_chunk_start at pngwutil.c:115:1 in png.h
// png_write_chunk_data at pngwutil.c:127:1 in png.h
// png_write_chunk_end at pngwutil.c:146:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_save_uint_32 at pngwutil.c:24:1 in png.h
// png_save_int_32 at png.c:726:1 in png.h
// png_save_uint_16 at pngwutil.c:37:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
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
#include <cstring>
#include <vector>

static void PngWriteCallback(png_structp png_ptr, png_bytep data, png_size_t length) {
  FILE* fp = reinterpret_cast<FILE*>(png_get_io_ptr(png_ptr));
  if (fp != nullptr && data != nullptr && length > 0) {
    (void)fwrite(data, 1, length, fp);
  }
}

static void PngFlushCallback(png_structp png_ptr) {
  FILE* fp = reinterpret_cast<FILE*>(png_get_io_ptr(png_ptr));
  if (fp != nullptr) {
    (void)fflush(fp);
  }
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  uint8_t buf4[4] = {0, 0, 0, 0};
  uint8_t buf2[2] = {0, 0};

  if (Size >= 4) {
    memcpy(buf4, Data, 4);
  } else if (Size > 0) {
    memcpy(buf4, Data, Size);
  }

  png_uint_32 u32 = 0;
  png_int_32 i32 = 0;
  unsigned int u16 = 0;

  if (Size >= 4) {
    u32 = (static_cast<png_uint_32>(Data[0]) << 24) |
          (static_cast<png_uint_32>(Data[1]) << 16) |
          (static_cast<png_uint_32>(Data[2]) << 8) |
          (static_cast<png_uint_32>(Data[3]));
    i32 = static_cast<png_int_32>(u32);
  } else {
    for (size_t i = 0; i < Size; ++i) {
      u32 = (u32 << 8) | Data[i];
    }
    i32 = static_cast<png_int_32>(u32);
  }

  if (Size >= 6) {
    u16 = (static_cast<unsigned int>(Data[4]) << 8) |
          static_cast<unsigned int>(Data[5]);
  } else if (Size >= 2) {
    u16 = (static_cast<unsigned int>(Data[0]) << 8) |
          static_cast<unsigned int>(Data[1]);
  } else if (Size == 1) {
    u16 = static_cast<unsigned int>(Data[0]);
  }

  png_save_uint_32(buf4, u32);
  png_save_int_32(buf4, i32);
  png_save_uint_16(buf2, u16);
  (void)png_get_int_32(buf4);

  FILE* fp = fopen("./dummy_file", "wb");
  if (fp == nullptr) {
    return 0;
  }

  if (Size > 0) {
    (void)fwrite(Data, 1, Size, fp);
    (void)fflush(fp);
    (void)fseek(fp, 0, SEEK_SET);
  }

  png_structp png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (png_ptr == nullptr) {
    fclose(fp);
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr) {
    png_destroy_write_struct(&png_ptr, nullptr);
    fclose(fp);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
    return 0;
  }

  png_set_write_fn(png_ptr, fp, PngWriteCallback, PngFlushCallback);

  uint8_t chunk_name[4] = {'f', 'U', 'Z', 'Z'};
  if (Size >= 10) {
    for (int i = 0; i < 4; ++i) {
      uint8_t c = Data[6 + i];
      if (c < 32 || c > 126) c = static_cast<uint8_t>('A' + (c % 26));
      chunk_name[i] = c;
    }
  }

  png_uint_32 chunk_length = 0;
  if (Size >= 14) {
    chunk_length = (static_cast<png_uint_32>(Data[10]) << 24) |
                   (static_cast<png_uint_32>(Data[11]) << 16) |
                   (static_cast<png_uint_32>(Data[12]) << 8) |
                   (static_cast<png_uint_32>(Data[13]));
  } else {
    chunk_length = static_cast<png_uint_32>(Size);
  }

  if (Size < chunk_length) {
    chunk_length = static_cast<png_uint_32>(Size);
  }

  png_write_chunk_start(png_ptr, chunk_name, chunk_length);

  if (chunk_length > 0) {
    png_write_chunk_data(png_ptr, const_cast<png_bytep>(reinterpret_cast<const png_byte*>(Data)), chunk_length);
  }

  png_write_chunk_end(png_ptr);

  if (Size >= 4) {
    png_uint_32 alt_len = static_cast<png_uint_32>(png_get_int_32(buf4));
    if (alt_len > Size) {
      alt_len = static_cast<png_uint_32>(Size);
    }

    uint8_t chunk_name2[4] = {'T', 'E', 'S', 'T'};
    png_save_uint_32(buf4, alt_len);
    png_write_chunk_start(png_ptr, chunk_name2, alt_len);
    if (alt_len > 0) {
      png_write_chunk_data(png_ptr, const_cast<png_bytep>(reinterpret_cast<const png_byte*>(Data)), alt_len);
    }
    png_write_chunk_end(png_ptr);
  }

  png_destroy_write_struct(&png_ptr, &info_ptr);
  fclose(fp);
  return 0;
}