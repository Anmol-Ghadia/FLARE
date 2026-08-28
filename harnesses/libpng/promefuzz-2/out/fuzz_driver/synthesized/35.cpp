// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_read_row at pngread.c:383:1 in png.h
// png_get_current_row_number at pngtrans.c:844:1 in png.h
// png_read_end at pngread.c:766:1 in png.h
// png_read_end at pngread.c:766:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_get_current_row_number at pngtrans.c:844:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_set_write_status_fn at pngwrite.c:1312:1 in png.h
// png_set_write_user_transform_fn at pngwrite.c:1322:1 in png.h
// png_set_write_status_fn at pngwrite.c:1312:1 in png.h
// png_set_write_user_transform_fn at pngwrite.c:1322:1 in png.h
// png_init_io at png.c:703:1 in png.h
// png_set_IHDR at pngset.c:254:1 in png.h
// png_get_current_row_number at pngtrans.c:844:1 in png.h
// png_write_info at pngwrite.c:192:1 in png.h
// png_get_rowbytes at pngget.c:30:1 in png.h
// png_write_row at pngwrite.c:693:1 in png.h
// png_get_current_row_number at pngtrans.c:844:1 in png.h
// png_write_end at pngwrite.c:358:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_get_current_row_number at pngtrans.c:844:1 in png.h
// png_get_current_row_number at pngtrans.c:844:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_set_user_transform_info at pngtrans.c:802:1 in png.h
// png_set_write_status_fn at pngwrite.c:1312:1 in png.h
// png_set_read_status_fn at pngread.c:1037:1 in png.h
// png_get_current_row_number at pngtrans.c:844:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_set_user_transform_info at pngtrans.c:802:1 in png.h
// png_set_read_status_fn at pngread.c:1037:1 in png.h
// png_set_read_user_transform_fn at pngrtran.c:1073:1 in png.h
// png_set_read_status_fn at pngread.c:1037:1 in png.h
// png_set_read_user_transform_fn at pngrtran.c:1073:1 in png.h
// png_set_user_transform_info at pngtrans.c:802:1 in png.h
// png_init_io at png.c:703:1 in png.h
// png_read_info at pngread.c:92:1 in png.h
// png_set_user_transform_info at pngtrans.c:802:1 in png.h
// png_read_update_info at pngread.c:267:1 in png.h
// png_get_image_height at pngget.c:61:1 in png.h
// png_get_rowbytes at pngget.c:30:1 in png.h
#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <setjmp.h>
#include <png.h>

static void ReadStatusCallback(png_structp png_ptr, png_uint_32 row, int pass) {
  (void)row;
  (void)pass;
  (void)png_get_current_row_number(png_ptr);
}

static void WriteStatusCallback(png_structp png_ptr, png_uint_32 row, int pass) {
  (void)row;
  (void)pass;
  (void)png_get_current_row_number(png_ptr);
}

static void ReadUserTransformCallback(png_structp png_ptr, png_row_infop row_info,
                                      png_bytep data) {
  (void)row_info;
  (void)data;
  (void)png_get_current_row_number(png_ptr);
}

static void WriteUserTransformCallback(png_structp png_ptr, png_row_infop row_info,
                                       png_bytep data) {
  (void)row_info;
  (void)data;
  (void)png_get_current_row_number(png_ptr);
}

