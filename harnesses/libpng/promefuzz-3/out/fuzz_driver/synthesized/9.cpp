// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_read_image at pngread.c:700:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_set_mem_fn at pngmem.c:260:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_set_compression_buffer_size at pngset.c:1566:1 in png.h
// png_malloc_default at pngmem.c:188:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_set_write_fn at pngwio.c:122:1 in png.h
// png_set_IHDR at pngset.c:254:1 in png.h
// png_write_info at pngwrite.c:192:1 in png.h
// png_write_image at pngwrite.c:596:1 in png.h
// png_write_end at pngwrite.c:358:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_set_mem_fn at pngmem.c:260:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_set_compression_buffer_size at pngset.c:1566:1 in png.h
// png_malloc_default at pngmem.c:188:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_set_read_fn at pngrio.c:87:1 in png.h
// png_read_info at pngread.c:92:1 in png.h
// png_get_image_width at pngget.c:52:1 in png.h
// png_get_image_height at pngget.c:61:1 in png.h
// png_get_bit_depth at pngget.c:70:1 in png.h
// png_get_color_type at pngget.c:79:1 in png.h
// png_set_strip_16 at pngrtran.c:197:1 in png.h
// png_set_palette_to_rgb at pngrtran.c:902:1 in png.h
// png_set_expand_gray_1_2_4_to_8 at pngrtran.c:914:1 in png.h
// png_get_valid at pngget.c:20:1 in png.h
// png_set_tRNS_to_alpha at pngrtran.c:926:1 in png.h
// png_read_update_info at pngread.c:267:1 in png.h
// png_get_rowbytes at pngget.c:30:1 in png.h
#include <png.h>
#include <setjmp.h>
#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>

static png_voidp PNGCBAPI custom_malloc_fn(png_structp /*png_ptr*/, png_alloc_size_t size) {
  if (size == 0) return nullptr;
  return malloc(size);
}

static void PNGCBAPI custom_free_fn(png_structp /*png_ptr*/, png_voidp ptr) {
  free(ptr);
}

static void write_data_to_dummy_file(const uint8_t *Data, size_t Size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (!fp) return;
  if (Size > 0) fwrite(Data, 1, Size, fp);
  fclose(fp);
}

static void read_data_fn(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead) {
  FILE *fp = reinterpret_cast<FILE*>(png_get_io_ptr(png_ptr));
  if (!fp || fread(outBytes, 1, byteCountToRead, fp) != byteCountToRead) {
    png_error(png_ptr, "read error");
  }
}

static void write_data_fn(png_structp png_ptr, png_bytep data, png_size_t length) {
  FILE *fp = reinterpret_cast<FILE*>(png_get_io_ptr(png_ptr));
  if (!fp || fwrite(data, 1, length, fp) != length) {
    png_error(png_ptr, "write error");
  }
}

static void flush_data_fn(png_structp png_ptr) {
  FILE *fp = reinterpret_cast<FILE*>(png_get_io_ptr(png_ptr));
  if (fp) fflush(fp);
}

