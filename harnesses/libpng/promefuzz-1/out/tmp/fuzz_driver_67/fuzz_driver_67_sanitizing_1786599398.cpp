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
#include <vector>

static void PngErrorFn(png_structp, png_const_charp)
{
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp)
{
}

static void WriteDataFn(png_structp png_ptr, png_bytep data, png_size_t length)
{
  FILE* fp = reinterpret_cast<FILE*>(png_get_io_ptr(png_ptr));
  if (fp != nullptr && data != nullptr && length > 0)
    fwrite(data, 1, length, fp);
}

static void FlushFn(png_structp png_ptr)
{
  FILE* fp = reinterpret_cast<FILE*>(png_get_io_ptr(png_ptr));
  if (fp != nullptr)
    fflush(fp);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
  FILE* fp = fopen("./dummy_file", "wb+");
  if (fp == nullptr)
    return 0;

  if (Size > 0)
    fwrite(Data, 1, Size, fp);
  fflush(fp);
  rewind(fp);

  png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                                                nullptr,
                                                PngErrorFn,
                                                PngWarningFn);
  if (png_ptr == nullptr)
  {
    fclose(fp);
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr)
  {
    png_destroy_write_struct(&png_ptr, nullptr);
    fclose(fp);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr)))
  {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
    return 0;
  }

  png_set_write_fn(png_ptr, fp, WriteDataFn, FlushFn);

  png_uint_32 feature_bits = 0;
  for (size_t i = 0; i < Size && i < sizeof(feature_bits); ++i)
    feature_bits = (feature_bits << 8) | Data[i];
  (void)png_permit_mng_features(png_ptr, feature_bits);
  (void)png_permit_mng_features(nullptr, feature_bits);

  int width = 1;
  int height = 1;
  if (Size >= 2)
  {
    width = 1 + (Data[0] % 32);
    height = 1 + (Data[1] % 32);
  }

  int color_type = PNG_COLOR_TYPE_RGB;
  int bit_depth = 8;
  if (Size >= 3)
  {
    switch (Data[2] % 5)
    {
      case 0: color_type = PNG_COLOR_TYPE_GRAY; break;
      case 1: color_type = PNG_COLOR_TYPE_GRAY_ALPHA; break;
      case 2: color_type = PNG_COLOR_TYPE_RGB; break;
      case 3: color_type = PNG_COLOR_TYPE_RGB_ALPHA; break;
      case 4: color_type = PNG_COLOR_TYPE_PALETTE; break;
    }
  }

  if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_PALETTE)
  {
    if (Size >= 4)
    {
      switch (Data[3] % 5)
      {
        case 0: bit_depth = 1; break;
        case 1: bit_depth = 2; break;
        case 2: bit_depth = 4; break;
        case 3: bit_depth = 8; break;
        case 4: bit_depth = 16; break;
      }
    }
    if (color_type == PNG_COLOR_TYPE_PALETTE && bit_depth == 16)
      bit_depth = 8;
  }
  else
  {
    bit_depth = (Size >= 4 && (Data[3] & 1)) ? 16 : 8;
  }

  png_set_IHDR(png_ptr, info_ptr,
               static_cast<png_uint_32>(width),
               static_cast<png_uint_32>(height),
               bit_depth,
               color_type,
               PNG_INTERLACE_NONE,
               PNG_COMPRESSION_TYPE_BASE,
               PNG_FILTER_TYPE_BASE);

  png_color palette[256];
  if (color_type == PNG_COLOR_TYPE_PALETTE)
  {
    int palette_entries = 1;
    if (Size >= 5)
      palette_entries = 1 + (Data[4] % 256);

    for (int i = 0; i < palette_entries; ++i)
    {
      size_t base = 5 + static_cast<size_t>(i) * 3;
      palette[i].red = (base < Size) ? Data[base] : static_cast<png_byte>(i);
      palette[i].green = (base + 1 < Size) ? Data[base + 1] : static_cast<png_byte>(i * 3);
      palette[i].blue = (base + 2 < Size) ? Data[base + 2] : static_cast<png_byte>(i * 7);
    }
    png_set_PLTE(png_ptr, info_ptr, palette, palette_entries);
  }

  const char* iccp_name = "fuzz";
  const png_bytep profile = reinterpret_cast<const png_bytep>(Data);
  png_uint_32 proflen = static_cast<png_uint_32>(Size > 4096 ? 4096 : Size);

  png_set_iCCP(png_ptr, info_ptr, iccp_name, PNG_COMPRESSION_TYPE_BASE, profile, proflen);

  char* out_name = nullptr;
  int compression_type = -1;
  png_byte* out_profile = nullptr;
  png_uint_32 out_proflen = 0;
  (void)png_get_iCCP(png_ptr, info_ptr, &out_name, &compression_type, &out_profile, &out_proflen);
  (void)png_get_iCCP(png_ptr, info_ptr, &out_name, nullptr, &out_profile, &out_proflen);

  png_write_sig(png_ptr);

  png_byte chunk_name[4] = {'f', 'U', 'Z', 'Z'};
  if (Size >= 9)
  {
    chunk_name[0] = Data[5];
    chunk_name[1] = Data[6];
    chunk_name[2] = Data[7];
    chunk_name[3] = Data[8];
  }
  png_uint_32 chunk_len = 0;
  if (Size >= 13)
  {
    chunk_len = (static_cast<png_uint_32>(Data[9]) << 24) |
                (static_cast<png_uint_32>(Data[10]) << 16) |
                (static_cast<png_uint_32>(Data[11]) << 8) |
                static_cast<png_uint_32>(Data[12]);
    chunk_len &= 0xFFFF;
  }
  png_write_chunk_start(png_ptr, chunk_name, chunk_len);

  png_write_info(png_ptr, info_ptr);

  png_destroy_write_struct(&png_ptr, &info_ptr);
  fclose(fp);
  return 0;
}