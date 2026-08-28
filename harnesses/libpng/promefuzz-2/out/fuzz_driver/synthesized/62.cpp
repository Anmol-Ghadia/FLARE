// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_get_compression_buffer_size at pngget.c:1169:1 in png.h
// png_set_compression_method at pngwrite.c:1219:1 in png.h
// png_set_compression_mem_level at pngwrite.c:1164:1 in png.h
// png_set_text_compression_window_bits at pngwrite.c:1275:1 in png.h
// png_set_flush at pngwrite.c:903:1 in png.h
// png_set_compression_method at pngwrite.c:1219:1 in png.h
// png_set_compression_mem_level at pngwrite.c:1164:1 in png.h
// png_set_text_compression_window_bits at pngwrite.c:1275:1 in png.h
// png_set_flush at pngwrite.c:903:1 in png.h
// png_get_compression_buffer_size at pngget.c:1169:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_get_compression_buffer_size at pngget.c:1169:1 in png.h
// png_set_compression_buffer_size at pngset.c:1566:1 in png.h
// png_set_compression_method at pngwrite.c:1219:1 in png.h
// png_set_compression_mem_level at pngwrite.c:1164:1 in png.h
// png_set_text_compression_window_bits at pngwrite.c:1275:1 in png.h
// png_set_flush at pngwrite.c:903:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_get_compression_buffer_size at pngget.c:1169:1 in png.h
// png_set_compression_buffer_size at pngset.c:1566:1 in png.h
// png_get_compression_buffer_size at pngget.c:1169:1 in png.h
// png_set_compression_buffer_size at pngset.c:1566:1 in png.h
// png_get_compression_buffer_size at pngget.c:1169:1 in png.h
// png_set_compression_buffer_size at pngset.c:1566:1 in png.h
// png_get_compression_buffer_size at pngget.c:1169:1 in png.h
// png_set_compression_method at pngwrite.c:1219:1 in png.h
// png_set_compression_mem_level at pngwrite.c:1164:1 in png.h
// png_set_text_compression_window_bits at pngwrite.c:1275:1 in png.h
// png_set_flush at pngwrite.c:903:1 in png.h
// png_set_compression_method at pngwrite.c:1219:1 in png.h
// png_set_compression_mem_level at pngwrite.c:1164:1 in png.h
// png_set_text_compression_window_bits at pngwrite.c:1275:1 in png.h
// png_set_flush at pngwrite.c:903:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_get_compression_buffer_size at pngget.c:1169:1 in png.h
// png_set_compression_buffer_size at pngset.c:1566:1 in png.h
// png_get_compression_buffer_size at pngget.c:1169:1 in png.h
// png_set_compression_buffer_size at pngset.c:1566:1 in png.h
// png_get_compression_buffer_size at pngget.c:1169:1 in png.h
// png_set_compression_buffer_size at pngset.c:1566:1 in png.h
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
#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>

static void PngErrorFn(png_structp, png_const_charp)
{
  /* Prevent libpng from aborting the process; actual longjmp target is set by
   * setjmp(png_jmpbuf(...)) in the caller.
   */
}

static void PngWarningFn(png_structp, png_const_charp)
{
}

static uint32_t ReadU32(const uint8_t *data, size_t size, size_t offset)
{
  if (offset + 4 > size) return 0;
  return (static_cast<uint32_t>(data[offset]) << 24) |
         (static_cast<uint32_t>(data[offset + 1]) << 16) |
         (static_cast<uint32_t>(data[offset + 2]) << 8) |
         static_cast<uint32_t>(data[offset + 3]);
}

