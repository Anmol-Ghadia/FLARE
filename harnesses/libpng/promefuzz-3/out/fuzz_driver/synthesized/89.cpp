// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_write_chunk_start at pngwutil.c:115:1 in png.h
// png_write_chunk_data at pngwutil.c:127:1 in png.h
// png_write_chunk_end at pngwutil.c:146:1 in png.h
// png_write_chunk_start at pngwutil.c:115:1 in png.h
// png_write_chunk_end at pngwutil.c:146:1 in png.h
// png_write_chunk_end at pngwutil.c:146:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_save_uint_32 at pngwutil.c:24:1 in png.h
// png_save_int_32 at png.c:726:1 in png.h
// png_save_uint_16 at pngwutil.c:37:1 in png.h
// png_save_uint_32 at pngwutil.c:24:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_init_io at png.c:703:1 in png.h
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
#include <string.h>
#include <vector>

static void PngErrorFn(png_structp, png_const_charp)
{
  longjmp(png_jmpbuf((png_structp)0), 1);
}

static void PngWarningFn(png_structp, png_const_charp)
{
}

extern "C" int LLVMFuzzerTestOneInput_89(const uint8_t *Data, size_t Size)
{
  png_byte buf4[4] = {0, 0, 0, 0};
  png_byte buf2[2] = {0, 0};

  png_uint_32 u32 = 0;
  png_int_32 s32 = 0;
  unsigned int u16 = 0;

  if (Size >= 4)
  {
    u32 = ((png_uint_32)Data[0] << 24) |
          ((png_uint_32)Data[1] << 16) |
          ((png_uint_32)Data[2] << 8) |
          (png_uint_32)Data[3];
    s32 = (png_int_32)u32;
  }
  else
  {
    for (size_t i = 0; i < Size; ++i)
      u32 = (u32 << 8) | Data[i];
    s32 = (png_int_32)u32;
  }

  if (Size >= 2)
    u16 = ((unsigned int)Data[0] << 8) | (unsigned int)Data[1];
  else if (Size == 1)
    u16 = Data[0];

  png_save_uint_32(buf4, u32);
  png_get_int_32(buf4);
  png_save_int_32(buf4, s32);
  png_get_int_32(buf4);
  png_save_uint_16(buf2, u16);

  if (Size >= 8)
  {
    png_save_uint_32(buf4, ((png_uint_32)Data[4] << 24) |
                           ((png_uint_32)Data[5] << 16) |
                           ((png_uint_32)Data[6] << 8) |
                           (png_uint_32)Data[7]);
    png_get_int_32(buf4);
  }

  FILE *fp = fopen("./dummy_file", "wb");
  if (!fp)
    return 0;

  png_structp png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png_ptr)
  {
    fclose(fp);
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr)
  {
    png_destroy_write_struct(&png_ptr, NULL);
    fclose(fp);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr)))
  {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
    return 0;
  }

  png_init_io(png_ptr, fp);

  png_byte sig[8] = {137, 80, 78, 71, 13, 10, 26, 10};
  fwrite(sig, 1, 8, fp);

  png_byte chunk_name[4] = {'f', 'U', 'Z', 'z'};
  if (Size >= 12)
  {
    for (int i = 0; i < 4; ++i)
    {
      png_byte c = Data[8 + i];
      if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
        chunk_name[i] = c;
    }
  }

  png_uint_32 chunk_len = 0;
  if (Size > 12)
  {
    size_t remain = Size - 12;
    chunk_len = (png_uint_32)(remain > 1024 ? 1024 : remain);
  }

  png_write_chunk_start(png_ptr, chunk_name, chunk_len);
  if (chunk_len > 0)
    png_write_chunk_data(png_ptr, Data + 12, chunk_len);
  png_write_chunk_end(png_ptr);

  png_write_chunk_start(png_ptr, (png_const_bytep)"IEND", 0);
  png_write_chunk_end(png_ptr);

  png_write_chunk_end(NULL);

  png_destroy_write_struct(&png_ptr, &info_ptr);
  fclose(fp);
  return 0;
}