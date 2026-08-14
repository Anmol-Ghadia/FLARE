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

static void png_fuzz_error_fn(png_structp png_ptr, png_const_charp)
{
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void png_fuzz_warning_fn(png_structp, png_const_charp)
{
}

static uint32_t read_u32(const uint8_t *data, size_t size, size_t offset)
{
  if (offset + 4 > size) return 0;
  return ((uint32_t)data[offset] << 24) |
         ((uint32_t)data[offset + 1] << 16) |
         ((uint32_t)data[offset + 2] << 8) |
         (uint32_t)data[offset + 3];
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
  FILE *fp = fopen("./dummy_file", "wb");
  if (fp != NULL)
  {
    if (Data != NULL && Size > 0)
      fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  png_structp write_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL,
                              png_fuzz_error_fn, png_fuzz_warning_fn);
  png_structp read_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL,
                             png_fuzz_error_fn, png_fuzz_warning_fn);

  if (write_ptr != NULL)
  {
    if (setjmp(png_jmpbuf(write_ptr)) == 0)
    {
      int v1 = (int)read_u32(Data, Size, 0);
      int v2 = (int)read_u32(Data, Size, 4);
      int v3 = (int)read_u32(Data, Size, 8);
      int v4 = (int)read_u32(Data, Size, 12);
      size_t s1 = (size_t)read_u32(Data, Size, 16);
      size_t s2 = (size_t)read_u32(Data, Size, 20);

      png_set_text_compression_window_bits(write_ptr, v1);
      png_set_text_compression_level(write_ptr, v2);
      png_set_compression_mem_level(write_ptr, v3);
      png_set_text_compression_mem_level(write_ptr, v4);

      png_set_compression_buffer_size(write_ptr, s1);
      png_set_compression_buffer_size(write_ptr, s2);

      png_set_text_compression_window_bits(write_ptr, -1);
      png_set_text_compression_window_bits(write_ptr, 0);
      png_set_text_compression_window_bits(write_ptr, 8);
      png_set_text_compression_window_bits(write_ptr, 15);
      png_set_text_compression_window_bits(write_ptr, 100);

      png_set_text_compression_level(write_ptr, -10);
      png_set_text_compression_level(write_ptr, 0);
      png_set_text_compression_level(write_ptr, 1);
      png_set_text_compression_level(write_ptr, 9);
      png_set_text_compression_level(write_ptr, 100);

      png_set_compression_mem_level(write_ptr, -1);
      png_set_compression_mem_level(write_ptr, 1);
      png_set_compression_mem_level(write_ptr, 8);
      png_set_compression_mem_level(write_ptr, 9);
      png_set_compression_mem_level(write_ptr, 100);

      png_set_text_compression_mem_level(write_ptr, -1);
      png_set_text_compression_mem_level(write_ptr, 1);
      png_set_text_compression_mem_level(write_ptr, 8);
      png_set_text_compression_mem_level(write_ptr, 9);
      png_set_text_compression_mem_level(write_ptr, 100);

      png_set_compression_buffer_size(write_ptr, 0);
      png_set_compression_buffer_size(write_ptr, 1);
      png_set_compression_buffer_size(write_ptr, 1024);
      png_set_compression_buffer_size(write_ptr, 65536);
      png_set_compression_buffer_size(write_ptr, (size_t)0x7fffffffU);
      png_set_compression_buffer_size(write_ptr, (size_t)0x80000000ULL);
      png_set_compression_buffer_size(write_ptr, (size_t)-1);
    }
  }

  if (read_ptr != NULL)
  {
    if (setjmp(png_jmpbuf(read_ptr)) == 0)
    {
      int rv = png_reset_zstream(read_ptr);
      (void)rv;

      png_set_compression_buffer_size(read_ptr, 1);
      png_set_compression_buffer_size(read_ptr, 4096);
      png_set_compression_buffer_size(read_ptr, 0);
      png_set_compression_buffer_size(read_ptr, (size_t)0x80000000ULL);

      rv = png_reset_zstream(read_ptr);
      (void)rv;
    }
  }

  png_set_text_compression_window_bits(NULL, (int)read_u32(Data, Size, 24));
  png_set_text_compression_level(NULL, (int)read_u32(Data, Size, 28));
  png_set_compression_mem_level(NULL, (int)read_u32(Data, Size, 32));
  png_set_text_compression_mem_level(NULL, (int)read_u32(Data, Size, 36));
  png_set_compression_buffer_size(NULL, (size_t)read_u32(Data, Size, 40));
  (void)png_reset_zstream(NULL);

  if (write_ptr != NULL)
    png_destroy_write_struct(&write_ptr, NULL);
  if (read_ptr != NULL)
    png_destroy_read_struct(&read_ptr, NULL, NULL);

  return 0;
}