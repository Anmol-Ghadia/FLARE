// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_read_image at pngread.c:700:1 in png.h
// png_read_rows at pngread.c:645:1 in png.h
// png_read_row at pngread.c:383:1 in png.h
// png_read_end at pngread.c:766:1 in png.h
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
// png_set_gray_to_rgb at pngrtran.c:955:1 in png.h
// png_set_tRNS_to_alpha at pngrtran.c:926:1 in png.h
// png_set_packing at pngtrans.c:50:1 in png.h
// png_set_gray_to_rgb at pngrtran.c:955:1 in png.h
// png_set_tRNS_to_alpha at pngrtran.c:926:1 in png.h
// png_set_packing at pngtrans.c:50:1 in png.h
// png_read_info at pngread.c:92:1 in png.h
// png_set_gray_to_rgb at pngrtran.c:955:1 in png.h
// png_set_tRNS_to_alpha at pngrtran.c:926:1 in png.h
// png_set_packing at pngtrans.c:50:1 in png.h
// png_set_interlace_handling at pngtrans.c:99:1 in png.h
// png_read_update_info at pngread.c:267:1 in png.h
// png_get_image_width at pngget.c:52:1 in png.h
// png_get_image_height at pngget.c:61:1 in png.h
// png_get_rowbytes at pngget.c:30:1 in png.h
// png_read_end at pngread.c:766:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
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
#include <vector>

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (!fp) return 0;
  if (Size > 0) fwrite(Data, 1, Size, fp);
  fclose(fp);

  fp = fopen("./dummy_file", "rb");
  if (!fp) return 0;

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn, PngWarningFn);
  if (!png_ptr) {
    fclose(fp);
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    fclose(fp);
    return 0;
  }

  png_infop end_info = png_create_info_struct(png_ptr);
  if (!end_info) {
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    fclose(fp);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    fclose(fp);
    return 0;
  }

  png_init_io(png_ptr, fp);

  if (Size > 0) {
    if (Data[0] & 0x01) png_set_gray_to_rgb(png_ptr);
    if (Data[0] & 0x02) png_set_tRNS_to_alpha(png_ptr);
    if (Data[0] & 0x04) png_set_packing(png_ptr);
  } else {
    png_set_gray_to_rgb(png_ptr);
    png_set_tRNS_to_alpha(png_ptr);
    png_set_packing(png_ptr);
  }

  png_read_info(png_ptr, info_ptr);

  if (Size > 1) {
    if (Data[1] & 0x01) png_set_gray_to_rgb(png_ptr);
    if (Data[1] & 0x02) png_set_tRNS_to_alpha(png_ptr);
    if (Data[1] & 0x04) png_set_packing(png_ptr);
  }

  if (Size > 2 && (Data[2] & 0x01)) {
    png_set_interlace_handling(png_ptr);
  }

  png_read_update_info(png_ptr, info_ptr);

  png_uint_32 width = png_get_image_width(png_ptr, info_ptr);
  png_uint_32 height = png_get_image_height(png_ptr, info_ptr);
  png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);

  if (rowbytes == 0 || height == 0 || width == 0) {
    png_read_end(png_ptr, end_info);
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    fclose(fp);
    return 0;
  }

  const png_uint_32 kMaxDim = 4096;
  const png_size_t kMaxRowbytes = 1 << 20;
  const png_uint_32 kMaxRows = 4096;

  if (height > kMaxRows || width > kMaxDim || rowbytes > kMaxRowbytes) {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    fclose(fp);
    return 0;
  }

  if (height != 0 && rowbytes > ((size_t)-1) / height) {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    fclose(fp);
    return 0;
  }

  std::vector<png_byte> image_storage((size_t)rowbytes * height);
  std::vector<png_bytep> rows(height);
  std::vector<png_byte> display_storage((size_t)rowbytes * height);
  std::vector<png_bytep> display_rows(height);

  for (png_uint_32 i = 0; i < height; ++i) {
    rows[i] = image_storage.data() + (size_t)i * rowbytes;
    display_rows[i] = display_storage.data() + (size_t)i * rowbytes;
  }

  uint8_t mode = Size > 3 ? Data[3] : 0;

  if ((mode % 3) == 0) {
    png_read_image(png_ptr, rows.data());
  } else if ((mode % 3) == 1) {
    png_uint_32 remaining = height;
    png_uint_32 offset = 0;
    size_t idx = 4;

    while (remaining > 0) {
      png_uint_32 chunk = 1;
      if (idx < Size) {
        chunk = (png_uint_32)(Data[idx] % 8) + 1;
        ++idx;
      }
      if (chunk > remaining) chunk = remaining;

      png_bytepp row_ptr = rows.data() + offset;
      png_bytepp display_ptr = nullptr;
      if (idx < Size && (Data[idx] & 1)) display_ptr = display_rows.data() + offset;
      if (idx < Size) ++idx;

      png_read_rows(png_ptr, row_ptr, display_ptr, chunk);
      offset += chunk;
      remaining -= chunk;
    }
  } else {
    for (png_uint_32 i = 0; i < height; ++i) {
      png_bytep display = nullptr;
      if (Size > 4 + i && (Data[4 + i] & 1)) display = display_rows[i];
      png_read_row(png_ptr, rows[i], display);
    }
  }

  png_read_end(png_ptr, end_info);
  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
  fclose(fp);
  return 0;
}