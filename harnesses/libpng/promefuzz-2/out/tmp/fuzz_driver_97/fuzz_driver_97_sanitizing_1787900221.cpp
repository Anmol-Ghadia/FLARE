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
  std::abort();
}

static void PngWarningFn(png_structp, png_const_charp)
{
}

static void WriteDataFn(png_structp png_ptr, png_bytep data, png_size_t length)
{
  png_voidp io_ptr = png_get_io_ptr(png_ptr);
  if (io_ptr == nullptr || data == nullptr)
    return;

  std::vector<unsigned char>* out =
      reinterpret_cast<std::vector<unsigned char>*>(io_ptr);
  out->insert(out->end(), data, data + length);
}

static void FlushDataFn(png_structp)
{
}

static png_uint_32 ReadU32(const uint8_t* p, size_t size, size_t off)
{
  if (off + 4 > size)
    return 0;
  return (static_cast<png_uint_32>(p[off]) << 24) |
         (static_cast<png_uint_32>(p[off + 1]) << 16) |
         (static_cast<png_uint_32>(p[off + 2]) << 8) |
         static_cast<png_uint_32>(p[off + 3]);
}

static int ReadInt(const uint8_t* p, size_t size, size_t off)
{
  return static_cast<int>(ReadU32(p, size, off));
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
  if (Data == nullptr)
    return 0;

  {
    uint8_t tmp[4] = {0, 0, 0, 0};
    if (Size >= 4)
      memcpy(tmp, Data, 4);
    png_int_32 v1 = png_get_int_32(tmp);
    uint8_t outbuf[4] = {0, 0, 0, 0};
    png_save_int_32(outbuf, v1);
    (void)png_get_int_32(outbuf);

    if (Size >= 8)
    {
      png_int_32 v2 = png_get_int_32(Data + (Size - 4));
      png_save_int_32(outbuf, v2);
      (void)png_get_int_32(outbuf);
    }
  }

  FILE* fp = std::fopen("./dummy_file", "wb");
  if (fp != nullptr)
  {
    if (Size > 0)
      std::fwrite(Data, 1, Size, fp);
    std::fclose(fp);
  }

  png_structp png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr,
                              PngErrorFn, PngWarningFn);
  if (png_ptr == nullptr)
    return 0;

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr)
  {
    png_destroy_write_struct(&png_ptr, nullptr);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr)))
  {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    return 0;
  }

  std::vector<unsigned char> output;
  png_set_write_fn(png_ptr, &output, WriteDataFn, FlushDataFn);

  png_uint_32 width = (Size > 0 ? (Data[0] % 32) + 1 : 1);
  png_uint_32 height = (Size > 1 ? (Data[1] % 32) + 1 : 1);
  int bit_depth = 8;
  int color_type = PNG_COLOR_TYPE_RGB;
  int interlace = (Size > 2 && (Data[2] & 1)) ? PNG_INTERLACE_ADAM7 : PNG_INTERLACE_NONE;
  int compression = PNG_COMPRESSION_TYPE_BASE;
  int filter = PNG_FILTER_TYPE_BASE;

  png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, color_type,
               interlace, compression, filter);

  png_int_32 offset_x = static_cast<png_int_32>(ReadU32(Data, Size, 0));
  png_int_32 offset_y = static_cast<png_int_32>(ReadU32(Data, Size, 4));
  int unit_type = (Size > 8) ? (Data[8] % 3) : 0;
  png_set_oFFs(png_ptr, info_ptr, offset_x, offset_y, unit_type);

  char purpose_buf[17];
  char units_buf[17];
  for (int i = 0; i < 16; ++i)
  {
    purpose_buf[i] = (Size > static_cast<size_t>(9 + i) && Data[9 + i] != 0)
                         ? static_cast<char>(Data[9 + i])
                         : 'A';
    units_buf[i] = (Size > static_cast<size_t>(25 + i) && Data[25 + i] != 0)
                       ? static_cast<char>(Data[25 + i])
                       : 'U';
  }
  purpose_buf[16] = '\0';
  units_buf[16] = '\0';

  char param_storage[4][16];
  char* params[4];
  for (int j = 0; j < 4; ++j)
  {
    for (int i = 0; i < 15; ++i)
    {
      size_t idx = 41 + j * 15 + i;
      param_storage[j][i] =
          (Size > idx && Data[idx] != 0) ? static_cast<char>(Data[idx]) : '0';
    }
    param_storage[j][15] = '\0';
    params[j] = param_storage[j];
  }

  int type = (Size > 101) ? (Data[101] % 6) : 0;
  int nparams = (Size > 102) ? (Data[102] % 5) : 0;
  png_int_32 x0 = static_cast<png_int_32>(ReadInt(Data, Size, 103));
  png_int_32 x1 = static_cast<png_int_32>(ReadInt(Data, Size, 107));

  if (type >= 0 && type <= 3)
    png_set_pCAL(png_ptr, info_ptr, purpose_buf, x0, x1, type, nparams,
                 units_buf, params);

  png_write_info(png_ptr, info_ptr);

  png_byte chunk_name[4] = {'f', 'U', 'Z', 'Z'};
  if (Size >= 4)
    memcpy(chunk_name, Data, 4);

  const png_bytep chunk_data =
      (Size > 4) ? reinterpret_cast<const png_bytep>(Data + 4) : nullptr;
  size_t chunk_length = (Size > 4) ? (Size - 4) : 0;
  if (chunk_length > 1024)
    chunk_length = 1024;

  png_write_chunk(png_ptr, chunk_name, chunk_data, chunk_length);

  png_byte empty_row[3] = {0, 0, 0};
  for (png_uint_32 y = 0; y < height; ++y)
    png_write_row(png_ptr, empty_row);

  png_write_end(png_ptr, info_ptr);
  png_destroy_write_struct(&png_ptr, &info_ptr);
  return 0;
}