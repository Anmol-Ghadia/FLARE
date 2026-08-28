// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_sRGB at pngset.c:653:1 in png.h
// png_set_sRGB_gAMA_and_cHRM at pngset.c:665:1 in png.h
// png_set_cHRM_fixed at pngset.c:40:1 in png.h
// png_set_gAMA_fixed at pngset.c:186:1 in png.h
// png_set_cHRM at pngset.c:102:1 in png.h
// png_get_gAMA at pngget.c:697:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_read_fn at pngrio.c:87:1 in png.h
// png_read_info at pngread.c:92:1 in png.h
// png_get_gAMA at pngget.c:697:1 in png.h
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
#include <vector>

static void PngErrorFn(png_structp, png_const_charp) {
  longjmp(png_jmpbuf((png_structp)0), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

static void ReadCallback(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead) {
  struct BufferState {
    const uint8_t* data;
    size_t size;
    size_t offset;
  };

  BufferState* state =
      reinterpret_cast<BufferState*>(png_get_io_ptr(png_ptr));
  if (state == nullptr || state->offset > state->size ||
      byteCountToRead > state->size - state->offset) {
    png_error(png_ptr, "read error");
    return;
  }

  memcpy(outBytes, state->data + state->offset, byteCountToRead);
  state->offset += byteCountToRead;
}

static png_fixed_point ReadFixed(const uint8_t* data, size_t size, size_t& off) {
  png_fixed_point v = 0;
  if (off + 4 <= size) {
    v = static_cast<png_fixed_point>(
        (static_cast<uint32_t>(data[off]) << 24) |
        (static_cast<uint32_t>(data[off + 1]) << 16) |
        (static_cast<uint32_t>(data[off + 2]) << 8) |
        static_cast<uint32_t>(data[off + 3]));
    off += 4;
  }
  return v;
}

static double ReadDoubleLike(const uint8_t* data, size_t size, size_t& off) {
  png_fixed_point fp = ReadFixed(data, size, off);
  return static_cast<double>(fp) / 100000.0;
}

extern "C" int LLVMFuzzerTestOneInput_11(const uint8_t *Data, size_t Size) {
  if (Data == nullptr || Size == 0) return 0;

  {
    FILE* fp = fopen("./dummy_file", "wb");
    if (fp != nullptr) {
      fwrite(Data, 1, Size, fp);
      fclose(fp);
    }
  }

  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (png_ptr == nullptr) return 0;

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr) {
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    return 0;
  }

  struct BufferState {
    const uint8_t* data;
    size_t size;
    size_t offset;
  } state = {Data, Size, 0};

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    return 0;
  }

  png_set_read_fn(png_ptr, &state, ReadCallback);

  png_read_info(png_ptr, info_ptr);

  double file_gamma = 0.0;
  png_get_gAMA(png_ptr, info_ptr, &file_gamma);

  size_t off = state.offset;

  int srgb_intent1 = 0;
  int srgb_intent2 = 0;
  if (off < Size) srgb_intent1 = static_cast<int>(Data[off++]);
  if (off < Size) srgb_intent2 = static_cast<int>(Data[off++]);

  png_set_sRGB(png_ptr, info_ptr, srgb_intent1);
  png_set_sRGB_gAMA_and_cHRM(png_ptr, info_ptr, srgb_intent2);

  png_fixed_point white_x = ReadFixed(Data, Size, off);
  png_fixed_point white_y = ReadFixed(Data, Size, off);
  png_fixed_point red_x = ReadFixed(Data, Size, off);
  png_fixed_point red_y = ReadFixed(Data, Size, off);
  png_fixed_point green_x = ReadFixed(Data, Size, off);
  png_fixed_point green_y = ReadFixed(Data, Size, off);
  png_fixed_point blue_x = ReadFixed(Data, Size, off);
  png_fixed_point blue_y = ReadFixed(Data, Size, off);

  png_set_cHRM_fixed(png_ptr, info_ptr, white_x, white_y, red_x, red_y,
                     green_x, green_y, blue_x, blue_y);

  png_fixed_point int_file_gamma = ReadFixed(Data, Size, off);
  png_set_gAMA_fixed(png_ptr, info_ptr, int_file_gamma);

  double d_white_x = ReadDoubleLike(Data, Size, off);
  double d_white_y = ReadDoubleLike(Data, Size, off);
  double d_red_x = ReadDoubleLike(Data, Size, off);
  double d_red_y = ReadDoubleLike(Data, Size, off);
  double d_green_x = ReadDoubleLike(Data, Size, off);
  double d_green_y = ReadDoubleLike(Data, Size, off);
  double d_blue_x = ReadDoubleLike(Data, Size, off);
  double d_blue_y = ReadDoubleLike(Data, Size, off);

  png_set_cHRM(png_ptr, info_ptr, d_white_x, d_white_y, d_red_x, d_red_y,
               d_green_x, d_green_y, d_blue_x, d_blue_y);

  double file_gamma_after = 0.0;
  png_get_gAMA(png_ptr, info_ptr, &file_gamma_after);

  png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
  return 0;
}