extern "C" int LLVMFuzzerTestOneInput_62(const uint8_t *Data, size_t Size)
{
  FILE *fp = std::fopen("./dummy_file", "wb");
  if (fp != nullptr)
  {
    if (Size > 0) std::fwrite(Data, 1, Size, fp);
    std::fclose(fp);
  }

  png_structp read_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn, PngWarningFn);
  png_structp write_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn, PngWarningFn);

  if (read_ptr != nullptr)
  {
    if (setjmp(png_jmpbuf(read_ptr)) == 0)
    {
      size_t s1 = Size > 0 ? static_cast<size_t>(Data[0]) : 0;
      size_t s2 = ReadU32(Data, Size, 1);
      size_t s3 = (Size >= 9) ? (static_cast<size_t>(ReadU32(Data, Size, 5)) & 0x7fffffffU) : 1;

      png_get_compression_buffer_size(read_ptr);
      png_set_compression_buffer_size(read_ptr, s1);
      png_get_compression_buffer_size(read_ptr);
      png_set_compression_buffer_size(read_ptr, s2);
      png_get_compression_buffer_size(read_ptr);
      png_set_compression_buffer_size(read_ptr, s3);
      png_get_compression_buffer_size(read_ptr);

      int method = (Size > 9) ? static_cast<int>(static_cast<int8_t>(Data[9])) : 8;
      int mem_level = (Size > 10) ? static_cast<int>(static_cast<int8_t>(Data[10])) : 0;
      int text_window_bits = (Size > 11) ? static_cast<int>(static_cast<int8_t>(Data[11])) : 8;
      int flush_rows = (Size > 12) ? static_cast<int>(static_cast<int8_t>(Data[12])) : 0;

      png_set_compression_method(read_ptr, method);
      png_set_compression_mem_level(read_ptr, mem_level);
      png_set_text_compression_window_bits(read_ptr, text_window_bits);
      png_set_flush(read_ptr, flush_rows);

      png_set_compression_method(read_ptr, 8);
      png_set_compression_mem_level(read_ptr, mem_level * 2);
      png_set_text_compression_window_bits(read_ptr, text_window_bits + 20);
      png_set_flush(read_ptr, -flush_rows);
    }

    png_destroy_read_struct(&read_ptr, nullptr, nullptr);
  }

  if (write_ptr != nullptr)
  {
    if (setjmp(png_jmpbuf(write_ptr)) == 0)
    {
      size_t s1 = Size > 13 ? static_cast<size_t>(Data[13]) : 1;
      size_t s2 = ReadU32(Data, Size, 14);
      size_t s3 = (Size >= 22) ? (static_cast<size_t>(ReadU32(Data, Size, 18)) & 0x7fffffffU) : 1024;

      png_get_compression_buffer_size(write_ptr);
      png_set_compression_buffer_size(write_ptr, s1);
      png_get_compression_buffer_size(write_ptr);
      png_set_compression_buffer_size(write_ptr, s2);
      png_get_compression_buffer_size(write_ptr);
      png_set_compression_buffer_size(write_ptr, s3);
      png_get_compression_buffer_size(write_ptr);

      int method = (Size > 22) ? static_cast<int>(static_cast<int8_t>(Data[22])) : 8;
      int mem_level = (Size > 23) ? static_cast<int>(static_cast<int8_t>(Data[23])) : 9;
      int text_window_bits = (Size > 24) ? static_cast<int>(static_cast<int8_t>(Data[24])) : 15;
      int flush_rows = (Size > 25) ? static_cast<int>(static_cast<int8_t>(Data[25])) : 1;

      png_set_compression_method(write_ptr, method);
      png_set_compression_mem_level(write_ptr, mem_level);
      png_set_text_compression_window_bits(write_ptr, text_window_bits);
      png_set_flush(write_ptr, flush_rows);

      png_set_compression_method(write_ptr, method ^ 8);
      png_set_compression_mem_level(write_ptr, mem_level - 20);
      png_set_text_compression_window_bits(write_ptr, text_window_bits - 30);
      png_set_flush(write_ptr, 0);

      png_get_compression_buffer_size(write_ptr);
    }

    png_destroy_write_struct(&write_ptr, nullptr);
  }

  png_get_compression_buffer_size(nullptr);
  png_set_compression_buffer_size(nullptr, 1);
  png_set_compression_method(nullptr, 8);
  png_set_compression_mem_level(nullptr, 0);
  png_set_text_compression_window_bits(nullptr, 8);
  png_set_flush(nullptr, 0);

  return 0;
}