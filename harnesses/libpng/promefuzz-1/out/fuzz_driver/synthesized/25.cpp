// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_warning at pngerror.c:216:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_chunk_warning at pngerror.c:497:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_benign_error at pngerror.c:362:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_chunk_benign_error at pngerror.c:514:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_process_data_skip at pngpread.c:78:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_warning at pngerror.c:216:1 in png.h
// png_chunk_warning at pngerror.c:497:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_chunk_error at pngerror.c:479:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
#include <png.h>
#include <stdint.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdio.h>

static void DummyWarning(png_structp, png_const_charp) {}

static void DummyError(png_structp png_ptr, png_const_charp)
{
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void BuildMessage(const uint8_t *data, size_t size, char *out, size_t out_size)
{
  if (out_size == 0) return;
  size_t n = size < (out_size - 1) ? size : (out_size - 1);
  for (size_t i = 0; i < n; ++i)
  {
    uint8_t c = data[i];
    out[i] = (c >= 32 && c <= 126) ? (char)c : 'A';
  }
  out[n] = '\0';
}

extern "C" int LLVMFuzzerTestOneInput_25(const uint8_t *Data, size_t Size)
{
  FILE *fp = fopen("./dummy_file", "wb");
  if (fp != NULL)
  {
    if (Size > 0) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  char msg1[256];
  char msg2[256];
  char msg3[256];
  BuildMessage(Data, Size, msg1, sizeof(msg1));
  BuildMessage(Data + (Size > 8 ? 8 : 0), Size > 8 ? Size - 8 : 0, msg2, sizeof(msg2));
  BuildMessage(Data + (Size > 16 ? 16 : 0), Size > 16 ? Size - 16 : 0, msg3, sizeof(msg3));

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, DummyError, DummyWarning);
  if (png_ptr == NULL)
    return 0;

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == NULL)
  {
    png_destroy_read_struct(&png_ptr, NULL, NULL);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr)) == 0)
  {
    png_warning(png_ptr, msg1);
  }

  if (setjmp(png_jmpbuf(png_ptr)) == 0)
  {
    png_chunk_warning(png_ptr, msg2);
  }

  if (setjmp(png_jmpbuf(png_ptr)) == 0)
  {
    png_benign_error(png_ptr, msg3);
  }

  if (setjmp(png_jmpbuf(png_ptr)) == 0)
  {
    png_chunk_benign_error(png_ptr, msg1);
  }

  if (setjmp(png_jmpbuf(png_ptr)) == 0)
  {
    (void)png_process_data_skip(png_ptr);
  }

  if (setjmp(png_jmpbuf(png_ptr)) == 0)
  {
    png_warning(png_ptr, "#1234 stripped-prefix warning");
    png_chunk_warning(png_ptr, "#9999 stripped-prefix chunk warning");
  }

  if (Size > 0 && (Data[0] & 1))
  {
    if (setjmp(png_jmpbuf(png_ptr)) == 0)
    {
      png_chunk_error(png_ptr, msg2);
    }
  }

  png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
  return 0;
}