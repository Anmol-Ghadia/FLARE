// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_get_io_ptr at png.c:686:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_read_fn at pngrio.c:87:1 in png.h
// png_read_info at pngread.c:92:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_get_IHDR at pngget.c:825:1 in png.h
// png_set_gray_to_rgb at pngrtran.c:955:1 in png.h
// png_get_IHDR at pngget.c:825:1 in png.h
// png_get_IHDR at pngget.c:825:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
#include <png.h>
#include <stdint.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdio.h>
#include <string.h>

struct BufferState {
  const uint8_t* data;
  size_t size;
  size_t offset;
};

static void PngReadCallback(png_structp png_ptr, png_bytep outBytes,
                            png_size_t byteCountToRead) {
  BufferState* st = (BufferState*)png_get_io_ptr(png_ptr);
  if (st == NULL || outBytes == NULL || st->offset > st->size ||
      byteCountToRead > st->size - st->offset) {
    png_error(png_ptr, "read overflow");
    return;
  }

  memcpy(outBytes, st->data + st->offset, byteCountToRead);
  st->offset += byteCountToRead;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE* fp = fopen("./dummy_file", "wb");
  if (fp != NULL) {
    if (Data != NULL && Size > 0) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  png_structp null_png = NULL;
  png_infop null_info = NULL;
  png_destroy_read_struct(&null_png, &null_info, NULL);

  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (png_ptr == NULL) {
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == NULL) {
    png_destroy_read_struct(&png_ptr, NULL, NULL);
    return 0;
  }

  jmp_buf* jb = png_set_longjmp_fn(png_ptr, longjmp, sizeof(jmp_buf));
  if (jb == NULL) {
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    return 0;
  }

  if (setjmp(*jb) == 0) {
    BufferState state = {Data, Size, 0};
    png_set_read_fn(png_ptr, &state, PngReadCallback);

    png_read_info(png_ptr, info_ptr);

    (void)png_set_longjmp_fn(png_ptr, longjmp, sizeof(jmp_buf));

    png_uint_32 width = 0, height = 0;
    int bit_depth = 0, color_type = 0, interlace_method = 0;
    int compression_method = 0, filter_method = 0;
    (void)png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth,
                       &color_type, &interlace_method, &compression_method,
                       &filter_method);

    png_set_gray_to_rgb(png_ptr);

    if ((Size & 1) == 0) {
      png_uint_32 w2 = 0, h2 = 0;
      (void)png_get_IHDR(png_ptr, info_ptr, &w2, &h2, NULL, NULL, NULL, NULL, NULL);
    } else {
      (void)png_get_IHDR(png_ptr, info_ptr, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    }
  }

  png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
  png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
  png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

  return 0;
}