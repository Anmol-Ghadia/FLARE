// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_get_io_ptr at png.c:686:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_init_io at png.c:703:1 in png.h
// png_set_read_fn at pngrio.c:87:1 in png.h
// png_read_info at pngread.c:92:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_get_IHDR at pngget.c:825:1 in png.h
// png_set_gray_to_rgb at pngrtran.c:955:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <png.h>
#include <setjmp.h>

static void WriteDummyFile(const uint8_t *Data, size_t Size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (fp != NULL) {
    if (Size > 0) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }
}

static void PngReadCallback(png_structp png_ptr, png_bytep outBytes,
                            png_size_t byteCountToRead) {
  FILE *fp = reinterpret_cast<FILE*>(png_get_io_ptr(png_ptr));
  if (fp == NULL || outBytes == NULL) {
    png_error(png_ptr, "invalid io ptr");
    return;
  }

  size_t n = fread(outBytes, 1, byteCountToRead, fp);
  if (n != byteCountToRead) {
    png_error(png_ptr, "short read");
  }
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  WriteDummyFile(Data, Size);

  png_structp png_ptr = NULL;
  png_infop info_ptr = NULL;
  png_infop end_info_ptr = NULL;

  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info_ptr);

  png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (png_ptr == NULL) {
    return 0;
  }

  info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == NULL) {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info_ptr);
    return 0;
  }

  FILE *fp = fopen("./dummy_file", "rb");
  if (fp == NULL) {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info_ptr);
    return 0;
  }

  png_set_longjmp_fn(png_ptr, longjmp, sizeof(jmp_buf));

  if (setjmp(png_jmpbuf(png_ptr))) {
    fclose(fp);
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info_ptr);
    return 0;
  }

  png_init_io(png_ptr, fp);
  png_set_read_fn(png_ptr, fp, PngReadCallback);

  png_read_info(png_ptr, info_ptr);

  png_set_longjmp_fn(png_ptr, longjmp, sizeof(jmp_buf));

  png_uint_32 width = 0, height = 0;
  int bit_depth = 0, color_type = 0;
  int interlace_method = 0, compression_method = 0, filter_method = 0;
  (void)png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth,
                     &color_type, &interlace_method,
                     &compression_method, &filter_method);

  png_set_gray_to_rgb(png_ptr);

  fclose(fp);
  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info_ptr);
  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info_ptr);
  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info_ptr);

  return 0;
}