// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_build_grayscale_palette at png.c:875:1 in png.h
// png_get_PLTE at pngget.c:1012:1 in png.h
// png_get_PLTE at pngget.c:1012:1 in png.h
// png_get_PLTE at pngget.c:1012:1 in png.h
// png_get_PLTE at pngget.c:1012:1 in png.h
// png_get_PLTE at pngget.c:1012:1 in png.h
// png_build_grayscale_palette at png.c:875:1 in png.h
// png_set_PLTE at pngset.c:572:1 in png.h
// png_get_PLTE at pngget.c:1012:1 in png.h
// png_set_tRNS at pngset.c:994:1 in png.h
// png_set_tRNS at pngset.c:994:1 in png.h
// png_write_info_before_PLTE at pngwrite.c:84:1 in png.h
// png_write_info_before_PLTE at pngwrite.c:84:1 in png.h
// png_write_info at pngwrite.c:192:1 in png.h
// png_write_info_before_PLTE at pngwrite.c:84:1 in png.h
// png_write_info at pngwrite.c:192:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_init_io at png.c:703:1 in png.h
// png_set_IHDR at pngset.c:254:1 in png.h
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
#include <setjmp.h>

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE *fp = std::fopen("./dummy_file", "wb+");
  if (fp == nullptr) {
    return 0;
  }

  if (Size > 0) {
    std::fwrite(Data, 1, Size, fp);
    std::fflush(fp);
    std::rewind(fp);
  }

  png_structp png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn, PngWarningFn);
  if (png_ptr == nullptr) {
    std::fclose(fp);
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr) {
    png_destroy_write_struct(&png_ptr, nullptr);
    std::fclose(fp);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    std::fclose(fp);
    return 0;
  }

  png_init_io(png_ptr, fp);

  size_t idx = 0;
  auto next_byte = [&]() -> uint8_t {
    if (idx < Size) return Data[idx++];
    return 0;
  };

  int width = (next_byte() % 8) + 1;
  int height = (next_byte() % 8) + 1;

  const int color_types[] = {
      PNG_COLOR_TYPE_GRAY,
      PNG_COLOR_TYPE_PALETTE,
      PNG_COLOR_TYPE_RGB,
      PNG_COLOR_TYPE_GRAY_ALPHA,
      PNG_COLOR_TYPE_RGB_ALPHA
  };
  int color_type = color_types[next_byte() % (sizeof(color_types) / sizeof(color_types[0]))];

  int bit_depth;
  if (color_type == PNG_COLOR_TYPE_PALETTE) {
    const int palette_depths[] = {1, 2, 4, 8};
    bit_depth = palette_depths[next_byte() % 4];
  } else if (color_type == PNG_COLOR_TYPE_GRAY) {
    const int gray_depths[] = {1, 2, 4, 8, 16};
    bit_depth = gray_depths[next_byte() % 5];
  } else {
    bit_depth = (next_byte() & 1) ? 8 : 16;
  }

  png_set_IHDR(png_ptr, info_ptr,
               static_cast<png_uint_32>(width),
               static_cast<png_uint_32>(height),
               bit_depth, color_type,
               PNG_INTERLACE_NONE,
               PNG_COMPRESSION_TYPE_BASE,
               PNG_FILTER_TYPE_BASE);

  png_color palette_storage[256];
  std::memset(palette_storage, 0, sizeof(palette_storage));

  int gray_bd = 1 << (next_byte() % 3); // 1,2,4
  png_build_grayscale_palette(gray_bd, palette_storage);

  png_colorp palette_out = nullptr;
  int num_palette_out = 0;
  (void)png_get_PLTE(png_ptr, info_ptr, &palette_out, &num_palette_out);
  (void)png_get_PLTE(nullptr, info_ptr, &palette_out, &num_palette_out);
  (void)png_get_PLTE(png_ptr, nullptr, &palette_out, &num_palette_out);
  (void)png_get_PLTE(png_ptr, info_ptr, nullptr, &num_palette_out);
  (void)png_get_PLTE(png_ptr, info_ptr, &palette_out, nullptr);

  if (color_type == PNG_COLOR_TYPE_PALETTE || (next_byte() & 1)) {
    int max_palette = (color_type == PNG_COLOR_TYPE_PALETTE) ? (1 << bit_depth) : 256;
    int requested = (Size > idx) ? static_cast<int>(next_byte()) : 0;
    int num_palette = max_palette > 0 ? (requested % (max_palette + 1)) : 0;

    if ((next_byte() & 1) == 0) {
      png_build_grayscale_palette(gray_bd, palette_storage);
    } else {
      for (int i = 0; i < num_palette; ++i) {
        palette_storage[i].red = next_byte();
        palette_storage[i].green = next_byte();
        palette_storage[i].blue = next_byte();
      }
    }

    png_set_PLTE(png_ptr, info_ptr, palette_storage, num_palette);

    png_colorp got_palette = nullptr;
    int got_num_palette = 0;
    (void)png_get_PLTE(png_ptr, info_ptr, &got_palette, &got_num_palette);

    if ((next_byte() & 1) && num_palette > 0) {
      std::vector<png_byte> trans_alpha(static_cast<size_t>(num_palette));
      for (int i = 0; i < num_palette; ++i) {
        trans_alpha[static_cast<size_t>(i)] = next_byte();
      }
      png_set_tRNS(png_ptr, info_ptr, trans_alpha.data(), num_palette, nullptr);
    }
  } else {
    png_color_16 trans_color;
    std::memset(&trans_color, 0, sizeof(trans_color));
    trans_color.index = next_byte();
    trans_color.red = static_cast<png_uint_16>((next_byte() << 8) | next_byte());
    trans_color.green = static_cast<png_uint_16>((next_byte() << 8) | next_byte());
    trans_color.blue = static_cast<png_uint_16>((next_byte() << 8) | next_byte());
    trans_color.gray = static_cast<png_uint_16>((next_byte() << 8) | next_byte());
    png_set_tRNS(png_ptr, info_ptr, nullptr, 0, &trans_color);
  }

  if (next_byte() & 1) {
    png_write_info_before_PLTE(png_ptr, info_ptr);
    png_write_info_before_PLTE(png_ptr, info_ptr);
  }

  if (next_byte() & 1) {
    png_write_info(png_ptr, info_ptr);
  } else {
    png_write_info_before_PLTE(png_ptr, info_ptr);
    png_write_info(png_ptr, info_ptr);
  }

  png_destroy_write_struct(&png_ptr, &info_ptr);
  std::fclose(fp);
  return 0;
}