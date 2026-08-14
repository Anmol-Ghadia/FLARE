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
#include <vector>

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

static void ReadDataFn(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead) {
  struct MemoryReader {
    const uint8_t* data;
    size_t size;
    size_t offset;
  };

  MemoryReader* reader =
      static_cast<MemoryReader*>(png_get_io_ptr(png_ptr));
  if (reader == nullptr || reader->offset > reader->size ||
      byteCountToRead > reader->size - reader->offset) {
    png_error(png_ptr, "read error");
    return;
  }

  memcpy(outBytes, reader->data + reader->offset, byteCountToRead);
  reader->offset += byteCountToRead;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  if (Data == nullptr || Size == 0) {
    return 0;
  }

  FILE* fp = fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr,
                                               PngErrorFn, PngWarningFn);
  if (png_ptr == nullptr) {
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr) {
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    return 0;
  }

  struct MemoryReader {
    const uint8_t* data;
    size_t size;
    size_t offset;
  } reader = {Data, Size, 0};

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    return 0;
  }

  png_set_read_fn(png_ptr, &reader, ReadDataFn);

  if (Size > 8) {
    size_t sig_bytes = Data[0] % 8;
    if (sig_bytes > Size) sig_bytes = Size;
    png_set_sig_bytes(png_ptr, static_cast<int>(sig_bytes));
    reader.offset = sig_bytes;
  }

  png_read_info(png_ptr, info_ptr);

  png_uint_32 res_x = 0, res_y = 0;
  int unit_type = 0;

  (void)png_get_pHYs(png_ptr, info_ptr, &res_x, &res_y, &unit_type);
  (void)png_get_pHYs(png_ptr, info_ptr, nullptr, &res_y, &unit_type);
  (void)png_get_pHYs(png_ptr, info_ptr, &res_x, nullptr, &unit_type);
  (void)png_get_pHYs(png_ptr, info_ptr, &res_x, &res_y, nullptr);
  (void)png_get_pHYs(png_ptr, info_ptr, nullptr, nullptr, nullptr);

  (void)png_get_x_pixels_per_meter(png_ptr, info_ptr);
  (void)png_get_y_pixels_per_meter(png_ptr, info_ptr);
  (void)png_get_pixel_aspect_ratio(png_ptr, info_ptr);
  (void)png_get_x_offset_inches(png_ptr, info_ptr);

#ifdef PNG_FIXED_POINT_SUPPORTED
  (void)png_get_x_offset_inches_fixed(png_ptr, info_ptr);
  (void)png_get_y_offset_inches_fixed(png_ptr, info_ptr);
  (void)png_get_pixel_aspect_ratio_fixed(png_ptr, info_ptr);
#endif

#ifdef PNG_pHYs_SUPPORTED
  (void)png_get_pHYs_dpi(png_ptr, info_ptr, &res_x, &res_y, &unit_type);
  (void)png_get_pHYs_dpi(png_ptr, info_ptr, nullptr, &res_y, &unit_type);
  (void)png_get_pHYs_dpi(png_ptr, info_ptr, &res_x, nullptr, nullptr);
#endif

  png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
  return 0;
}