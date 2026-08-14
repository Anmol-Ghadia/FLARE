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

static void png_noop_error(png_structp, png_const_charp) {}
static void png_noop_warning(png_structp, png_const_charp) {}

static void write_dummy_file(const uint8_t *Data, size_t Size) {
  FILE *fp = std::fopen("./dummy_file", "wb");
  if (!fp) return;
  if (Size > 0) std::fwrite(Data, 1, Size, fp);
  std::fclose(fp);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  write_dummy_file(Data, Size);

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr,
                             png_noop_error, png_noop_warning);
  if (!png_ptr) return 0;

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_info_struct(png_ptr, &info_ptr);
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    return 0;
  }

  png_bytep exif_buf = nullptr;
  png_uint_32 exif_len = 0;

  if (Size > 0) {
    exif_len = static_cast<png_uint_32>(Size);
    exif_buf = static_cast<png_bytep>(std::malloc(exif_len));
    if (exif_buf) std::memcpy(exif_buf, Data, exif_len);
  }

  png_set_eXIf(png_ptr, info_ptr, exif_buf);

  if (exif_buf) {
    png_set_eXIf_1(png_ptr, info_ptr, exif_len, exif_buf);
  } else {
    png_set_eXIf_1(png_ptr, info_ptr, 0, nullptr);
  }

  if (Size >= 4) {
    png_uint_32 alt_len =
        static_cast<png_uint_32>((static_cast<unsigned>(Data[0]) << 24) ^
                                 (static_cast<unsigned>(Data[1]) << 16) ^
                                 (static_cast<unsigned>(Data[2]) << 8) ^
                                 static_cast<unsigned>(Data[3]));
    if (alt_len > Size) alt_len = static_cast<png_uint_32>(Size);
    png_set_eXIf_1(png_ptr, info_ptr, alt_len, exif_buf);
  }

  png_bytep exif_out = nullptr;
  png_uint_32 num_exif_out = 0;
  (void)png_get_eXIf(png_ptr, info_ptr, &exif_out);
  (void)png_get_eXIf_1(png_ptr, info_ptr, &num_exif_out, &exif_out);

  png_charp iccp_name = nullptr;
  int compression_type = 0;
  png_bytep profile = nullptr;
  png_uint_32 profile_len = 0;
  (void)png_get_iCCP(png_ptr, info_ptr, &iccp_name, &compression_type,
                     &profile, &profile_len);

  png_destroy_info_struct(png_ptr, &info_ptr);
  png_destroy_info_struct(png_ptr, &info_ptr);

  std::free(exif_buf);
  png_destroy_read_struct(&png_ptr, nullptr, nullptr);
  return 0;
}