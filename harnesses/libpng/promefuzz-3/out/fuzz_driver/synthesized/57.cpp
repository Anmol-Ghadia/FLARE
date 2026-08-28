// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_quantize at pngrtran.c:428:1 in png.h
// png_set_expand at pngrtran.c:872:1 in png.h
// png_set_tRNS_to_alpha at pngrtran.c:926:1 in png.h
// png_set_scale_16 at pngrtran.c:183:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_init_io at png.c:703:1 in png.h
// png_sig_cmp at png.c:74:1 in png.h
// png_set_sig_bytes at png.c:47:1 in png.h
// png_read_info at pngread.c:92:1 in png.h
// png_get_image_width at pngget.c:52:1 in png.h
// png_get_image_height at pngget.c:61:1 in png.h
// png_get_bit_depth at pngget.c:70:1 in png.h
// png_get_color_type at pngget.c:79:1 in png.h
// png_set_expand at pngrtran.c:872:1 in png.h
// png_set_tRNS_to_alpha at pngrtran.c:926:1 in png.h
// png_set_scale_16 at pngrtran.c:183:1 in png.h
// png_set_PLTE at pngset.c:572:1 in png.h
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

static void WriteDummyFile(const uint8_t* Data, size_t Size) {
  FILE* fp = std::fopen("./dummy_file", "wb");
  if (!fp) return;
  if (Data != nullptr && Size > 0) {
    std::fwrite(Data, 1, Size, fp);
  }
  std::fclose(fp);
}

extern "C" int LLVMFuzzerTestOneInput_57(const uint8_t *Data, size_t Size) {
  WriteDummyFile(Data, Size);

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (!png_ptr) return 0;

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    return 0;
  }

  FILE* fp = std::fopen("./dummy_file", "rb");
  if (!fp) {
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    return 0;
  }

  png_init_io(png_ptr, fp);

  if (Size >= 8) {
    png_byte sig[8];
    for (int i = 0; i < 8; ++i) sig[i] = Data[i];
    if (png_sig_cmp(sig, 0, 8) == 0) {
      png_set_sig_bytes(png_ptr, 0);
    }
  }

  png_read_info(png_ptr, info_ptr);

  png_uint_32 width = png_get_image_width(png_ptr, info_ptr);
  png_uint_32 height = png_get_image_height(png_ptr, info_ptr);
  int bit_depth = png_get_bit_depth(png_ptr, info_ptr);
  int color_type = png_get_color_type(png_ptr, info_ptr);

  png_set_expand(png_ptr);
  png_set_tRNS_to_alpha(png_ptr);
  png_set_scale_16(png_ptr);

  int max_palette = 256;
  if (color_type == PNG_COLOR_TYPE_PALETTE && bit_depth > 0 && bit_depth <= 8) {
    max_palette = 1 << bit_depth;
  }

  std::vector<png_color> palette(256);
  for (size_t i = 0; i < palette.size(); ++i) {
    size_t base = (i * 3) % (Size == 0 ? 1 : Size);
    palette[i].red = Size ? Data[base % Size] : 0;
    palette[i].green = Size ? Data[(base + 1) % Size] : 0;
    palette[i].blue = Size ? Data[(base + 2) % Size] : 0;
  }

  int num_palette = 0;
  if (Size > 0) {
    num_palette = static_cast<int>(Data[0]) % 257;
  }
  if (num_palette > 256) num_palette = 256;

  png_set_PLTE(png_ptr, info_ptr, palette.data(), num_palette);

  std::vector<png_uint_16> histogram(256);
  for (size_t i = 0; i < histogram.size(); ++i) {
    histogram[i] = Size ? static_cast<png_uint_16>(Data[i % Size] * ((i + 1) & 0xff)) : 0;
  }

  int maximum_colors = 0;
  int full_quantize = 0;
  if (Size > 1) maximum_colors = Data[1] % 257;
  if (Size > 2) full_quantize = Data[2] & 1;

  png_set_quantize(png_ptr,
                   palette.data(),
                   num_palette,
                   maximum_colors,
                   histogram.data(),
                   full_quantize);

  if (width > 0 && height > 0) {
    png_set_expand(png_ptr);
    png_set_tRNS_to_alpha(png_ptr);
    png_set_scale_16(png_ptr);
  }

  std::fclose(fp);
  png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
  return 0;
}