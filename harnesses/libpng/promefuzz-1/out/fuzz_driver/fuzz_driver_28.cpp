// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_init_io at png.c:703:1 in png.h
// png_set_expand_gray_1_2_4_to_8 at pngrtran.c:914:1 in png.h
// png_set_gray_to_rgb at pngrtran.c:955:1 in png.h
// png_set_strip_alpha at pngrtran.c:210:1 in png.h
// png_set_scale_16 at pngrtran.c:183:1 in png.h
// png_set_invert_alpha at pngtrans.c:237:1 in png.h
// png_read_info at pngread.c:92:1 in png.h
// png_set_expand_gray_1_2_4_to_8 at pngrtran.c:914:1 in png.h
// png_set_gray_to_rgb at pngrtran.c:955:1 in png.h
// png_set_strip_alpha at pngrtran.c:210:1 in png.h
// png_set_scale_16 at pngrtran.c:183:1 in png.h
// png_set_invert_alpha at pngtrans.c:237:1 in png.h
// png_read_update_info at pngread.c:267:1 in png.h
// png_start_read_image at pngread.c:298:1 in png.h
// png_start_read_image at pngread.c:298:1 in png.h
// png_get_image_width at pngget.c:52:1 in png.h
// png_get_image_height at pngget.c:61:1 in png.h
// png_get_rowbytes at pngget.c:30:1 in png.h
// png_read_image at pngread.c:700:1 in png.h
// png_read_end at pngread.c:766:1 in png.h
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
#include <vector>

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE *fp = std::fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    if (Size > 0) {
      std::fwrite(Data, 1, Size, fp);
    }
    std::fclose(fp);
  }

  fp = std::fopen("./dummy_file", "rb");
  if (fp == nullptr) {
    return 0;
  }

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn, PngWarningFn);
  if (png_ptr == nullptr) {
    std::fclose(fp);
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  png_infop end_info = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr || end_info == nullptr) {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    std::fclose(fp);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    std::fclose(fp);
    return 0;
  }

  png_init_io(png_ptr, fp);

  png_set_expand_gray_1_2_4_to_8(png_ptr);
  png_set_gray_to_rgb(png_ptr);
  png_set_strip_alpha(png_ptr);
  png_set_scale_16(png_ptr);
  png_set_invert_alpha(png_ptr);

  png_read_info(png_ptr, info_ptr);

  if (Size > 0) {
    if (Data[0] & 0x01) png_set_expand_gray_1_2_4_to_8(png_ptr);
    if (Data[0] & 0x02) png_set_gray_to_rgb(png_ptr);
    if (Data[0] & 0x04) png_set_strip_alpha(png_ptr);
    if (Data[0] & 0x08) png_set_scale_16(png_ptr);
    if (Data[0] & 0x10) png_set_invert_alpha(png_ptr);
  }

  png_read_update_info(png_ptr, info_ptr);

  png_start_read_image(png_ptr);
  if (Size > 1 && (Data[1] & 0x01)) {
    png_start_read_image(png_ptr);
  }

  png_uint_32 width = png_get_image_width(png_ptr, info_ptr);
  png_uint_32 height = png_get_image_height(png_ptr, info_ptr);
  png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);

  if (height > 0 && rowbytes > 0 && rowbytes < (1u << 26) && height < (1u << 20)) {
    std::vector<png_bytep> rows(height, nullptr);
    std::vector<uint8_t> image;
    size_t total = static_cast<size_t>(rowbytes) * static_cast<size_t>(height);
    if (total / rowbytes == height) {
      image.resize(total);
      for (png_uint_32 y = 0; y < height; ++y) {
        rows[y] = image.data() + static_cast<size_t>(y) * rowbytes;
      }
      png_read_image(png_ptr, rows.data());
      png_read_end(png_ptr, end_info);
    }
  }

  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
  std::fclose(fp);
  return 0;
}