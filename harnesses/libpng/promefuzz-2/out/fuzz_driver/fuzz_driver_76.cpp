// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_write_fn at pngwio.c:122:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_set_IHDR at pngset.c:254:1 in png.h
// png_set_PLTE at pngset.c:572:1 in png.h
// png_set_oFFs at pngset.c:295:1 in png.h
// png_set_pCAL at pngset.c:312:1 in png.h
// png_write_info at pngwrite.c:192:1 in png.h
// png_write_chunk at pngwutil.c:192:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_write_row at pngwrite.c:693:1 in png.h
// png_write_end at pngwrite.c:358:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_save_int_32 at png.c:726:1 in png.h
// png_save_int_32 at png.c:726:1 in png.h
// png_save_int_32 at png.c:726:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <vector>
#include <png.h>

static void PngErrorFn(png_structp png_ptr, png_const_charp)
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
    (void)fwrite(data, 1, length, fp);
}

static void FlushFn(png_structp png_ptr)
{
  FILE* fp = reinterpret_cast<FILE*>(png_get_io_ptr(png_ptr));
  if (fp != nullptr)
    (void)fflush(fp);
}

static uint32_t ReadU32(const uint8_t* data, size_t size, size_t offset)
{
  if (offset + 4 > size)
    return 0;
  return (static_cast<uint32_t>(data[offset]) << 24) |
         (static_cast<uint32_t>(data[offset + 1]) << 16) |
         (static_cast<uint32_t>(data[offset + 2]) << 8) |
         static_cast<uint32_t>(data[offset + 3]);
}

