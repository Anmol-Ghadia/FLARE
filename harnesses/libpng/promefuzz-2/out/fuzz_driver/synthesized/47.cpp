// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_write_chunk at pngwutil.c:192:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_save_uint_16 at pngwutil.c:37:1 in png.h
// png_save_uint_32 at pngwutil.c:24:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_get_uint_31 at pngrutil.c:22:1 in png.h
// png_set_write_fn at pngwio.c:122:1 in png.h
// png_write_chunk at pngwutil.c:192:1 in png.h
// png_save_uint_16 at pngwutil.c:37:1 in png.h
// png_save_uint_32 at pngwutil.c:24:1 in png.h
// png_save_uint_32 at pngwutil.c:24:1 in png.h
// png_write_chunk at pngwutil.c:192:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
#include <png.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <setjmp.h>
#include <string.h>

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

static void PngWriteFn(png_structp png_ptr, png_bytep data, png_size_t length) {
  FILE* fp = (FILE*)png_get_io_ptr(png_ptr);
  if (fp != NULL && data != NULL && length > 0) {
    fwrite(data, 1, length, fp);
  }
}

static void PngFlushFn(png_structp png_ptr) {
  FILE* fp = (FILE*)png_get_io_ptr(png_ptr);
  if (fp != NULL) {
    fflush(fp);
  }
}

extern "C" int LLVMFuzzerTestOneInput_47(const uint8_t *Data, size_t Size) {
  png_byte local4[4] = {0};
  png_byte local2[2] = {0};

  if (Size > 0) {
    size_t copy_len = Size < sizeof(local4) ? Size : sizeof(local4);
    memcpy(local4, Data, copy_len);
    if (Size >= 2) {
      memcpy(local2, Data, 2);
    } else {
      memcpy(local2, Data, copy_len < sizeof(local2) ? copy_len : sizeof(local2));
    }
  }

  unsigned int input16 = (unsigned int)Size;
  if (Size >= 2) {
    input16 = ((unsigned int)Data[0] << 8) | (unsigned int)Data[1];
  }

  png_save_uint_16(local2, input16);
  png_uint_16 v16 = png_get_uint_16(local2);

  png_uint_32 seed32 = (png_uint_32)Size;
  if (Size >= 4) {
    seed32 = png_get_uint_32((png_const_bytep)Data);
  }
  png_save_uint_32(local4, seed32);
  png_uint_32 v32 = png_get_uint_32(local4);

  png_structp png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, PngErrorFn, PngWarningFn);
  if (png_ptr == NULL) {
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_write_struct(&png_ptr, NULL);
    return 0;
  }

  png_uint_32 v31 = png_get_uint_31(png_ptr, local4);

  FILE* fp = fopen("./dummy_file", "wb");
  if (fp != NULL) {
    png_set_write_fn(png_ptr, fp, PngWriteFn, PngFlushFn);

    png_byte chunk_name1[4] = {'t', 'E', 'S', 'T'};
    if (Size >= 4) {
      memcpy(chunk_name1, Data, 4);
    }

    const png_byte* chunk_data1 = Data;
    size_t chunk_len1 = Size;
    if (chunk_len1 > 1024) {
      chunk_len1 = 1024;
    }
    png_write_chunk(png_ptr, chunk_name1, chunk_data1, chunk_len1);

    png_byte derived_chunk[10];
    png_save_uint_16(derived_chunk, v16);
    png_save_uint_32(derived_chunk + 2, v32);
    png_save_uint_32(derived_chunk + 6, v31);

    png_byte chunk_name2[4] = {'F', 'U', 'Z', 'Z'};
    png_write_chunk(png_ptr, chunk_name2, derived_chunk, sizeof(derived_chunk));

    if (Size >= 8) {
      png_byte chunk_name3[4];
      memcpy(chunk_name3, Data + 4, 4);
      const png_byte* chunk_data3 = Data + 8;
      size_t chunk_len3 = Size - 8;
      if (chunk_len3 > 1024) {
        chunk_len3 = 1024;
      }
      png_write_chunk(png_ptr, chunk_name3, chunk_data3, chunk_len3);
    }

    fclose(fp);
  }

  png_destroy_write_struct(&png_ptr, NULL);
  return 0;
}