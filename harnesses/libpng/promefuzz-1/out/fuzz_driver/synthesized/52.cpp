// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_chunk_cache_max at pngset.c:1652:1 in png.h
// png_set_chunk_malloc_max at pngset.c:1660:1 in png.h
// png_set_compression_buffer_size at pngset.c:1566:1 in png.h
// png_get_chunk_cache_max at pngget.c:1208:1 in png.h
// png_get_chunk_malloc_max at pngget.c:1215:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_get_chunk_malloc_max at pngget.c:1215:1 in png.h
// png_get_chunk_cache_max at pngget.c:1208:1 in png.h
// png_set_chunk_cache_max at pngset.c:1652:1 in png.h
// png_set_chunk_malloc_max at pngset.c:1660:1 in png.h
// png_create_read_struct_2 at pngread.c:40:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_get_chunk_malloc_max at pngget.c:1215:1 in png.h
// png_get_chunk_cache_max at pngget.c:1208:1 in png.h
// png_set_chunk_cache_max at pngset.c:1652:1 in png.h
// png_set_chunk_malloc_max at pngset.c:1660:1 in png.h
// png_set_compression_buffer_size at pngset.c:1566:1 in png.h
// png_get_chunk_cache_max at pngget.c:1208:1 in png.h
// png_get_chunk_malloc_max at pngget.c:1215:1 in png.h
#include <png.h>
#include <stdint.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>

static void FuzzPngError(png_structp png_ptr, png_const_charp) {
  if (png_ptr) {
    longjmp(png_jmpbuf(png_ptr), 1);
  }
}

static void FuzzPngWarn(png_structp, png_const_charp) {
}

static png_voidp FuzzMalloc(png_structp, png_alloc_size_t size) {
  return malloc(size);
}

static void FuzzFree(png_structp, png_voidp ptr) {
  free(ptr);
}

static uint32_t ReadU32(const uint8_t *data, size_t size, size_t *off) {
  uint32_t v = 0;
  for (int i = 0; i < 4; ++i) {
    v <<= 8;
    if (*off < size) v |= data[(*off)++];
  }
  return v;
}

static size_t ReadSizeT(const uint8_t *data, size_t size, size_t *off) {
  size_t v = 0;
  for (size_t i = 0; i < sizeof(size_t); ++i) {
    v <<= 8;
    if (*off < size) v |= data[(*off)++];
  }
  return v;
}

extern "C" int LLVMFuzzerTestOneInput_52(const uint8_t *Data, size_t Size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  size_t off = 0;

  png_structp null_png = NULL;
  (void)png_get_chunk_malloc_max(null_png);
  (void)png_get_chunk_cache_max(null_png);
  png_set_chunk_cache_max(null_png, 0);
  png_set_chunk_malloc_max(null_png, 0);

  png_structp png_ptr = png_create_read_struct_2(
      PNG_LIBPNG_VER_STRING,
      NULL,
      FuzzPngError,
      FuzzPngWarn,
      NULL,
      FuzzMalloc,
      FuzzFree);

  if (!png_ptr)
    return 0;

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, NULL, NULL);
    return 0;
  }

  (void)png_get_chunk_malloc_max(png_ptr);
  (void)png_get_chunk_cache_max(png_ptr);

  for (int i = 0; i < 8; ++i) {
    uint32_t cache_max = ReadU32(Data, Size, &off);
    size_t malloc_max = ReadSizeT(Data, Size, &off);
    size_t comp_buf_size = ReadSizeT(Data, Size, &off);

    switch ((off < Size) ? (Data[off++] % 6) : (i % 6)) {
      case 0:
        png_set_chunk_cache_max(png_ptr, cache_max);
        break;
      case 1:
        png_set_chunk_malloc_max(png_ptr, (png_alloc_size_t)malloc_max);
        break;
      case 2:
        png_set_compression_buffer_size(png_ptr, comp_buf_size);
        break;
      case 3:
        (void)png_get_chunk_cache_max(png_ptr);
        break;
      case 4:
        (void)png_get_chunk_malloc_max(png_ptr);
        break;
      case 5:
        png_set_chunk_cache_max(png_ptr, cache_max);
        png_set_chunk_malloc_max(png_ptr, (png_alloc_size_t)malloc_max);
        if (comp_buf_size != 0)
          png_set_compression_buffer_size(png_ptr, comp_buf_size);
        break;
    }

    (void)png_get_chunk_cache_max(png_ptr);
    (void)png_get_chunk_malloc_max(png_ptr);
  }

  png_destroy_read_struct(&png_ptr, NULL, NULL);
  return 0;
}