static png_int_32 ReadI32ViaLibpng(const uint8_t* data, size_t size, size_t offset)
{
  if (offset + 4 > size)
    return 0;
  return png_get_int_32(reinterpret_cast<png_const_bytep>(data + offset));
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
  png_byte tmpbuf[4] = {0, 0, 0, 0};

  if (Size >= 4)
  {
    png_int_32 v0 = png_get_int_32(reinterpret_cast<png_const_bytep>(Data));
    png_save_int_32(tmpbuf, v0);
    (void)png_get_int_32(tmpbuf);
  }

  if (Size >= 8)
  {
    png_int_32 v1 = png_get_int_32(reinterpret_cast<png_const_bytep>(Data + Size - 4));
    png_save_int_32(tmpbuf, v1);
    (void)png_get_int_32(tmpbuf);
  }

  if (Size >= 12)
  {
    size_t mid = (Size / 2) - 2;
    png_int_32 v2 = png_get_int_32(reinterpret_cast<png_const_bytep>(Data + mid));
    png_save_int_32(tmpbuf, v2);
    (void)png_get_int_32(tmpbuf);
  }

  FILE* fp = std::fopen("./dummy_file", "wb+");
  if (fp == nullptr)
    return 0;

  png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr,
                                                PngErrorFn, PngWarningFn);
  if (png_ptr == nullptr)
  {
    std::fclose(fp);
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr)
  {
    png_destroy_write_struct(&png_ptr, nullptr);
    std::fclose(fp);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr)))
  {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    std::fclose(fp);
    return 0;
  }

  png_set_write_fn(png_ptr, fp, WriteDataFn, FlushFn);

  uint32_t width_raw = ReadU32(Data, Size, 0);
  uint32_t height_raw = ReadU32(Data, Size, 4);
  png_uint_32 width = static_cast<png_uint_32>((width_raw % 8) + 1);
  png_uint_32 height = static_cast<png_uint_32>((height_raw % 8) + 1);

  int color_type_choices[] = {
      PNG_COLOR_TYPE_GRAY,
      PNG_COLOR_TYPE_RGB,
      PNG_COLOR_TYPE_PALETTE,
      PNG_COLOR_TYPE_GRAY_ALPHA,
      PNG_COLOR_TYPE_RGB_ALPHA
  };
  int bit_depth_choices[] = {1, 2, 4, 8, 16};

  int color_sel = (Size > 8) ? (Data[8] % 5) : 0;
  int color_type = color_type_choices[color_sel];
  int bit_sel = (Size > 9) ? (Data[9] % 5) : 3;
  int bit_depth = bit_depth_choices[bit_sel];

  if ((color_type == PNG_COLOR_TYPE_RGB ||
       color_type == PNG_COLOR_TYPE_RGB_ALPHA ||
       color_type == PNG_COLOR_TYPE_GRAY_ALPHA) &&
      bit_depth < 8)
  {
    bit_depth = 8;
  }

  if (color_type == PNG_COLOR_TYPE_PALETTE && bit_depth == 16)
    bit_depth = 8;

  png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, color_type,
               PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

  if (color_type == PNG_COLOR_TYPE_PALETTE)
  {
    png_color palette[256];
    for (int i = 0; i < 256; ++i)
    {
      size_t base = 10 + static_cast<size_t>(i) * 3;
      palette[i].red = (base < Size) ? Data[base] : static_cast<png_byte>(i);
      palette[i].green = (base + 1 < Size) ? Data[base + 1] : static_cast<png_byte>(255 - i);
      palette[i].blue = (base + 2 < Size) ? Data[base + 2] : static_cast<png_byte>(i ^ 0xAA);
    }
    int num_palette = (Size > 20) ? ((Data[10] % 16) + 1) : 16;
    png_set_PLTE(png_ptr, info_ptr, palette, num_palette);
  }

  png_int_32 offset_x = ReadI32ViaLibpng(Data, Size, 12);
  png_int_32 offset_y = ReadI32ViaLibpng(Data, Size, 16);
  int unit_type = (Size > 20) ? (Data[20] % 2) : 0;
  png_set_oFFs(png_ptr, info_ptr, offset_x, offset_y, unit_type);

  char purpose_buf[17];
  char units_buf[17];
  std::memset(purpose_buf, 0, sizeof(purpose_buf));
  std::memset(units_buf, 0, sizeof(units_buf));

  size_t purpose_len = 0;
  size_t units_len = 0;
  for (size_t i = 0; i < 16; ++i)
  {
    if (21 + i < Size)
      purpose_buf[purpose_len++] = static_cast<char>('A' + (Data[21 + i] % 26));
    if (37 + i < Size)
      units_buf[units_len++] = static_cast<char>('a' + (Data[37 + i] % 26));
  }
  if (purpose_len == 0)
    std::memcpy(purpose_buf, "p", 2);
  if (units_len == 0)
    std::memcpy(units_buf, "u", 2);

  int pcal_type = (Size > 53) ? static_cast<int>(Data[53] % 4) : 0;
  int nparams = 0;
  char** params_ptr = nullptr;
  png_set_pCAL(png_ptr, info_ptr, purpose_buf, ReadI32ViaLibpng(Data, Size, 0),
               ReadI32ViaLibpng(Data, Size, 4), pcal_type, nparams, units_buf,
               params_ptr);

  png_write_info(png_ptr, info_ptr);

  png_byte chunk_name[4] = {'f', 'U', 'Z', 'Z'};
  if (Size >= 4)
  {
    chunk_name[0] = Data[0] ? Data[0] : 'f';
    chunk_name[1] = Data[1] ? Data[1] : 'U';
    chunk_name[2] = Data[2] ? Data[2] : 'Z';
    chunk_name[3] = Data[3] ? Data[3] : 'Z';
  }

  size_t chunk_len = 0;
  png_const_bytep chunk_data = nullptr;
  if (Size > 64)
  {
    chunk_len = (Size - 64 > 32) ? 32 : (Size - 64);
    chunk_data = reinterpret_cast<png_const_bytep>(Data + 64);
  }
  png_write_chunk(png_ptr, chunk_name, chunk_data, chunk_len);

  int channels = 1;
  switch (color_type)
  {
    case PNG_COLOR_TYPE_GRAY: channels = 1; break;
    case PNG_COLOR_TYPE_PALETTE: channels = 1; break;
    case PNG_COLOR_TYPE_RGB: channels = 3; break;
    case PNG_COLOR_TYPE_GRAY_ALPHA: channels = 2; break;
    case PNG_COLOR_TYPE_RGB_ALPHA: channels = 4; break;
    default: channels = 1; break;
  }

  size_t bytes_per_sample = (bit_depth == 16) ? 2u : 1u;
  size_t rowbytes;
  if ((color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_PALETTE) &&
      bit_depth < 8)
  {
    rowbytes = ((static_cast<size_t>(width) * static_cast<size_t>(bit_depth)) + 7u) / 8u;
  }
  else
  {
    rowbytes = static_cast<size_t>(width) * static_cast<size_t>(channels) * bytes_per_sample;
  }

  std::vector<png_byte> row(rowbytes ? rowbytes : 1u, 0);
  size_t data_cursor = 0;
  for (png_uint_32 y = 0; y < height; ++y)
  {
    for (size_t i = 0; i < row.size(); ++i)
    {
      row[i] = (data_cursor < Size) ? Data[data_cursor] : static_cast<uint8_t>(i + y);
      ++data_cursor;
    }
    png_write_row(png_ptr, row.data());
  }

  png_write_end(png_ptr, info_ptr);
  png_destroy_write_struct(&png_ptr, &info_ptr);
  std::fclose(fp);
  return 0;
}