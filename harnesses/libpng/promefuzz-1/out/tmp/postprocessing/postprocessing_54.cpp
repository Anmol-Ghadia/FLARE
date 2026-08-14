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
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <stdio.h>

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE *fp = fopen("./dummy_file", "wb+");
  if (fp != NULL) {
    if (Data != NULL && Size > 0) {
      fwrite(Data, 1, Size, fp);
    }
    fflush(fp);
    rewind(fp);
  }

  png_structp png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, PngErrorFn, PngWarningFn);
  if (png_ptr == NULL) {
    if (fp != NULL) fclose(fp);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_write_struct(&png_ptr, NULL);
    if (fp != NULL) fclose(fp);
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == NULL) {
    png_destroy_write_struct(&png_ptr, NULL);
    if (fp != NULL) fclose(fp);
    return 0;
  }

  if (fp != NULL) {
    png_init_io(png_ptr, fp);
  }

  png_byte sig[8] = {137, 80, 78, 71, 13, 10, 26, 10};
  png_set_sig_bytes(png_ptr, 0);

  png_set_IHDR(png_ptr, info_ptr,
               1, 1, 8, PNG_COLOR_TYPE_GRAY,
               PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE,
               PNG_FILTER_TYPE_BASE);
  png_write_info(png_ptr, info_ptr);

  size_t s1 = 0, s2 = 0, s3 = 0;
  if (Size > 0) s1 = Data[0];
  if (Size > 1) s2 = ((size_t)Data[1] << 8) | Data[0];
  if (Size > 2) s3 = ((size_t)Data[2] << 16) | ((size_t)Data[1] << 8) | Data[0];

  s1 %= 4096;
  s2 %= 8192;
  s3 %= 16384;

  png_voidp p1 = png_malloc(png_ptr, (png_alloc_size_t)s1);
  if (p1 != NULL) {
    if (Size > 3 && s1 > 0) {
      memcpy(p1, Data + 3, s1 < (Size - 3) ? s1 : (Size - 3));
    }
    png_free(png_ptr, p1);
  }

  png_voidp p2 = png_calloc(png_ptr, (png_alloc_size_t)s2);
  if (p2 != NULL) {
    png_free(png_ptr, p2);
  }

  png_voidp p3 = png_malloc_default(png_ptr, (png_alloc_size_t)s3);
  if (p3 != NULL) {
    png_free_default(png_ptr, p3);
  }

  png_voidp p4 = png_malloc(png_ptr, 0);
  png_free(png_ptr, p4);

  png_voidp p5 = png_calloc(png_ptr, 0);
  png_free(png_ptr, p5);

  png_write_chunk_start(png_ptr, (png_const_bytep)"fuZZ", Size);
  png_write_chunk_data(png_ptr, Data, Size);
  png_write_chunk_data(png_ptr, NULL, 0);
  png_write_chunk_end(png_ptr);

  if (Size > 0) {
    size_t split = Data[0] % (Size + 1);
    png_write_chunk_start(png_ptr, (png_const_bytep)"daTA", Size);
    png_write_chunk_data(png_ptr, Data, split);
    png_write_chunk_data(png_ptr, Data + split, Size - split);
    png_write_chunk_end(png_ptr);
  } else {
    png_write_chunk_start(png_ptr, (png_const_bytep)"emTY", 0);
    png_write_chunk_data(png_ptr, NULL, 0);
    png_write_chunk_end(png_ptr);
  }

  png_byte row[1] = {0};
  if (Size > 0) row[0] = Data[0];
  png_write_row(png_ptr, row);
  png_write_end(png_ptr, info_ptr);

  png_free(png_ptr, NULL);
  png_free_default(png_ptr, NULL);

  png_destroy_write_struct(&png_ptr, &info_ptr);
  if (fp != NULL) fclose(fp);
  return 0;
}