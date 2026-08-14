// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_get_IHDR at pngget.c:825:1 in png.h
// png_get_sBIT at pngget.c:1031:1 in png.h
// png_get_sBIT at pngget.c:1031:1 in png.h
// png_get_sBIT at pngget.c:1031:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_init_io at png.c:703:1 in png.h
// png_read_info at pngread.c:92:1 in png.h
// png_get_bit_depth at pngget.c:70:1 in png.h
// png_get_interlace_type at pngget.c:97:1 in png.h
// png_get_filter_type at pngget.c:88:1 in png.h
// png_get_IHDR at pngget.c:825:1 in png.h
// png_get_IHDR at pngget.c:825:1 in png.h
// png_get_IHDR at pngget.c:825:1 in png.h
// png_get_valid at pngget.c:20:1 in png.h
// png_get_sBIT at pngget.c:1031:1 in png.h
// png_get_bit_depth at pngget.c:70:1 in png.h
// png_get_bit_depth at pngget.c:70:1 in png.h
// png_get_interlace_type at pngget.c:97:1 in png.h
// png_get_interlace_type at pngget.c:97:1 in png.h
// png_get_filter_type at pngget.c:88:1 in png.h
// png_get_filter_type at pngget.c:88:1 in png.h
// png_get_valid at pngget.c:20:1 in png.h
// png_get_valid at pngget.c:20:1 in png.h
// png_get_IHDR at pngget.c:825:1 in png.h
#include <png.h>

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <setjmp.h>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE *fp = std::fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    if (Data != nullptr && Size > 0) {
      std::fwrite(Data, 1, Size, fp);
    }
    std::fclose(fp);
  }

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
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

  fp = std::fopen("./dummy_file", "rb");
  if (fp != nullptr) {
    png_init_io(png_ptr, fp);
    png_read_info(png_ptr, info_ptr);

    (void)png_get_bit_depth(png_ptr, info_ptr);
    (void)png_get_interlace_type(png_ptr, info_ptr);
    (void)png_get_filter_type(png_ptr, info_ptr);

    png_uint_32 width = 0;
    png_uint_32 height = 0;
    int bit_depth = 0;
    int color_type = 0;
    int interlace_method = 0;
    int compression_method = 0;
    int filter_method = 0;

    (void)png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth,
                       &color_type, &interlace_method, &compression_method,
                       &filter_method);
    (void)png_get_IHDR(png_ptr, info_ptr, nullptr, &height, nullptr,
                       &color_type, nullptr, &compression_method, nullptr);
    (void)png_get_IHDR(png_ptr, info_ptr, &width, nullptr, &bit_depth, nullptr,
                       &interlace_method, nullptr, &filter_method);

    png_uint_32 flags[5];
    flags[0] = 0;
    flags[1] = PNG_INFO_sBIT;
    flags[2] = 0xffffffffu;
    flags[3] = static_cast<png_uint_32>(Size);
    flags[4] = 0;
    if (Size >= sizeof(png_uint_32)) {
      std::memcpy(&flags[4], Data, sizeof(png_uint_32));
    }

    for (size_t i = 0; i < 5; ++i) {
      (void)png_get_valid(png_ptr, info_ptr, flags[i]);
    }

    png_color_8p sig_bit_ptr = nullptr;
    (void)png_get_sBIT(png_ptr, info_ptr, &sig_bit_ptr);

    std::fclose(fp);
  }

  (void)png_get_bit_depth(nullptr, info_ptr);
  (void)png_get_bit_depth(png_ptr, nullptr);
  (void)png_get_interlace_type(nullptr, info_ptr);
  (void)png_get_interlace_type(png_ptr, nullptr);
  (void)png_get_filter_type(nullptr, info_ptr);
  (void)png_get_filter_type(png_ptr, nullptr);
  (void)png_get_valid(nullptr, info_ptr, PNG_INFO_sBIT);
  (void)png_get_valid(png_ptr, nullptr, PNG_INFO_sBIT);

  png_uint_32 width2 = 0;
  png_uint_32 height2 = 0;
  int bit_depth2 = 0;
  int color_type2 = 0;
  int interlace_method2 = 0;
  int compression_method2 = 0;
  int filter_method2 = 0;

  (void)png_get_IHDR(nullptr, info_ptr, &width2, &height2, &bit_depth2,
                     &color_type2, &interlace_method2, &compression_method2,
                     &filter_method2);
  (void)png_get_IHDR(png_ptr, nullptr, &width2, &height2, &bit_depth2,
                     &color_type2, &interlace_method2, &compression_method2,
                     &filter_method2);

  png_color_8p sig_bit_ptr2 = nullptr;
  (void)png_get_sBIT(nullptr, info_ptr, &sig_bit_ptr2);
  (void)png_get_sBIT(png_ptr, nullptr, &sig_bit_ptr2);
  (void)png_get_sBIT(png_ptr, info_ptr, nullptr);

  png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
  return 0;
}