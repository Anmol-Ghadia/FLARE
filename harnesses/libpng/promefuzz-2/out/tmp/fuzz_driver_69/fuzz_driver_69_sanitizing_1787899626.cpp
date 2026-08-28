#include <cstdio>
#include <cstdint>
#include <cstddef>
#include <csetjmp>
#include <png.h>

static uint32_t ReadU32(const uint8_t *data, size_t size, size_t *off) {
  if (*off + 4 > size) return 0;
  uint32_t v = (uint32_t)data[*off] |
               ((uint32_t)data[*off + 1] << 8) |
               ((uint32_t)data[*off + 2] << 16) |
               ((uint32_t)data[*off + 3] << 24);
  *off += 4;
  return v;
}

static uint64_t ReadU64(const uint8_t *data, size_t size, size_t *off) {
  uint64_t lo = ReadU32(data, size, off);
  uint64_t hi = ReadU32(data, size, off);
  return lo | (hi << 32);
}

static int32_t ReadS32(const uint8_t *data, size_t size, size_t *off) {
  return (int32_t)ReadU32(data, size, off);
}

static double ReadDouble(const uint8_t *data, size_t size, size_t *off) {
  union {
    uint64_t u;
    double d;
  } conv;
  conv.u = ReadU64(data, size, off);
  return conv.d;
}

static void UserWarningFn(png_structp, png_const_charp) {}

static void UserErrorFn(png_structp png_ptr, png_const_charp) {
  if (png_ptr) longjmp(png_jmpbuf(png_ptr), 1);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Data && Size) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  png_structp png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, UserErrorFn, UserWarningFn);
  if (!png_ptr) return 0;

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    png_destroy_write_struct(&png_ptr, NULL);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    return 0;
  }

  size_t off = 0;

  double white_x = ReadDouble(Data, Size, &off);
  double white_y = ReadDouble(Data, Size, &off);
  double red_x = ReadDouble(Data, Size, &off);
  double red_y = ReadDouble(Data, Size, &off);
  double green_x = ReadDouble(Data, Size, &off);
  double green_y = ReadDouble(Data, Size, &off);
  double blue_x = ReadDouble(Data, Size, &off);
  double blue_y = ReadDouble(Data, Size, &off);

  double red_X = ReadDouble(Data, Size, &off);
  double red_Y = ReadDouble(Data, Size, &off);
  double red_Z = ReadDouble(Data, Size, &off);
  double green_X = ReadDouble(Data, Size, &off);
  double green_Y = ReadDouble(Data, Size, &off);
  double green_Z = ReadDouble(Data, Size, &off);
  double blue_X = ReadDouble(Data, Size, &off);
  double blue_Y = ReadDouble(Data, Size, &off);
  double blue_Z = ReadDouble(Data, Size, &off);

  double gamma = ReadDouble(Data, Size, &off);
  int srgb_intent = ReadS32(Data, Size, &off);

  png_set_IHDR(png_ptr, info_ptr, 1, 1, 8, PNG_COLOR_TYPE_RGB,
               PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE,
               PNG_FILTER_TYPE_BASE);

#ifdef PNG_cHRM_SUPPORTED
  png_set_cHRM(png_ptr, info_ptr, white_x, white_y, red_x, red_y,
               green_x, green_y, blue_x, blue_y);

  png_set_cHRM_XYZ(png_ptr, info_ptr, red_X, red_Y, red_Z,
                   green_X, green_Y, green_Z, blue_X, blue_Y, blue_Z);

  if (off < Size) {
    png_set_cHRM(png_ptr, info_ptr,
                 -white_x, -white_y, -red_x, -red_y,
                 -green_x, -green_y, -blue_x, -blue_y);
  }

  if (off + 1 < Size) {
    png_set_cHRM_XYZ(png_ptr, info_ptr,
                     blue_X, blue_Y, blue_Z,
                     red_X, red_Y, red_Z,
                     green_X, green_Y, green_Z);
  }
#endif

#ifdef PNG_gAMA_SUPPORTED
  png_set_gAMA(png_ptr, info_ptr, gamma);
  if (off + 2 < Size) {
    png_set_gAMA(png_ptr, info_ptr, -gamma);
  }
#endif

#ifdef PNG_sRGB_SUPPORTED
  png_set_sRGB_gAMA_and_cHRM(png_ptr, info_ptr, srgb_intent);
  if (off + 3 < Size) {
    png_set_sRGB_gAMA_and_cHRM(png_ptr, info_ptr, srgb_intent & 3);
  }
#endif

#ifdef PNG_cHRM_SUPPORTED
  double o_white_x = 0.0, o_white_y = 0.0, o_red_x = 0.0, o_red_y = 0.0;
  double o_green_x = 0.0, o_green_y = 0.0, o_blue_x = 0.0, o_blue_y = 0.0;
  png_get_cHRM(png_ptr, info_ptr,
               &o_white_x, &o_white_y,
               &o_red_x, &o_red_y,
               &o_green_x, &o_green_y,
               &o_blue_x, &o_blue_y);

  png_get_cHRM(png_ptr, info_ptr,
               NULL, NULL,
               &o_red_x, NULL,
               NULL, &o_green_y,
               NULL, NULL);

  double o_red_X = 0.0, o_red_Y = 0.0, o_red_Z = 0.0;
  double o_green_X = 0.0, o_green_Y = 0.0, o_green_Z = 0.0;
  double o_blue_X = 0.0, o_blue_Y = 0.0, o_blue_Z = 0.0;
  png_get_cHRM_XYZ(png_ptr, info_ptr,
                   &o_red_X, &o_red_Y, &o_red_Z,
                   &o_green_X, &o_green_Y, &o_green_Z,
                   &o_blue_X, &o_blue_Y, &o_blue_Z);

  png_get_cHRM_XYZ(png_ptr, info_ptr,
                   NULL, &o_red_Y, NULL,
                   &o_green_X, NULL, &o_green_Z,
                   NULL, NULL, &o_blue_Z);
#endif

  png_destroy_write_struct(&png_ptr, &info_ptr);
  return 0;
}