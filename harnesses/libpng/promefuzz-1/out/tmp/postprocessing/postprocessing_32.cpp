#include <png.h>
#include <zlib.h>
#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>

static void WriteBE32(uint8_t* p, uint32_t v) {
  p[0] = static_cast<uint8_t>((v >> 24) & 0xff);
  p[1] = static_cast<uint8_t>((v >> 16) & 0xff);
  p[2] = static_cast<uint8_t>((v >> 8) & 0xff);
  p[3] = static_cast<uint8_t>(v & 0xff);
}

static uint32_t ReadLE32(const uint8_t* p, size_t size, size_t off, uint32_t fallback) {
  if (off + 4 > size) return fallback;
  return static_cast<uint32_t>(p[off]) |
         (static_cast<uint32_t>(p[off + 1]) << 8) |
         (static_cast<uint32_t>(p[off + 2]) << 16) |
         (static_cast<uint32_t>(p[off + 3]) << 24);
}

static void WriteChunk(FILE* fp, const char type[4], const uint8_t* data, uint32_t len) {
  uint8_t len_be[4];
  WriteBE32(len_be, len);
  fwrite(len_be, 1, 4, fp);
  fwrite(type, 1, 4, fp);
  if (len > 0 && data != NULL) fwrite(data, 1, len, fp);

  uLong crc = crc32(0L, Z_NULL, 0);
  crc = crc32(crc, reinterpret_cast<const Bytef*>(type), 4);
  if (len > 0 && data != NULL) {
    crc = crc32(crc, reinterpret_cast<const Bytef*>(data), len);
  }

  uint8_t crc_be[4];
  WriteBE32(crc_be, static_cast<uint32_t>(crc));
  fwrite(crc_be, 1, 4, fp);
}

static void BuildPNGFile(const uint8_t* Data, size_t Size) {
  FILE* fp = fopen("./dummy_file", "wb");
  if (!fp) return;

  static const uint8_t sig[8] = {137, 80, 78, 71, 13, 10, 26, 10};
  fwrite(sig, 1, 8, fp);

  uint8_t ihdr[13];
  uint32_t width = (ReadLE32(Data, Size, 0, 1) % 64) + 1;
  uint32_t height = (ReadLE32(Data, Size, 4, 1) % 64) + 1;
  WriteBE32(ihdr + 0, width);
  WriteBE32(ihdr + 4, height);
  ihdr[8] = 8;
  ihdr[9] = 2;
  ihdr[10] = 0;
  ihdr[11] = 0;
  ihdr[12] = 0;
  WriteChunk(fp, "IHDR", ihdr, 13);

  if (Size > 0) {
    uint8_t offs[9];
    uint32_t xoff = ReadLE32(Data, Size, 8, 0);
    uint32_t yoff = ReadLE32(Data, Size, 12, 0);
    WriteBE32(offs + 0, xoff);
    WriteBE32(offs + 4, yoff);
    offs[8] = static_cast<uint8_t>((Size > 16 ? Data[16] : Data[0]) & 1);
    WriteChunk(fp, "oFFs", offs, 9);
  }

  if (Size > 1) {
    uint8_t phys[9];
    uint32_t res_x = (ReadLE32(Data, Size, 17, 1) % 1000000U);
    uint32_t res_y = (ReadLE32(Data, Size, 21, 1) % 1000000U);
    WriteBE32(phys + 0, res_x);
    WriteBE32(phys + 4, res_y);
    phys[8] = static_cast<uint8_t>((Size > 25 ? Data[25] : Data[1]) & 1);
    WriteChunk(fp, "pHYs", phys, 9);
  }

  size_t rowbytes = width * 3;
  size_t raw_size = height * (1 + rowbytes);
  uint8_t* raw = static_cast<uint8_t*>(malloc(raw_size));
  if (raw != NULL) {
    size_t idx = 0;
    for (uint32_t y = 0; y < height; ++y) {
      raw[idx++] = 0;
      for (size_t x = 0; x < rowbytes; ++x) {
        raw[idx++] = (Size == 0) ? 0 : Data[(x + y) % Size];
      }
    }

    uLongf comp_bound = compressBound(static_cast<uLong>(raw_size));
    uint8_t* comp = static_cast<uint8_t*>(malloc(comp_bound));
    if (comp != NULL) {
      if (compress2(comp, &comp_bound, raw, static_cast<uLong>(raw_size), Z_BEST_SPEED) == Z_OK) {
        WriteChunk(fp, "IDAT", comp, static_cast<uint32_t>(comp_bound));
      }
      free(comp);
    }
    free(raw);
  }

  WriteChunk(fp, "IEND", NULL, 0);
  fclose(fp);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  BuildPNGFile(Data, Size);

  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (png_ptr == NULL) return 0;

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == NULL) {
    png_destroy_read_struct(&png_ptr, NULL, NULL);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    return 0;
  }

  FILE* fp = fopen("./dummy_file", "rb");
  if (fp == NULL) {
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    return 0;
  }

  png_init_io(png_ptr, fp);
  png_read_info(png_ptr, info_ptr);

  (void)png_get_x_offset_pixels(png_ptr, info_ptr);
  (void)png_get_y_offset_pixels(png_ptr, info_ptr);
  (void)png_get_x_offset_microns(png_ptr, info_ptr);
  (void)png_get_y_offset_microns(png_ptr, info_ptr);
#ifdef PNG_FIXED_POINT_SUPPORTED
  (void)png_get_x_offset_inches_fixed(png_ptr, info_ptr);
  (void)png_get_pixel_aspect_ratio_fixed(png_ptr, info_ptr);
#endif

#ifdef PNG_pHYs_SUPPORTED
  png_uint_32 res_x = 0, res_y = 0;
  int unit_type = 0;
  (void)png_get_pHYs_dpi(png_ptr, info_ptr, &res_x, &res_y, &unit_type);
#endif

  png_read_update_info(png_ptr, info_ptr);

  (void)png_get_x_offset_pixels(png_ptr, info_ptr);
  (void)png_get_y_offset_pixels(png_ptr, info_ptr);
  (void)png_get_x_offset_microns(png_ptr, info_ptr);
  (void)png_get_y_offset_microns(png_ptr, info_ptr);
#ifdef PNG_FIXED_POINT_SUPPORTED
  (void)png_get_x_offset_inches_fixed(png_ptr, info_ptr);
  (void)png_get_pixel_aspect_ratio_fixed(png_ptr, info_ptr);
#endif
#ifdef PNG_pHYs_SUPPORTED
  (void)png_get_pHYs_dpi(png_ptr, info_ptr, &res_x, &res_y, &unit_type);
#endif

  fclose(fp);
  png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
  return 0;
}