static void PngErrorCallback(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningCallback(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput_35(const uint8_t *Data, size_t Size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (fp != NULL) {
    if (Size > 0) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  png_set_user_transform_info(NULL, NULL, 0, 0);
  png_set_write_status_fn(NULL, NULL);
  png_set_read_status_fn(NULL, NULL);
  (void)png_get_current_row_number(NULL);

  png_structp read_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, PngErrorCallback, PngWarningCallback);
  png_infop read_info = NULL;
  if (read_ptr != NULL) {
    read_info = png_create_info_struct(read_ptr);
  }

  if (read_ptr != NULL && read_info != NULL) {
    if (setjmp(png_jmpbuf(read_ptr)) == 0) {
      int depth = Size > 0 ? (int)Data[0] : 0;
      int channels = Size > 1 ? (int)Data[1] : 0;
      png_voidp user_ptr = (Size > 2) ? (png_voidp)(uintptr_t)Data[2] : NULL;

      png_set_user_transform_info(read_ptr, user_ptr, depth, channels);
      png_set_read_status_fn(read_ptr, ReadStatusCallback);
      png_set_read_user_transform_fn(read_ptr, ReadUserTransformCallback);

      if (Size > 2 && (Data[2] & 1) == 0) {
        png_set_read_status_fn(read_ptr, NULL);
      }
      if (Size > 3 && (Data[3] & 1) == 0) {
        png_set_read_user_transform_fn(read_ptr, NULL);
      }
      if (Size > 4) {
        png_set_user_transform_info(read_ptr, (png_voidp)(uintptr_t)Data[4],
                                    (int)(signed char)Data[4],
                                    (int)((Data[4] % 8) + 1));
      }

      FILE *rf = fopen("./dummy_file", "rb");
      if (rf != NULL) {
        png_init_io(read_ptr, rf);
        png_read_info(read_ptr, read_info);

        if (Size > 5) {
          png_set_user_transform_info(read_ptr, (png_voidp)(uintptr_t)Data[5],
                                      (int)Data[5], (int)(Data[5] % 16));
        }

        png_read_update_info(read_ptr, read_info);

        png_uint_32 height = png_get_image_height(read_ptr, read_info);
        png_size_t rowbytes = png_get_rowbytes(read_ptr, read_info);

        if (height > 0 && rowbytes > 0 && rowbytes < (1u << 24) && height < (1u << 20)) {
          png_bytep row = (png_bytep)malloc(rowbytes);
          if (row != NULL) {
            for (png_uint_32 y = 0; y < height; ++y) {
              png_read_row(read_ptr, row, NULL);
              (void)png_get_current_row_number(read_ptr);
            }
            png_read_end(read_ptr, read_info);
            free(row);
          }
        } else {
          png_read_end(read_ptr, read_info);
        }

        fclose(rf);
      }
    }
  }

  if (read_ptr != NULL) {
    png_destroy_read_struct(&read_ptr, &read_info, NULL);
  }

  png_structp write_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, PngErrorCallback, PngWarningCallback);
  png_infop write_info = NULL;
  if (write_ptr != NULL) {
    write_info = png_create_info_struct(write_ptr);
  }

  if (write_ptr != NULL && write_info != NULL) {
    if (setjmp(png_jmpbuf(write_ptr)) == 0) {
      png_set_write_status_fn(write_ptr, WriteStatusCallback);
      png_set_write_user_transform_fn(write_ptr, WriteUserTransformCallback);

      if (Size > 6 && (Data[6] & 1) == 0) {
        png_set_write_status_fn(write_ptr, NULL);
      }
      if (Size > 7 && (Data[7] & 1) == 0) {
        png_set_write_user_transform_fn(write_ptr, NULL);
      }

      FILE *wf = fopen("./dummy_file", "wb");
      if (wf != NULL) {
        png_init_io(write_ptr, wf);

        png_uint_32 width = 1;
        png_uint_32 height = 1;
        int bit_depth = 8;
        int color_type = PNG_COLOR_TYPE_RGB;

        if (Size > 8) {
          width = (png_uint_32)(Data[8] % 8 + 1);
        }
        if (Size > 9) {
          height = (png_uint_32)(Data[9] % 8 + 1);
        }
        if (Size > 10) {
          switch (Data[10] % 5) {
            case 0: color_type = PNG_COLOR_TYPE_GRAY; break;
            case 1: color_type = PNG_COLOR_TYPE_GRAY_ALPHA; break;
            case 2: color_type = PNG_COLOR_TYPE_RGB; break;
            case 3: color_type = PNG_COLOR_TYPE_RGB_ALPHA; break;
            default: color_type = PNG_COLOR_TYPE_PALETTE; break;
          }
        }
        if (Size > 11) {
          switch (Data[11] % 4) {
            case 0: bit_depth = 1; break;
            case 1: bit_depth = 2; break;
            case 2: bit_depth = 4; break;
            default: bit_depth = 8; break;
          }
          if (color_type == PNG_COLOR_TYPE_RGB ||
              color_type == PNG_COLOR_TYPE_RGB_ALPHA ||
              color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
            bit_depth = 8;
          }
        }

        png_set_IHDR(write_ptr, write_info, width, height, bit_depth, color_type,
                     PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE,
                     PNG_FILTER_TYPE_BASE);

        png_write_info(write_ptr, write_info);

        png_size_t rowbytes = png_get_rowbytes(write_ptr, write_info);
        if (rowbytes == 0) {
          rowbytes = width * 4;
        }

        png_bytep row = (png_bytep)malloc(rowbytes);
        if (row != NULL) {
          for (png_uint_32 y = 0; y < height; ++y) {
            for (png_size_t i = 0; i < rowbytes; ++i) {
              row[i] = (Size > 0) ? Data[(i + y) % Size] : 0;
            }
            png_write_row(write_ptr, row);
            (void)png_get_current_row_number(write_ptr);
          }
          free(row);
        }

        png_write_end(write_ptr, write_info);
        fclose(wf);
      }
    }
  }

  if (write_ptr != NULL) {
    png_destroy_write_struct(&write_ptr, &write_info);
  }

  return 0;
}