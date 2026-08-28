// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_sig_bytes at png.c:47:1 in png.h
// png_read_info at pngread.c:92:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_get_valid at pngget.c:20:1 in png.h
// png_get_valid at pngget.c:20:1 in png.h
// png_get_x_offset_microns at pngget.c:236:1 in png.h
// png_get_x_offset_pixels at pngget.c:276:1 in png.h
// png_get_signature at pngget.c:477:1 in png.h
// png_get_y_offset_inches_fixed at pngget.c:389:1 in png.h
// png_get_pHYs_dpi at pngget.c:421:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_read_fn at pngrio.c:87:1 in png.h
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

static void user_read_data(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead) {
  if (png_ptr == nullptr || outBytes == nullptr) return;

  struct BufferState {
    const uint8_t* data;
    size_t size;
    size_t offset;
  };

  BufferState* state =
      reinterpret_cast<BufferState*>(png_get_io_ptr(png_ptr));
  if (state == nullptr) {
    png_error(png_ptr, "No IO state");
    return;
  }

  if (byteCountToRead > state->size - state->offset) {
    png_error(png_ptr, "Read beyond end of buffer");
    return;
  }

  memcpy(outBytes, state->data + state->offset, byteCountToRead);
  state->offset += byteCountToRead;
}

static void exercise_getters(png_structp png_ptr, png_infop info_ptr) {
  if (png_ptr == nullptr || info_ptr == nullptr) return;

  (void)png_get_valid(png_ptr, info_ptr, PNG_INFO_oFFs);
#ifdef PNG_INFO_pHYs
  (void)png_get_valid(png_ptr, info_ptr, PNG_INFO_pHYs);
#endif
#ifdef PNG_INFO_IHDR
  (void)png_get_valid(png_ptr, info_ptr, PNG_INFO_IHDR);
#endif

  (void)png_get_x_offset_microns(png_ptr, info_ptr);
  (void)png_get_x_offset_pixels(png_ptr, info_ptr);
  (void)png_get_signature(png_ptr, info_ptr);
  (void)png_get_y_offset_inches_fixed(png_ptr, info_ptr);

#ifdef PNG_pHYs_SUPPORTED
  png_uint_32 res_x = 0, res_y = 0;
  int unit_type = 0;
  (void)png_get_pHYs_dpi(png_ptr, info_ptr, &res_x, &res_y, &unit_type);
#endif
}

extern "C" int LLVMFuzzerTestOneInput_46(const uint8_t *Data, size_t Size) {
  FILE* fp = fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    if (Data != nullptr && Size > 0) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (png_ptr == nullptr) {
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr) {
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    exercise_getters(png_ptr, info_ptr);
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    return 0;
  }

  struct BufferState {
    const uint8_t* data;
    size_t size;
    size_t offset;
  } state = { Data, Size, 0 };

  png_set_read_fn(png_ptr, &state, user_read_data);

  if (Size >= 8) {
    png_set_sig_bytes(png_ptr, 0);
  }

  png_read_info(png_ptr, info_ptr);
  exercise_getters(png_ptr, info_ptr);

  png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
  return 0;
}