extern "C" int LLVMFuzzerTestOneInput_9(const uint8_t *Data, size_t Size) {
  write_data_to_dummy_file(Data, Size);

  if (Size == 0) return 0;

  {
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    png_infop info_ptr = nullptr;

    if (png_ptr) {
      png_set_mem_fn(png_ptr, nullptr, custom_malloc_fn, custom_free_fn);

      png_set_longjmp_fn(png_ptr, longjmp, sizeof(jmp_buf));
      if (setjmp(*png_set_longjmp_fn(png_ptr, longjmp, sizeof(jmp_buf))) == 0) {
        size_t buf_size = 1 + (Data[0] % 64);
        png_set_compression_buffer_size(png_ptr, buf_size);

        png_voidp mem = png_malloc_default(png_ptr, static_cast<png_alloc_size_t>((Size % 1024) + 1));
        if (mem) {
          memset(mem, 0, (Size % 1024) + 1);
          custom_free_fn(png_ptr, mem);
        }

        info_ptr = png_create_info_struct(png_ptr);
        if (info_ptr) {
          FILE *fp = fopen("./dummy_file", "rb");
          if (fp) {
            png_set_read_fn(png_ptr, fp, read_data_fn);
            png_read_info(png_ptr, info_ptr);

            png_uint_32 width = png_get_image_width(png_ptr, info_ptr);
            png_uint_32 height = png_get_image_height(png_ptr, info_ptr);
            int bit_depth = png_get_bit_depth(png_ptr, info_ptr);
            int color_type = png_get_color_type(png_ptr, info_ptr);

            if (bit_depth == 16) png_set_strip_16(png_ptr);
            if (color_type == PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png_ptr);
            if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) png_set_expand_gray_1_2_4_to_8(png_ptr);
            if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png_ptr);

            png_read_update_info(png_ptr, info_ptr);

            png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);
            if (width > 0 && height > 0 && rowbytes > 0 && height < 4096 && rowbytes < (1u << 20)) {
              png_bytep *rows = static_cast<png_bytep*>(malloc(sizeof(png_bytep) * height));
              if (rows) {
                bool ok = true;
                for (png_uint_32 y = 0; y < height; ++y) {
                  rows[y] = static_cast<png_bytep>(malloc(rowbytes));
                  if (!rows[y]) {
                    ok = false;
                    for (png_uint_32 i = 0; i < y; ++i) free(rows[i]);
                    free(rows);
                    rows = nullptr;
                    break;
                  }
                }
                if (ok && rows) {
                  png_read_image(png_ptr, rows);
                  for (png_uint_32 y = 0; y < height; ++y) free(rows[y]);
                  free(rows);
                }
              }
            }
            fclose(fp);
          }
        }
      }

      if (info_ptr) png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
      else png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    }
  }

  {
    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    png_infop info_ptr = nullptr;

    if (png_ptr) {
      png_set_mem_fn(png_ptr, nullptr, custom_malloc_fn, custom_free_fn);

      png_set_longjmp_fn(png_ptr, longjmp, sizeof(jmp_buf));
      if (setjmp(*png_set_longjmp_fn(png_ptr, longjmp, sizeof(jmp_buf))) == 0) {
        size_t buf_size = 1 + ((Size > 1 ? Data[1] : Data[0]) % 128);
        png_set_compression_buffer_size(png_ptr, buf_size);

        png_voidp mem = png_malloc_default(png_ptr, static_cast<png_alloc_size_t>(((Size + 7) % 2048) + 1));
        if (mem) {
          memset(mem, 0xA5, ((Size + 7) % 2048) + 1);
          custom_free_fn(png_ptr, mem);
        }

        info_ptr = png_create_info_struct(png_ptr);
        if (info_ptr) {
          FILE *fp = fopen("./dummy_file", "wb");
          if (fp) {
            png_set_write_fn(png_ptr, fp, write_data_fn, flush_data_fn);

            png_uint_32 width = (Size > 2 ? Data[2] : 1) % 32 + 1;
            png_uint_32 height = (Size > 3 ? Data[3] : 1) % 32 + 1;
            int color_type = (Size > 4 && (Data[4] & 1)) ? PNG_COLOR_TYPE_RGBA : PNG_COLOR_TYPE_RGB;
            int bit_depth = 8;

            png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, color_type,
                         PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
            png_write_info(png_ptr, info_ptr);

            int channels = (color_type == PNG_COLOR_TYPE_RGBA) ? 4 : 3;
            png_size_t rowbytes = width * channels;
            png_bytep *rows = static_cast<png_bytep*>(malloc(sizeof(png_bytep) * height));
            if (rows) {
              bool ok = true;
              size_t offset = 5;
              for (png_uint_32 y = 0; y < height; ++y) {
                rows[y] = static_cast<png_bytep>(malloc(rowbytes));
                if (!rows[y]) {
                  ok = false;
                  for (png_uint_32 i = 0; i < y; ++i) free(rows[i]);
                  free(rows);
                  rows = nullptr;
                  break;
                }
                for (png_size_t x = 0; x < rowbytes; ++x) {
                  rows[y][x] = (offset < Size) ? Data[offset++] : static_cast<uint8_t>((x + y) & 0xFF);
                }
              }

              if (ok && rows) {
                png_write_image(png_ptr, rows);
                png_write_end(png_ptr, info_ptr);
                for (png_uint_32 y = 0; y < height; ++y) free(rows[y]);
                free(rows);
              }
            }
            fclose(fp);
          }
        }
      }

      if (info_ptr) png_destroy_write_struct(&png_ptr, &info_ptr);
      else png_destroy_write_struct(&png_ptr, nullptr);
    }
  }

  return 0;
}