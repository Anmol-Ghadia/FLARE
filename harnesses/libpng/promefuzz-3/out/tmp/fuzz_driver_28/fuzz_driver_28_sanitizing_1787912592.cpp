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
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

static double ConsumeDouble(const uint8_t *Data, size_t Size, size_t *Offset) {
  if (*Offset + sizeof(uint64_t) > Size) return 1.0;
  uint64_t v = 0;
  memcpy(&v, Data + *Offset, sizeof(v));
  *Offset += sizeof(v);
  v %= 1000000ULL;
  return (double)v / 100000.0;
}

static png_fixed_point ConsumeFixed(const uint8_t *Data, size_t Size, size_t *Offset) {
  if (*Offset + sizeof(uint32_t) > Size) return PNG_GAMMA_LINEAR;
  uint32_t v = 0;
  memcpy(&v, Data + *Offset, sizeof(v));
  *Offset += sizeof(v);

  switch (v % 8) {
    case 0: return PNG_DEFAULT_sRGB;
    case 1: return PNG_GAMMA_MAC_18;
    case 2: return PNG_GAMMA_sRGB;
    case 3: return PNG_GAMMA_LINEAR;
    case 4: return 1;
    case 5: return 100000;
    case 6: return 220000;
    default: return (png_fixed_point)((v % 500000) + 1);
  }
}

static int ConsumeInt(const uint8_t *Data, size_t Size, size_t *Offset) {
  if (*Offset + sizeof(uint32_t) > Size) return 0;
  uint32_t v = 0;
  memcpy(&v, Data + *Offset, sizeof(v));
  *Offset += sizeof(v);
  return (int)v;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (!fp) return 0;
  if (Size > 0) fwrite(Data, 1, Size, fp);
  fclose(fp);

  fp = fopen("./dummy_file", "rb");
  if (!fp) return 0;

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, PngErrorFn, PngWarningFn);
  if (!png_ptr) {
    fclose(fp);
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    png_destroy_read_struct(&png_ptr, NULL, NULL);
    fclose(fp);
    return 0;
  }

  png_infop end_info = png_create_info_struct(png_ptr);
  if (!end_info) {
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    fclose(fp);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    fclose(fp);
    return 0;
  }

  size_t off = 0;

  png_color_16 background;
  memset(&background, 0, sizeof(background));
  if (Size > off) background.index = Data[off++];
  if (Size >= off + 2) {
    background.red = (png_uint_16)((Data[off] << 8) | Data[off + 1]);
    off += 2;
  }
  if (Size >= off + 2) {
    background.green = (png_uint_16)((Data[off] << 8) | Data[off + 1]);
    off += 2;
  }
  if (Size >= off + 2) {
    background.blue = (png_uint_16)((Data[off] << 8) | Data[off + 1]);
    off += 2;
  }
  if (Size >= off + 2) {
    background.gray = (png_uint_16)((Data[off] << 8) | Data[off + 1]);
    off += 2;
  }

  png_fixed_point screen_gamma_fixed = ConsumeFixed(Data, Size, &off);
  png_fixed_point file_gamma_fixed = ConsumeFixed(Data, Size, &off);
  double screen_gamma = ConsumeDouble(Data, Size, &off);
  double file_gamma = ConsumeDouble(Data, Size, &off);
  double alpha_gamma = ConsumeDouble(Data, Size, &off);

  int alpha_modes[] = {
      PNG_ALPHA_PNG,
      PNG_ALPHA_STANDARD,
      PNG_ALPHA_OPTIMIZED,
      PNG_ALPHA_BROKEN
  };
  int bg_gamma_codes[] = {
      PNG_BACKGROUND_GAMMA_UNKNOWN,
      PNG_BACKGROUND_GAMMA_SCREEN,
      PNG_BACKGROUND_GAMMA_FILE,
      PNG_BACKGROUND_GAMMA_UNIQUE
  };

  int alpha_mode = alpha_modes[(unsigned)ConsumeInt(Data, Size, &off) % 4];
  int bg_gamma_code = bg_gamma_codes[(unsigned)ConsumeInt(Data, Size, &off) % 4];
  int need_expand = ConsumeInt(Data, Size, &off) & 1;

  png_init_io(png_ptr, fp);

  if ((ConsumeInt(Data, Size, &off) & 1) != 0) {
    png_set_gamma_fixed(png_ptr, screen_gamma_fixed, file_gamma_fixed);
  } else {
    png_set_gamma(png_ptr, screen_gamma, file_gamma);
  }

  if ((ConsumeInt(Data, Size, &off) & 1) != 0) {
    png_set_alpha_mode(png_ptr, alpha_mode, alpha_gamma);
  }

  if ((ConsumeInt(Data, Size, &off) & 1) != 0) {
    png_set_background(png_ptr, &background, bg_gamma_code, need_expand,
                       ConsumeDouble(Data, Size, &off));
  }

  if ((ConsumeInt(Data, Size, &off) & 1) != 0) {
    png_set_expand_16(png_ptr);
  }

#ifdef PNG_READ_SCALE_16_TO_8_SUPPORTED
  if ((ConsumeInt(Data, Size, &off) & 1) != 0) {
    png_set_scale_16(png_ptr);
  }
#endif

  png_read_info(png_ptr, info_ptr);

  if ((ConsumeInt(Data, Size, &off) & 1) != 0) {
    png_set_gamma_fixed(png_ptr, screen_gamma_fixed, file_gamma_fixed);
  }
  if ((ConsumeInt(Data, Size, &off) & 1) != 0) {
    png_set_background(png_ptr, &background, bg_gamma_code, need_expand,
                       ConsumeDouble(Data, Size, &off));
  }
  if ((ConsumeInt(Data, Size, &off) & 1) != 0) {
    png_set_expand_16(png_ptr);
  }
  if ((ConsumeInt(Data, Size, &off) & 1) != 0) {
    png_set_alpha_mode(png_ptr, alpha_mode, alpha_gamma);
  }

  png_read_update_info(png_ptr, info_ptr);

  png_uint_32 width = png_get_image_width(png_ptr, info_ptr);
  png_uint_32 height = png_get_image_height(png_ptr, info_ptr);
  png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);

  if (height == 0 || rowbytes == 0 || height > 100000 || rowbytes > 1000000) {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    fclose(fp);
    return 0;
  }

  png_bytep image_data = (png_bytep)malloc(rowbytes * height);
  png_bytepp rows = (png_bytepp)malloc(sizeof(png_bytep) * height);
  if (!image_data || !rows) {
    free(image_data);
    free(rows);
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    fclose(fp);
    return 0;
  }

  for (png_uint_32 y = 0; y < height; ++y) {
    rows[y] = image_data + y * rowbytes;
  }

  png_read_image(png_ptr, rows);
  png_read_end(png_ptr, end_info);

  free(rows);
  free(image_data);
  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
  fclose(fp);
  return 0;
}