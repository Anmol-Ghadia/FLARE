// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_get_sCAL_s at pngget.c:960:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_init_io at png.c:703:1 in png.h
// png_read_info at pngread.c:92:1 in png.h
// png_get_valid at pngget.c:20:1 in png.h
// png_get_gAMA_fixed at pngget.c:678:1 in png.h
// png_get_bKGD at pngget.c:488:1 in png.h
// png_get_sCAL at pngget.c:944:1 in png.h
// png_get_sCAL_fixed at pngget.c:921:1 in png.h
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

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput_48(const uint8_t *Data, size_t Size) {
  FILE *fp = std::fopen("./dummy_file", "wb");
  if (!fp) return 0;
  if (Size > 0) std::fwrite(Data, 1, Size, fp);
  std::fclose(fp);

  fp = std::fopen("./dummy_file", "rb");
  if (!fp) return 0;

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn, PngWarningFn);
  if (!png_ptr) {
    std::fclose(fp);
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    std::fclose(fp);
    return 0;
  }

  png_infop end_info = png_create_info_struct(png_ptr);
  if (!end_info) {
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    std::fclose(fp);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    std::fclose(fp);
    return 0;
  }

  png_init_io(png_ptr, fp);
  png_read_info(png_ptr, info_ptr);

  png_uint_32 flags[] = {
      0u,
      PNG_INFO_gAMA,
      PNG_INFO_sCAL,
      PNG_INFO_bKGD,
      PNG_INFO_gAMA | PNG_INFO_sCAL,
      PNG_INFO_bKGD | PNG_INFO_sCAL,
      0xffffffffu
  };

  for (size_t i = 0; i < sizeof(flags) / sizeof(flags[0]); ++i) {
    (void)png_get_valid(png_ptr, info_ptr, flags[i]);
  }

#ifdef PNG_gAMA_SUPPORTED
  png_fixed_point gamma = 0;
  (void)png_get_gAMA_fixed(png_ptr, info_ptr, &gamma);
#endif

#ifdef PNG_bKGD_SUPPORTED
  png_color_16p background = nullptr;
  (void)png_get_bKGD(png_ptr, info_ptr, &background);
  if (background) {
    volatile png_uint_16 sink = background->red;
    (void)sink;
  }
#endif

#ifdef PNG_sCAL_SUPPORTED
  int unit = 0;
  double width = 0.0, height = 0.0;
  (void)png_get_sCAL(png_ptr, info_ptr, &unit, &width, &height);

  png_fixed_point fwidth = 0, fheight = 0;
  unit = 0;
  (void)png_get_sCAL_fixed(png_ptr, info_ptr, &unit, &fwidth, &fheight);

  png_charp swidth = nullptr;
  png_charp sheight = nullptr;
  unit = 0;
  (void)png_get_sCAL_s(png_ptr, info_ptr, &unit, &swidth, &sheight);
#endif

  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
  std::fclose(fp);
  return 0;
}