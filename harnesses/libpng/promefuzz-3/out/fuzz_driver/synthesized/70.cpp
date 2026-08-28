// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_read_info at pngread.c:92:1 in png.h
// png_set_tRNS_to_alpha at pngrtran.c:926:1 in png.h
// png_set_palette_to_rgb at pngrtran.c:902:1 in png.h
// png_set_packswap at pngtrans.c:70:1 in png.h
// png_set_swap at pngtrans.c:35:1 in png.h
// png_set_expand_gray_1_2_4_to_8 at pngrtran.c:914:1 in png.h
// png_set_gray_to_rgb at pngrtran.c:955:1 in png.h
// png_get_IHDR at pngget.c:825:1 in png.h
// png_read_update_info at pngread.c:267:1 in png.h
// png_get_rowbytes at pngget.c:30:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_interlace_handling at pngtrans.c:99:1 in png.h
// png_read_row at pngread.c:383:1 in png.h
// png_read_row at pngread.c:383:1 in png.h
// png_read_row at pngread.c:383:1 in png.h
// png_read_end at pngread.c:766:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_read_fn at pngrio.c:87:1 in png.h
// png_set_sig_bytes at png.c:47:1 in png.h
// png_set_tRNS_to_alpha at pngrtran.c:926:1 in png.h
// png_set_palette_to_rgb at pngrtran.c:902:1 in png.h
// png_set_packswap at pngtrans.c:70:1 in png.h
// png_set_swap at pngtrans.c:35:1 in png.h
// png_set_expand_gray_1_2_4_to_8 at pngrtran.c:914:1 in png.h
// png_set_gray_to_rgb at pngrtran.c:955:1 in png.h
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
#include <vector>
#include <algorithm>

static void PngReadCallback(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead) {
  png_voidp io_ptr = png_get_io_ptr(png_ptr);
  if (io_ptr == nullptr) {
    png_error(png_ptr, "no io_ptr");
    return;
  }

  struct BufState {
    const uint8_t* data;
    size_t size;
    size_t offset;
  };

  BufState* st = reinterpret_cast<BufState*>(io_ptr);
  if (st->offset > st->size || byteCountToRead > st->size - st->offset) {
    png_error(png_ptr, "read overflow");
    return;
  }

  for (png_size_t i = 0; i < byteCountToRead; ++i)
    outBytes[i] = st->data[st->offset + i];
  st->offset += byteCountToRead;
}

static void PngErrorCallback(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningCallback(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput_70(const uint8_t *Data, size_t Size) {
  if (Data == nullptr || Size == 0)
    return 0;

  FILE* fp = fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorCallback, PngWarningCallback);
  if (png_ptr == nullptr)
    return 0;

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr) {
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    return 0;
  }

  png_infop end_info = png_create_info_struct(png_ptr);
  if (end_info == nullptr) {
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    return 0;
  }

  struct BufState {
    const uint8_t* data;
    size_t size;
    size_t offset;
  } st = {Data, Size, 0};

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    return 0;
  }

  png_set_read_fn(png_ptr, &st, PngReadCallback);

  if (Size >= 8)
    png_set_sig_bytes(png_ptr, 0);

  uint8_t control = Data[0];

  if (control & 0x01) png_set_tRNS_to_alpha(png_ptr);
  if (control & 0x02) png_set_palette_to_rgb(png_ptr);
  if (control & 0x04) png_set_packswap(png_ptr);
  if (control & 0x08) png_set_swap(png_ptr);
  if (control & 0x10) png_set_expand_gray_1_2_4_to_8(png_ptr);
  if (control & 0x20) png_set_gray_to_rgb(png_ptr);

  png_read_info(png_ptr, info_ptr);

  if (control & 0x40) png_set_tRNS_to_alpha(png_ptr);
  if (control & 0x80) png_set_palette_to_rgb(png_ptr);
  png_set_packswap(png_ptr);
  png_set_swap(png_ptr);
  png_set_expand_gray_1_2_4_to_8(png_ptr);
  png_set_gray_to_rgb(png_ptr);

  png_uint_32 width = 0, height = 0;
  int bit_depth = 0, color_type = 0, interlace_type = 0, compression_type = 0, filter_method = 0;
  png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
               &interlace_type, &compression_type, &filter_method);

  png_read_update_info(png_ptr, info_ptr);

  png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);
  if (rowbytes == 0) {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    return 0;
  }

  const png_uint_32 max_rows = 1024;
  png_uint_32 rows_to_read = std::min(height, max_rows);

  std::vector<png_byte> image;
  if (rows_to_read > 0 && rowbytes <= (1u << 20)) {
    image.resize(static_cast<size_t>(rowbytes) * rows_to_read);
    std::vector<png_bytep> row_ptrs(rows_to_read);
    for (png_uint_32 y = 0; y < rows_to_read; ++y)
      row_ptrs[y] = image.data() + static_cast<size_t>(y) * rowbytes;

    int passes = png_set_interlace_handling(png_ptr);
    for (int pass = 0; pass < passes; ++pass) {
      for (png_uint_32 y = 0; y < rows_to_read; ++y)
        png_read_row(png_ptr, row_ptrs[y], nullptr);
    }

    for (png_uint_32 y = rows_to_read; y < height; ++y) {
      std::vector<png_byte> tmp(rowbytes);
      int passes2 = 1;
      if (interlace_type != PNG_INTERLACE_NONE)
        passes2 = 0;
      (void)passes2;
      png_read_row(png_ptr, tmp.data(), nullptr);
    }
  } else {
    for (png_uint_32 y = 0; y < height; ++y)
      png_read_row(png_ptr, nullptr, nullptr);
  }

  png_read_end(png_ptr, end_info);
  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
  return 0;
}