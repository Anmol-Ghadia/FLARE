// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_benign_errors at pngset.c:1671:1 in png.h
// png_warning at pngerror.c:216:1 in png.h
// png_chunk_warning at pngerror.c:497:1 in png.h
// png_benign_error at pngerror.c:362:1 in png.h
// png_chunk_benign_error at pngerror.c:514:1 in png.h
// png_set_benign_errors at pngset.c:1671:1 in png.h
// png_warning at pngerror.c:216:1 in png.h
// png_chunk_warning at pngerror.c:497:1 in png.h
// png_benign_error at pngerror.c:362:1 in png.h
// png_chunk_benign_error at pngerror.c:514:1 in png.h
// png_chunk_warning at pngerror.c:497:1 in png.h
// png_chunk_error at pngerror.c:479:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
#include <png.h>
#include <stdint.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdio.h>
#include <string.h>

static void NoopWarning(png_structp, png_const_charp) {}

static void NoopError(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static png_uint_32 ReadU32(const uint8_t *data, size_t size, size_t off) {
  if (off + 4 > size) return 0;
  return ((png_uint_32)data[off] << 24) |
         ((png_uint_32)data[off + 1] << 16) |
         ((png_uint_32)data[off + 2] << 8) |
         ((png_uint_32)data[off + 3]);
}

extern "C" int LLVMFuzzerTestOneInput_10(const uint8_t *Data, size_t Size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (fp != NULL) {
    if (Size > 0) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NoopError, NoopWarning);
  if (png_ptr == NULL) return 0;

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    return 0;
  }

  int benign_allowed = (Size > 0) ? (Data[0] & 1) : 0;
  png_set_benign_errors(png_ptr, benign_allowed);

  char msg1[256];
  char msg2[256];
  char msg3[256];

  size_t m1len = 0, m2len = 0, m3len = 0;

  if (Size > 1) {
    m1len = Data[1] % 120;
    if (2 + m1len > Size) m1len = (Size > 2) ? (Size - 2) : 0;
    memcpy(msg1, Data + 2, m1len);
  }
  msg1[m1len] = '\0';

  size_t off2 = 2 + m1len;
  if (off2 < Size) {
    m2len = Data[off2] % 120;
    if (off2 + 1 + m2len > Size)
      m2len = (off2 + 1 < Size) ? (Size - off2 - 1) : 0;
    memcpy(msg2, Data + off2 + 1, m2len);
  }
  msg2[m2len] = '\0';

  size_t off3 = off2 + 1 + m2len;
  if (off3 < Size) {
    m3len = Data[off3] % 120;
    if (off3 + 1 + m3len > Size)
      m3len = (off3 + 1 < Size) ? (Size - off3 - 1) : 0;
    memcpy(msg3, Data + off3 + 1, m3len);
  }
  msg3[m3len] = '\0';

  if (m1len == 0) strcpy(msg1, "warning");
  if (m2len == 0) strcpy(msg2, "#123 benign");
  if (m3len == 0) strcpy(msg3, "chunk issue");

  png_warning(png_ptr, msg1);
  png_chunk_warning(png_ptr, msg3);
  png_benign_error(png_ptr, msg2);
  png_chunk_benign_error(png_ptr, msg3);

  png_set_benign_errors(png_ptr, !benign_allowed);
  png_warning(png_ptr, msg2);
  png_chunk_warning(png_ptr, msg1);
  png_benign_error(png_ptr, msg3);
  png_chunk_benign_error(png_ptr, msg2);

  png_chunk_warning(NULL, msg1);

  if (Size >= 4 && (ReadU32(Data, Size, 0) & 1U) != 0) {
    png_chunk_error(png_ptr, msg1);
  }

  png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
  return 0;
}