// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_info_struct at png.c:386:1 in png.h
// png_destroy_info_struct at png.c:386:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_read_fn at pngrio.c:87:1 in png.h
// png_read_info at pngread.c:92:1 in png.h
// png_get_IHDR at pngget.c:825:1 in png.h
// png_read_update_info at pngread.c:267:1 in png.h
// png_get_rowbytes at pngget.c:30:1 in png.h
// png_read_image at pngread.c:700:1 in png.h
// png_read_end at pngread.c:766:1 in png.h
// png_read_end at pngread.c:766:1 in png.h
// png_destroy_info_struct at png.c:386:1 in png.h
// png_destroy_info_struct at png.c:386:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_set_tRNS at pngset.c:994:1 in png.h
// png_get_tRNS at pngget.c:1093:1 in png.h
// png_data_freer at png.c:452:1 in png.h
// png_set_invalid at pngset.c:1626:1 in png.h
// png_get_tRNS at pngget.c:1093:1 in png.h
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
#include <setjmp.h>

static void PngReadCallback(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead) {
  png_bytep io_ptr = static_cast<png_bytep>(png_get_io_ptr(png_ptr));
  if (io_ptr == NULL) {
    png_error(png_ptr, "no io ptr");
    return;
  }

  struct BufState {
    const uint8_t* data;
    size_t size;
    size_t offset;
  };

  BufState* st = reinterpret_cast<BufState*>(io_ptr);
  if (st->offset > st->size || byteCountToRead > st->size - st->offset) {
    png_error(png_ptr, "read overflow");
    return;
  }

  memcpy(outBytes, st->data + st->offset, byteCountToRead);
  st->offset += byteCountToRead;
}

static void ExerciseInfoAPIs(png_structp png_ptr, png_infop info_ptr,
                             const uint8_t* Data, size_t Size) {
  if (png_ptr == NULL || info_ptr == NULL) return;

  png_color_16 trans_color;
  memset(&trans_color, 0, sizeof(trans_color));
  if (Size > 0) trans_color.index = Data[0];
  if (Size > 2) trans_color.red = static_cast<png_uint_16>((Data[1] << 8) | Data[2]);
  if (Size > 4) trans_color.green = static_cast<png_uint_16>((Data[3] << 8) | Data[4]);
  if (Size > 6) trans_color.blue = static_cast<png_uint_16>((Data[5] << 8) | Data[6]);
  if (Size > 8) trans_color.gray = static_cast<png_uint_16>((Data[7] << 8) | Data[8]);

  int num_trans = 0;
  if (Size > 9) num_trans = Data[9] % 16;

  png_byte trans_alpha_local[16];
  memset(trans_alpha_local, 0, sizeof(trans_alpha_local));
  for (int i = 0; i < num_trans && 10u + static_cast<size_t>(i) < Size; ++i) {
    trans_alpha_local[i] = Data[10 + i];
  }

  png_set_tRNS(png_ptr, info_ptr,
               num_trans > 0 ? trans_alpha_local : NULL,
               num_trans,
               &trans_color);

  png_bytep out_alpha = NULL;
  int out_num_trans = 0;
  png_color_16p out_color = NULL;
  (void)png_get_tRNS(png_ptr, info_ptr, &out_alpha, &out_num_trans, &out_color);

  png_uint_32 masks[] = {
      0u,
      PNG_INFO_tRNS,
      PNG_INFO_PLTE,
      PNG_INFO_IDAT,
      PNG_INFO_tRNS | PNG_INFO_PLTE,
      0xffffffffu
  };

  int freer = (Size > 26 && (Data[26] & 1)) ? PNG_DESTROY_WILL_FREE_DATA
                                            : PNG_USER_WILL_FREE_DATA;
  png_uint_32 mask = masks[(Size > 27) ? (Data[27] % (sizeof(masks) / sizeof(masks[0]))) : 0];
  png_data_freer(png_ptr, info_ptr, freer, mask);

  int invalid_mask = (Size > 28) ? static_cast<int>(Data[28]) : 0;
  if (Size > 29 && (Data[29] & 1)) invalid_mask |= PNG_INFO_tRNS;
  if (Size > 30 && (Data[30] & 1)) invalid_mask |= PNG_INFO_PLTE;
  if (Size > 31 && (Data[31] & 1)) invalid_mask |= PNG_INFO_IDAT;
  png_set_invalid(png_ptr, info_ptr, invalid_mask);

  (void)png_get_tRNS(png_ptr, info_ptr, &out_alpha, &out_num_trans, &out_color);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE* fp = fopen("./dummy_file", "wb");
  if (fp != NULL) {
    if (Size > 0) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (png_ptr == NULL) return 0;

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == NULL) {
    png_destroy_read_struct(&png_ptr, NULL, NULL);
    return 0;
  }

  png_infop end_info_ptr = png_create_info_struct(png_ptr);

  if (setjmp(png_jmpbuf(png_ptr))) {
    if (end_info_ptr != NULL) {
      png_destroy_info_struct(png_ptr, &end_info_ptr);
    }
    if (info_ptr != NULL) {
      png_destroy_info_struct(png_ptr, &info_ptr);
    }
    png_destroy_read_struct(&png_ptr, NULL, NULL);
    return 0;
  }

  struct BufState {
    const uint8_t* data;
    size_t size;
    size_t offset;
  } st = { Data, Size, 0 };

  png_set_read_fn(png_ptr, &st, PngReadCallback);

  ExerciseInfoAPIs(png_ptr, info_ptr, Data, Size);

  png_read_info(png_ptr, info_ptr);

  ExerciseInfoAPIs(png_ptr, info_ptr, Data, Size);

  png_uint_32 width = 0, height = 0;
  int bit_depth = 0, color_type = 0, interlace_type = 0, compression_type = 0, filter_method = 0;
  if (png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
                   &interlace_type, &compression_type, &filter_method)) {
    if (width > 0 && height > 0 && width < 4096 && height < 4096) {
      png_read_update_info(png_ptr, info_ptr);
      png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);
      if (rowbytes > 0 && rowbytes < (1u << 20)) {
        png_bytep image = static_cast<png_bytep>(malloc(rowbytes * height));
        png_bytepp rows = static_cast<png_bytepp>(malloc(sizeof(png_bytep) * height));
        if (image != NULL && rows != NULL) {
          for (png_uint_32 y = 0; y < height; ++y) {
            rows[y] = image + y * rowbytes;
          }
          png_read_image(png_ptr, rows);
        }
        free(rows);
        free(image);
      }
    }
  }

  png_read_end(png_ptr, info_ptr);
  if (end_info_ptr != NULL) {
    png_read_end(png_ptr, end_info_ptr);
  }

  ExerciseInfoAPIs(png_ptr, info_ptr, Data, Size);
  if (end_info_ptr != NULL) {
    ExerciseInfoAPIs(png_ptr, end_info_ptr, Data, Size);
  }

  if (end_info_ptr != NULL) {
    png_destroy_info_struct(png_ptr, &end_info_ptr);
  }
  if (info_ptr != NULL) {
    png_destroy_info_struct(png_ptr, &info_ptr);
  }
  png_destroy_read_struct(&png_ptr, NULL, NULL);
  return 0;
}