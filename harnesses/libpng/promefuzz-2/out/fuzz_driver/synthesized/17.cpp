// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_get_valid at pngget.c:20:1 in png.h
// png_set_eXIf_1 at pngset.c:148:1 in png.h
// png_set_eXIf at pngset.c:139:1 in png.h
// png_get_eXIf_1 at pngget.c:789:1 in png.h
// png_get_valid at pngget.c:20:1 in png.h
// png_set_eXIf_1 at pngset.c:148:1 in png.h
// png_get_eXIf_1 at pngget.c:789:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_read_fn at pngrio.c:87:1 in png.h
// png_read_info at pngread.c:92:1 in png.h
// png_get_PLTE at pngget.c:1012:1 in png.h
// png_get_eXIf_1 at pngget.c:789:1 in png.h
// png_get_eXIf at pngget.c:779:1 in png.h
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

static void PngReadCallback(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead) {
  png_bytep io_ptr = reinterpret_cast<png_bytep>(png_get_io_ptr(png_ptr));
  if (io_ptr == NULL) {
    png_error(png_ptr, "No IO pointer");
    return;
  }

  struct ReaderState {
    const uint8_t* data;
    size_t size;
    size_t offset;
  };

  ReaderState* st = reinterpret_cast<ReaderState*>(io_ptr);
  if (st->offset > st->size || byteCountToRead > st->size - st->offset) {
    png_error(png_ptr, "Read overflow");
    return;
  }

  memcpy(outBytes, st->data + st->offset, byteCountToRead);
  st->offset += byteCountToRead;
}

extern "C" int LLVMFuzzerTestOneInput_17(const uint8_t *Data, size_t Size) {
  if (Data == NULL || Size == 0) {
    return 0;
  }

  FILE* fp = fopen("./dummy_file", "wb");
  if (fp != NULL) {
    fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (png_ptr == NULL) {
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == NULL) {
    png_destroy_read_struct(&png_ptr, NULL, NULL);
    return 0;
  }

  png_infop end_info = png_create_info_struct(png_ptr);
  if (end_info == NULL) {
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    return 0;
  }

  struct ReaderState {
    const uint8_t* data;
    size_t size;
    size_t offset;
  } st = { Data, Size, 0 };

  png_set_read_fn(png_ptr, &st, PngReadCallback);

  png_read_info(png_ptr, info_ptr);

  png_colorp palette = NULL;
  int num_palette = -1;
  (void)png_get_PLTE(png_ptr, info_ptr, &palette, &num_palette);

  png_uint_32 num_exif = 0;
  png_bytep exif_ptr = NULL;
  (void)png_get_eXIf_1(png_ptr, info_ptr, &num_exif, &exif_ptr);

  png_bytep exif_stub_ptr = NULL;
  (void)png_get_eXIf(png_ptr, info_ptr, &exif_stub_ptr);

  png_uint_32 flags[] = {
      PNG_INFO_PLTE,
#ifdef PNG_INFO_eXIf
      PNG_INFO_eXIf,
#endif
      0u,
      0xffffffffu
  };

  for (size_t i = 0; i < sizeof(flags) / sizeof(flags[0]); ++i) {
    (void)png_get_valid(png_ptr, info_ptr, flags[i]);
  }

  size_t exif_len = 0;
  if (Size > 8) {
    exif_len = Data[0] % (Size - 8);
  } else {
    exif_len = Size;
  }

  png_bytep mutable_exif = NULL;
  if (exif_len > 0) {
    mutable_exif = reinterpret_cast<png_bytep>(malloc(exif_len));
    if (mutable_exif != NULL) {
      memcpy(mutable_exif, Data + (Size - exif_len), exif_len);
    } else {
      exif_len = 0;
    }
  }

  png_set_eXIf_1(png_ptr, info_ptr, static_cast<png_uint_32>(exif_len), mutable_exif);
  png_set_eXIf(png_ptr, info_ptr, mutable_exif);

  num_exif = 0;
  exif_ptr = NULL;
  (void)png_get_eXIf_1(png_ptr, info_ptr, &num_exif, &exif_ptr);

  (void)png_get_valid(png_ptr, info_ptr,
#ifdef PNG_INFO_eXIf
                      PNG_INFO_eXIf
#else
                      0u
#endif
  );

  if (mutable_exif != NULL) {
    for (size_t i = 0; i < exif_len; ++i) {
      mutable_exif[i] ^= static_cast<png_byte>(i + 1);
    }
    png_set_eXIf_1(png_ptr, info_ptr, static_cast<png_uint_32>(exif_len), mutable_exif);
    num_exif = 0;
    exif_ptr = NULL;
    (void)png_get_eXIf_1(png_ptr, info_ptr, &num_exif, &exif_ptr);
  }

  free(mutable_exif);
  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
  return 0;
}