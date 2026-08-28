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

static png_voidp FuzzMalloc(png_structp, png_alloc_size_t size) {
  if (size == 0) size = 1;
  return malloc((size_t)size);
}

static void FuzzFree(png_structp, png_voidp ptr) {
  free(ptr);
}

static void FuzzError(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void FuzzWarning(png_structp, png_const_charp) {
}

static png_uint_32 ReadU32(const uint8_t *data, size_t size, size_t off) {
  if (off + 4 > size) return 0;
  return ((png_uint_32)data[off] << 24) |
         ((png_uint_32)data[off + 1] << 16) |
         ((png_uint_32)data[off + 2] << 8) |
         (png_uint_32)data[off + 3];
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (fp != NULL) {
    if (Size > 0) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (png_ptr == NULL) return 0;

  png_set_error_fn(png_ptr, NULL, FuzzError, FuzzWarning);
  png_set_mem_fn(png_ptr, NULL, FuzzMalloc, FuzzFree);

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_write_struct(&png_ptr, NULL);
    return 0;
  }

  png_inforp info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == NULL) {
    png_destroy_write_struct(&png_ptr, NULL);
    return 0;
  }

  size_t name_len = Size > 0 ? (Data[0] % 32) : 0;
  if (name_len > Size) name_len = Size;
  char name_buf[64];
  size_t copy_len = name_len < sizeof(name_buf) - 1 ? name_len : sizeof(name_buf) - 1;
  if (copy_len > 0) memcpy(name_buf, Data + 1 <= Data + Size ? Data + 1 : Data, copy_len);
  name_buf[copy_len] = '\0';
  const char *name = copy_len ? name_buf : "ICC";

  size_t profile_off = 1 + copy_len;
  if (profile_off > Size) profile_off = Size;
  png_uint_32 profile_len = (png_uint_32)(Size - profile_off);
  const png_byte *profile = profile_len ? (const png_byte *)(Data + profile_off) : (const png_byte *)"";

  int compression_type = PNG_COMPRESSION_TYPE_BASE;
  if (Size > 1 && (Data[1] & 1)) {
    compression_type = (int)(Data[1]);
  }

  png_uint_32 masks[] = {
      0,
      PNG_FREE_ALL,
      PNG_FREE_ICCP,
      PNG_FREE_TEXT,
      PNG_FREE_TRNS,
      PNG_FREE_HIST,
      PNG_FREE_PLTE,
      PNG_FREE_ROWS,
      PNG_FREE_PCAL,
      PNG_FREE_SCAL,
      PNG_FREE_SPLT,
      PNG_FREE_UNKN,
      ReadU32(Data, Size, 2)
  };

  int freer_values[] = {
      PNG_DESTROY_WILL_FREE_DATA,
      PNG_USER_WILL_FREE_DATA
  };

  png_set_iCCP(png_ptr, info_ptr, name, PNG_COMPRESSION_TYPE_BASE, profile, profile_len);

  if (Size > 2) {
    png_set_iCCP(png_ptr, info_ptr, name, compression_type, profile, profile_len);
  }

  for (size_t i = 0; i < sizeof(freer_values) / sizeof(freer_values[0]); ++i) {
    for (size_t j = 0; j < sizeof(masks) / sizeof(masks[0]); ++j) {
      png_data_freer(png_ptr, info_ptr, freer_values[i], masks[j]);
      (void)png_get_valid(png_ptr, info_ptr, masks[j]);
      png_set_invalid(png_ptr, info_ptr, (int)masks[j]);
      (void)png_get_valid(png_ptr, info_ptr, masks[j]);
    }
  }

  png_charp out_name = NULL;
  int out_comp = -1;
  png_bytep out_profile = NULL;
  png_uint_32 out_len = 0;
  (void)png_get_iCCP(png_ptr, info_ptr, &out_name, &out_comp, &out_profile, &out_len);
  (void)png_get_valid(png_ptr, info_ptr, PNG_INFO_iCCP);

  if (Size > 8) {
    png_set_invalid(png_ptr, info_ptr, (int)ReadU32(Data, Size, 4));
    (void)png_get_valid(png_ptr, info_ptr, ReadU32(Data, Size, 4));
  }

  png_destroy_info_struct(png_ptr, &info_ptr);
  png_destroy_write_struct(&png_ptr, NULL);
  return 0;
}