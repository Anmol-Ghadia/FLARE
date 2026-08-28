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
#include <string.h>
#include <vector>

static void WriteDummyFile(const uint8_t* Data, size_t Size) {
  FILE* fp = fopen("./dummy_file", "wb");
  if (fp == NULL) return;
  if (Size > 0) fwrite(Data, 1, Size, fp);
  fclose(fp);
}

static void CallTargetGetters(png_structp png_ptr, png_infop info_ptr) {
  if (png_ptr == NULL || info_ptr == NULL) return;

  (void)png_get_x_offset_microns(png_ptr, info_ptr);
  (void)png_get_y_offset_microns(png_ptr, info_ptr);
  (void)png_get_x_offset_inches(png_ptr, info_ptr);
  (void)png_get_y_offset_inches(png_ptr, info_ptr);

#ifdef PNG_FIXED_POINT_SUPPORTED
  (void)png_get_x_offset_inches_fixed(png_ptr, info_ptr);
  (void)png_get_y_offset_inches_fixed(png_ptr, info_ptr);
#endif

#ifdef PNG_pHYs_SUPPORTED
  png_uint_32 rx = 0, ry = 0;
  int unit_type = 0;
  (void)png_get_pHYs_dpi(png_ptr, info_ptr, &rx, &ry, &unit_type);
#endif
}

static void FuzzManualInfoStates(const uint8_t* Data, size_t Size) {
  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (png_ptr == NULL) return;

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == NULL) {
    png_destroy_read_struct(&png_ptr, NULL, NULL);
    return;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    return;
  }

  CallTargetGetters(png_ptr, info_ptr);

#ifdef PNG_oFFs_SUPPORTED
  png_int_32 x = 0;
  png_int_32 y = 0;
  int unit = PNG_OFFSET_PIXEL;

  if (Size >= 4) {
    x = static_cast<png_int_32>(
        (static_cast<uint32_t>(Data[0]) << 24) |
        (static_cast<uint32_t>(Data[1]) << 16) |
        (static_cast<uint32_t>(Data[2]) << 8) |
        static_cast<uint32_t>(Data[3]));
  }
  if (Size >= 8) {
    y = static_cast<png_int_32>(
        (static_cast<uint32_t>(Data[4]) << 24) |
        (static_cast<uint32_t>(Data[5]) << 16) |
        (static_cast<uint32_t>(Data[6]) << 8) |
        static_cast<uint32_t>(Data[7]));
  }
  if (Size >= 9) {
    unit = (Data[8] & 1) ? PNG_OFFSET_MICROMETER : PNG_OFFSET_PIXEL;
  }

  png_set_oFFs(png_ptr, info_ptr, x, y, unit);
  CallTargetGetters(png_ptr, info_ptr);

  png_set_oFFs(png_ptr, info_ptr, x, y, PNG_OFFSET_MICROMETER);
  CallTargetGetters(png_ptr, info_ptr);

  png_set_oFFs(png_ptr, info_ptr, x, y, PNG_OFFSET_PIXEL);
  CallTargetGetters(png_ptr, info_ptr);

  png_set_oFFs(png_ptr, info_ptr, 0, 0, PNG_OFFSET_MICROMETER);
  CallTargetGetters(png_ptr, info_ptr);

  png_set_oFFs(png_ptr, info_ptr, INT32_MAX, INT32_MIN, PNG_OFFSET_MICROMETER);
  CallTargetGetters(png_ptr, info_ptr);
#endif

  png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
}

static void FuzzReadPath(const uint8_t* Data, size_t Size) {
  WriteDummyFile(Data, Size);

  FILE* fp = fopen("./dummy_file", "rb");
  if (fp == NULL) return;

  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (png_ptr == NULL) {
    fclose(fp);
    return;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == NULL) {
    png_destroy_read_struct(&png_ptr, NULL, NULL);
    fclose(fp);
    return;
  }

  png_infop end_info = png_create_info_struct(png_ptr);
  if (end_info == NULL) {
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    fclose(fp);
    return;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    fclose(fp);
    return;
  }

  png_init_io(png_ptr, fp);
  png_read_info(png_ptr, info_ptr);
  CallTargetGetters(png_ptr, info_ptr);

  png_uint_32 width = 0, height = 0;
  int bit_depth = 0, color_type = 0, interlace_type = 0, compression_type = 0, filter_method = 0;
  png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
               &interlace_type, &compression_type, &filter_method);

  png_read_update_info(png_ptr, info_ptr);
  CallTargetGetters(png_ptr, info_ptr);

  png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);
  if (height > 0 && rowbytes > 0 && rowbytes < (1u << 26) && height < (1u << 20)) {
    std::vector<png_bytep> rows(height, NULL);
    std::vector<uint8_t> image(static_cast<size_t>(rowbytes) * height);
    for (png_uint_32 i = 0; i < height; ++i) {
      rows[i] = image.data() + static_cast<size_t>(i) * rowbytes;
    }
    png_read_image(png_ptr, rows.data());
    CallTargetGetters(png_ptr, info_ptr);
  } else {
    png_read_end(png_ptr, end_info);
    CallTargetGetters(png_ptr, info_ptr);
    CallTargetGetters(png_ptr, end_info);
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    fclose(fp);
    return;
  }

  png_read_end(png_ptr, end_info);
  CallTargetGetters(png_ptr, info_ptr);
  CallTargetGetters(png_ptr, end_info);

  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
  fclose(fp);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FuzzManualInfoStates(Data, Size);
  FuzzReadPath(Data, Size);
  return 0;
}