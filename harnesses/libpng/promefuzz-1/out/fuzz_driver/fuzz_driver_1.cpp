// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_add_alpha at pngtrans.c:205:1 in png.h
// png_set_swap_alpha at pngtrans.c:223:1 in png.h
// png_set_bgr at pngtrans.c:21:1 in png.h
// png_read_update_info at pngread.c:267:1 in png.h
// png_get_rowbytes at pngget.c:30:1 in png.h
// png_read_end at pngread.c:766:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_malloc at pngmem.c:171:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_get_image_height at pngget.c:61:1 in png.h
// png_read_row at pngread.c:383:1 in png.h
// png_read_end at pngread.c:766:1 in png.h
// png_free at pngmem.c:232:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_read_fn at pngrio.c:87:1 in png.h
// png_sig_cmp at png.c:74:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_sig_bytes at png.c:47:1 in png.h
// png_read_info at pngread.c:92:1 in png.h
// png_set_expand at pngrtran.c:872:1 in png.h
// png_set_gray_to_rgb at pngrtran.c:955:1 in png.h
// png_set_strip_16 at pngrtran.c:197:1 in png.h
// png_set_packing at pngtrans.c:50:1 in png.h
// png_set_tRNS_to_alpha at pngrtran.c:926:1 in png.h
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
#include <setjmp.h>

static void FuzzReadData(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead) {
  png_voidp io_ptr = png_get_io_ptr(png_ptr);
  if (io_ptr == NULL || outBytes == NULL) {
    return;
  }

  struct ReaderState {
    const uint8_t* data;
    size_t size;
    size_t offset;
  };

  ReaderState* st = reinterpret_cast<ReaderState*>(io_ptr);
  if (st->offset > st->size) {
    png_error(png_ptr, "invalid offset");
    return;
  }

  size_t remain = st->size - st->offset;
  size_t to_copy = byteCountToRead;
  if (to_copy > remain) {
    to_copy = remain;
  }

  if (to_copy > 0) {
    memcpy(outBytes, st->data + st->offset, to_copy);
    st->offset += to_copy;
  }

  if (to_copy < byteCountToRead) {
    memset(outBytes + to_copy, 0, byteCountToRead - to_copy);
    png_error(png_ptr, "read beyond end");
  }
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE* fp = fopen("./dummy_file", "wb");
  if (fp != NULL) {
    if (Size > 0) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  struct ReaderState {
    const uint8_t* data;
    size_t size;
    size_t offset;
  } state = { Data, Size, 0 };

  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (png_ptr == NULL) {
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  png_infop end_info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == NULL || end_info_ptr == NULL) {
    png_destroy_read_struct(&png_ptr, info_ptr ? &info_ptr : NULL, end_info_ptr ? &end_info_ptr : NULL);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info_ptr);
    return 0;
  }

  png_set_read_fn(png_ptr, &state, FuzzReadData);

  if (Size < 8 || png_sig_cmp(const_cast<png_bytep>(reinterpret_cast<const png_byte*>(Data)), 0, 8) != 0) {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info_ptr);
    return 0;
  }

  png_set_sig_bytes(png_ptr, 0);
  png_read_info(png_ptr, info_ptr);

  if (Size > 8) {
    uint8_t control = Data[8];
    if (control & 0x01) png_set_expand(png_ptr);
    if (control & 0x02) png_set_gray_to_rgb(png_ptr);
    if (control & 0x04) png_set_strip_16(png_ptr);
    if (control & 0x08) png_set_packing(png_ptr);
    if (control & 0x10) png_set_tRNS_to_alpha(png_ptr);
    if (control & 0x20) png_set_add_alpha(png_ptr, 0xff, PNG_FILLER_AFTER);
    if (control & 0x40) png_set_swap_alpha(png_ptr);
    if (control & 0x80) png_set_bgr(png_ptr);
  }

  png_read_update_info(png_ptr, info_ptr);

  size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);
  if (rowbytes == 0) {
    png_read_end(png_ptr, end_info_ptr);
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info_ptr);
    return 0;
  }

  png_voidp row_mem = png_malloc(png_ptr, rowbytes);
  if (row_mem == NULL) {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info_ptr);
    return 0;
  }

  png_bytep row = reinterpret_cast<png_bytep>(row_mem);
  png_uint_32 height = png_get_image_height(png_ptr, info_ptr);

  for (png_uint_32 y = 0; y < height; ++y) {
    png_read_row(png_ptr, row, NULL);
  }

  png_read_end(png_ptr, end_info_ptr);
  png_free(png_ptr, row_mem);
  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info_ptr);
  return 0;
}