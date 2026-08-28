// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_write_fn at pngwio.c:122:1 in png.h
// png_write_chunk_start at pngwutil.c:115:1 in png.h
// png_write_chunk_end at pngwutil.c:146:1 in png.h
// png_write_chunk_start at pngwutil.c:115:1 in png.h
// png_write_chunk_end at pngwutil.c:146:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_save_uint_32 at pngwutil.c:24:1 in png.h
// png_save_int_32 at png.c:726:1 in png.h
// png_save_uint_16 at pngwutil.c:37:1 in png.h
// png_save_uint_32 at pngwutil.c:24:1 in png.h
// png_create_write_struct_2 at pngwrite.c:505:1 in png.h
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
#include <vector>
#include <setjmp.h>

static png_voidp fuzz_malloc(png_structp, png_alloc_size_t size) {
  return std::malloc(size);
}

static void fuzz_free(png_structp, png_voidp ptr) {
  std::free(ptr);
}

static void fuzz_write_data(png_structp png_ptr, png_bytep data, png_size_t length) {
  FILE* fp = reinterpret_cast<FILE*>(png_get_io_ptr(png_ptr));
  if (fp != nullptr && data != nullptr && length > 0) {
    (void)std::fwrite(data, 1, length, fp);
  }
}

static void fuzz_flush(png_structp png_ptr) {
  FILE* fp = reinterpret_cast<FILE*>(png_get_io_ptr(png_ptr));
  if (fp != nullptr) {
    std::fflush(fp);
  }
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  uint8_t buf4[4] = {0, 0, 0, 0};
  uint8_t buf2[2] = {0, 0};

  for (size_t i = 0; i < 4 && i < Size; ++i) buf4[i] = Data[i];
  for (size_t i = 0; i < 2 && i < Size; ++i) buf2[i] = Data[i];

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

  if (Size >= 2) {
    u16 = (static_cast<unsigned int>(Data[0]) << 8) |
          static_cast<unsigned int>(Data[1]);
  } else if (Size == 1) {
    u16 = static_cast<unsigned int>(Data[0]);
  }

  png_save_uint_32(buf4, u32);
  png_save_int_32(buf4, i32);
  png_save_uint_16(buf2, u16);
  (void)png_get_int_32(buf4);

  if (Size >= 8) {
    png_save_uint_32(buf4, ~u32);
    (void)png_get_int_32(buf4);
  }

  FILE* fp = std::fopen("./dummy_file", "wb");
  if (fp == nullptr) {
    return 0;
  }

  if (Size > 0) {
    (void)std::fwrite(Data, 1, Size, fp);
    std::fflush(fp);
  }
  std::fclose(fp);

  fp = std::fopen("./dummy_file", "ab");
  if (fp == nullptr) {
    return 0;
  }

  png_structp png_ptr =
      png_create_write_struct_2(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr,
                                nullptr, fuzz_malloc, fuzz_free);
  if (png_ptr == nullptr) {
    std::fclose(fp);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_write_struct(&png_ptr, nullptr);
    std::fclose(fp);
    return 0;
  }

  png_set_write_fn(png_ptr, fp, fuzz_write_data, fuzz_flush);

  png_byte chunk_name[4] = {'t', 'E', 'S', 'T'};
  if (Size >= 4) {
    chunk_name[0] = Data[0];
    chunk_name[1] = Data[1];
    chunk_name[2] = Data[2];
    chunk_name[3] = Data[3];
  }

  png_uint_32 lengths[4];
  lengths[0] = (Size >= 4) ? u32 : static_cast<png_uint_32>(Size);
  lengths[1] = static_cast<png_uint_32>(Size & 0xffff);
  lengths[2] = 0;
  lengths[3] = (Size >= 8)
                   ? ((static_cast<png_uint_32>(Data[4]) << 24) |
                      (static_cast<png_uint_32>(Data[5]) << 16) |
                      (static_cast<png_uint_32>(Data[6]) << 8) |
                      (static_cast<png_uint_32>(Data[7])))
                   : 1;

  for (int idx = 0; idx < 4; ++idx) {
    png_byte name_variant[4] = {
        static_cast<png_byte>(chunk_name[0] ^ static_cast<png_byte>(idx)),
        static_cast<png_byte>(chunk_name[1]),
        static_cast<png_byte>(chunk_name[2]),
        static_cast<png_byte>(chunk_name[3])};

    png_write_chunk_start(png_ptr, name_variant, lengths[idx]);
    png_write_chunk_end(png_ptr);
  }

  png_write_chunk_start(png_ptr, chunk_name, 0);
  png_write_chunk_end(png_ptr);

  png_destroy_write_struct(&png_ptr, nullptr);
  std::fclose(fp);
  return 0;
}