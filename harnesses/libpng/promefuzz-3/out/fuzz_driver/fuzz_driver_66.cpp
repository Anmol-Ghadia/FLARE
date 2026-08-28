// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_get_pHYs_dpi at pngget.c:421:1 in png.h
// png_get_pHYs_dpi at pngget.c:421:1 in png.h
// png_get_pHYs_dpi at pngget.c:421:1 in png.h
// png_get_pHYs_dpi at pngget.c:421:1 in png.h
// png_get_x_pixels_per_inch at pngget.c:355:1 in png.h
// png_get_pixel_aspect_ratio at pngget.c:181:1 in png.h
// png_get_pixel_aspect_ratio_fixed at pngget.c:205:1 in png.h
// png_get_y_offset_inches at pngget.c:410:1 in png.h
// png_get_y_offset_inches_fixed at pngget.c:389:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_read_fn at pngrio.c:87:1 in png.h
// png_set_sig_bytes at png.c:47:1 in png.h
// png_read_info at pngread.c:92:1 in png.h
// png_get_pHYs at pngget.c:978:1 in png.h
// png_get_pHYs at pngget.c:978:1 in png.h
// png_get_pHYs at pngget.c:978:1 in png.h
// png_get_pHYs at pngget.c:978:1 in png.h
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

static void ReadCallback(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead) {
  if (png_ptr == nullptr || outBytes == nullptr) {
    png_error(png_ptr, "Invalid read callback arguments");
    return;
  }

  struct BufferState {
    const uint8_t* data;
    size_t size;
    size_t offset;
  };

  BufferState* state =
      reinterpret_cast<BufferState*>(png_get_io_ptr(png_ptr));
  if (state == nullptr) {
    png_error(png_ptr, "Missing IO state");
    return;
  }

  if (byteCountToRead > state->size - state->offset) {
    png_error(png_ptr, "Read beyond end of input");
    return;
  }

  memcpy(outBytes, state->data + state->offset, byteCountToRead);
  state->offset += byteCountToRead;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE* fp = fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    if (Size > 0) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  if (Size < 8) {
    return 0;
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
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    return 0;
  }

  struct BufferState {
    const uint8_t* data;
    size_t size;
    size_t offset;
  } state{Data, Size, 0};

  png_set_read_fn(png_ptr, &state, ReadCallback);

  if (Size >= 8 && (Data[0] & 1)) {
    png_byte sig[8];
    memcpy(sig, Data, 8);
    state.offset = 8;
    png_set_sig_bytes(png_ptr, 8);
  }

  png_read_info(png_ptr, info_ptr);

  png_uint_32 res_x = 0, res_y = 0;
  int unit_type = 0;
  png_uint_32 ret1 = png_get_pHYs(png_ptr, info_ptr, &res_x, &res_y, &unit_type);
  png_uint_32 ret2 = png_get_pHYs(png_ptr, info_ptr, &res_x, nullptr, &unit_type);
  png_uint_32 ret3 = png_get_pHYs(png_ptr, info_ptr, nullptr, &res_y, nullptr);
  png_uint_32 ret4 = png_get_pHYs(png_ptr, info_ptr, nullptr, nullptr, nullptr);

  png_uint_32 dpi_x = 0, dpi_y = 0;
  int dpi_unit = 0;
  png_uint_32 ret5 = png_get_pHYs_dpi(png_ptr, info_ptr, &dpi_x, &dpi_y, &dpi_unit);
  png_uint_32 ret6 = png_get_pHYs_dpi(png_ptr, info_ptr, &dpi_x, nullptr, &dpi_unit);
  png_uint_32 ret7 = png_get_pHYs_dpi(png_ptr, info_ptr, nullptr, &dpi_y, nullptr);
  png_uint_32 ret8 = png_get_pHYs_dpi(png_ptr, info_ptr, nullptr, nullptr, nullptr);

  png_uint_32 x_ppi = png_get_x_pixels_per_inch(png_ptr, info_ptr);
  float aspect = png_get_pixel_aspect_ratio(png_ptr, info_ptr);
#ifdef PNG_FIXED_POINT_SUPPORTED
  png_fixed_point aspect_fixed = png_get_pixel_aspect_ratio_fixed(png_ptr, info_ptr);
  (void)aspect_fixed;
#endif
  float y_offset_inches = png_get_y_offset_inches(png_ptr, info_ptr);
#ifdef PNG_FIXED_POINT_SUPPORTED
  png_fixed_point y_offset_inches_fixed = png_get_y_offset_inches_fixed(png_ptr, info_ptr);
  (void)y_offset_inches_fixed;
#endif

  volatile png_uint_32 sink_u32 =
      ret1 ^ ret2 ^ ret3 ^ ret4 ^ ret5 ^ ret6 ^ ret7 ^ ret8 ^ x_ppi ^
      res_x ^ res_y ^ dpi_x ^ dpi_y ^ static_cast<png_uint_32>(unit_type) ^
      static_cast<png_uint_32>(dpi_unit);
  volatile float sink_f = aspect + y_offset_inches;
  (void)sink_u32;
  (void)sink_f;

  png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
  return 0;
}