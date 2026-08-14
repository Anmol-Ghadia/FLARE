// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_data_freer at png.c:452:1 in png.h
// png_get_iCCP at pngget.c:736:1 in png.h
// png_write_info at pngwrite.c:192:1 in png.h
// png_set_rows at pngset.c:1546:1 in png.h
// png_write_png at pngwrite.c:1338:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_init_io at png.c:703:1 in png.h
// png_set_IHDR at pngset.c:254:1 in png.h
// png_set_PLTE at pngset.c:572:1 in png.h
// png_set_sRGB_gAMA_and_cHRM at pngset.c:665:1 in png.h
// png_set_iCCP at pngset.c:688:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
#include <cstdio>
#include <cstdint>
#include <cstddef>
#include <vector>
#include <setjmp.h>
#include <png.h>

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

static png_uint_32 ReadU32(const uint8_t *data, size_t size, size_t offset,
                           png_uint_32 defval) {
  if (offset + 4 > size) return defval;
  return (static_cast<png_uint_32>(data[offset]) << 24) |
         (static_cast<png_uint_32>(data[offset + 1]) << 16) |
         (static_cast<png_uint_32>(data[offset + 2]) << 8) |
         (static_cast<png_uint_32>(data[offset + 3]));
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    if (Size > 0) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr,
                                                PngErrorFn, PngWarningFn);
  if (png_ptr == nullptr) return 0;

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr) {
    png_destroy_write_struct(&png_ptr, nullptr);
    return 0;
  }

  FILE *out = fopen("./dummy_file", "wb");
  if (out == nullptr) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    fclose(out);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    return 0;
  }

  png_init_io(png_ptr, out);

  png_uint_32 width = (Size > 0 ? (Data[0] % 32) + 1 : 1);
  png_uint_32 height = (Size > 1 ? (Data[1] % 32) + 1 : 1);

  int color_type;
  switch (Size > 2 ? (Data[2] % 5) : 0) {
    case 0: color_type = PNG_COLOR_TYPE_GRAY; break;
    case 1: color_type = PNG_COLOR_TYPE_RGB; break;
    case 2: color_type = PNG_COLOR_TYPE_PALETTE; break;
    case 3: color_type = PNG_COLOR_TYPE_GRAY_ALPHA; break;
    default: color_type = PNG_COLOR_TYPE_RGB_ALPHA; break;
  }

  int bit_depth = 8;
  png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, color_type,
               PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE,
               PNG_FILTER_TYPE_BASE);

  png_color palette[256];
  if (color_type == PNG_COLOR_TYPE_PALETTE) {
    int palette_entries = (Size > 4 ? (Data[4] % 256) : 1);
    if (palette_entries <= 0) palette_entries = 1;
    for (int i = 0; i < palette_entries; ++i) {
      size_t base = 5 + static_cast<size_t>(i) * 3;
      palette[i].red = (base < Size) ? Data[base] : static_cast<png_byte>(i);
      palette[i].green =
          (base + 1 < Size) ? Data[base + 1] : static_cast<png_byte>(255 - i);
      palette[i].blue =
          (base + 2 < Size) ? Data[base + 2] : static_cast<png_byte>(i ^ 0xAA);
    }
    png_set_PLTE(png_ptr, info_ptr, palette, palette_entries);
  }

  int srgb_intent = (Size > 5) ? (Data[5] % 4) : 0;
  png_set_sRGB_gAMA_and_cHRM(png_ptr, info_ptr, srgb_intent);

  const char *icc_name = "fuzz-icc";
  const png_const_bytep profile = reinterpret_cast<png_const_bytep>(Data);
  png_uint_32 proflen = static_cast<png_uint_32>(Size > 1024 ? 1024 : Size);
  png_set_iCCP(png_ptr, info_ptr, icc_name, PNG_COMPRESSION_TYPE_BASE, profile,
               proflen);

  int freer_choice = (Size > 6) ? (Data[6] % 4) : 0;
  int freer;
  switch (freer_choice) {
    case 0: freer = PNG_DESTROY_WILL_FREE_DATA; break;
    case 1: freer = PNG_USER_WILL_FREE_DATA; break;
    case 2: freer = -1; break;
    default: freer = 12345; break;
  }

  png_uint_32 mask = (Size > 10) ? ReadU32(Data, Size, 7, PNG_FREE_ALL)
                                 : static_cast<png_uint_32>(PNG_FREE_ALL);
  png_data_freer(png_ptr, info_ptr, freer, mask);

  char *out_name = nullptr;
  int compression_type = -1;
  png_byte *out_profile = nullptr;
  png_uint_32 out_proflen = 0;
  (void)png_get_iCCP(png_ptr, info_ptr, &out_name, &compression_type,
                     &out_profile, &out_proflen);

  png_write_info(png_ptr, info_ptr);

  int channels = 1;
  switch (color_type) {
    case PNG_COLOR_TYPE_GRAY: channels = 1; break;
    case PNG_COLOR_TYPE_RGB: channels = 3; break;
    case PNG_COLOR_TYPE_PALETTE: channels = 1; break;
    case PNG_COLOR_TYPE_GRAY_ALPHA: channels = 2; break;
    case PNG_COLOR_TYPE_RGB_ALPHA: channels = 4; break;
    default: channels = 1; break;
  }

  size_t rowbytes = static_cast<size_t>(width) * static_cast<size_t>(channels);
  if (rowbytes == 0) rowbytes = 1;

  std::vector<png_byte> image(rowbytes * height);
  std::vector<png_bytep> rows(height);

  for (png_uint_32 y = 0; y < height; ++y) {
    rows[y] = image.data() + static_cast<size_t>(y) * rowbytes;
    for (size_t x = 0; x < rowbytes; ++x) {
      size_t idx = static_cast<size_t>(y) * rowbytes + x;
      rows[y][x] =
          (idx < Size) ? Data[idx] : static_cast<png_byte>((x + y) & 0xFF);
    }
  }

  png_set_rows(png_ptr, info_ptr, rows.data());

  int transforms = 0;
  if (Size > 11) {
    if (Data[8] & 1) transforms |= PNG_TRANSFORM_BGR;
    if (Data[9] & 1) transforms |= PNG_TRANSFORM_SWAP_ALPHA;
    if (Data[10] & 1) transforms |= PNG_TRANSFORM_INVERT_ALPHA;
    if (Data[11] & 1) transforms |= PNG_TRANSFORM_PACKING;
  }

  png_write_png(png_ptr, info_ptr, transforms, nullptr);

  fclose(out);
  png_destroy_write_struct(&png_ptr, &info_ptr);
  return 0;
}