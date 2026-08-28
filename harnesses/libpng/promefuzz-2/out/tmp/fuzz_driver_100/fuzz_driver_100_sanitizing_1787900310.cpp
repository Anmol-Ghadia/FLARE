#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <png.h>

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
  if (io_ptr == nullptr || data == nullptr || length == 0)
    return;

  std::vector<unsigned char>* out =
      reinterpret_cast<std::vector<unsigned char>*>(io_ptr);
  out->insert(out->end(), data, data + length);
}

static void FlushFn(png_structp)
{
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
  if (Data == nullptr)
    return 0;

  FILE* fp = std::fopen("./dummy_file", "wb");
  if (fp != nullptr)
  {
    if (Size > 0)
      std::fwrite(Data, 1, Size, fp);
    std::fclose(fp);
  }

  png_byte int_buf[4] = {0, 0, 0, 0};
  if (Size >= 4)
    std::memcpy(int_buf, Data, 4);
  else if (Size > 0)
    std::memcpy(int_buf, Data, Size);

  png_int_32 parsed = png_get_int_32(int_buf);

  png_byte saved_buf[4] = {0, 0, 0, 0};
  png_save_int_32(saved_buf, parsed);
  png_int_32 reparsed = png_get_int_32(saved_buf);

  png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr,
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

  std::vector<unsigned char> out;
  png_set_write_fn(png_ptr, &out, WriteDataFn, FlushFn);

  int width = 1 + (Size > 4 ? (Data[4] % 8) : 0);
  int height = 1 + (Size > 5 ? (Data[5] % 8) : 0);
  int color_type = PNG_COLOR_TYPE_RGB;
  int bit_depth = 8;
  int interlace = (Size > 6 && (Data[6] & 1)) ? PNG_INTERLACE_ADAM7 : PNG_INTERLACE_NONE;

  png_set_IHDR(png_ptr, info_ptr,
               static_cast<png_uint_32>(width),
               static_cast<png_uint_32>(height),
               bit_depth, color_type, interlace,
               PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

  png_int_32 off_x = parsed;
  png_int_32 off_y = reparsed;
  int unit_type = (Size > 7) ? (Data[7] % 3) : 0;
  png_set_oFFs(png_ptr, info_ptr, off_x, off_y, unit_type);

  char purpose[17];
  char units[17];
  std::memset(purpose, 0, sizeof(purpose));
  std::memset(units, 0, sizeof(units));

  size_t purpose_len = Size > 8 ? ((Data[8] % 15) + 1) : 1;
  size_t units_len = Size > 9 ? ((Data[9] % 15) + 1) : 1;

  for (size_t i = 0; i < purpose_len; ++i)
    purpose[i] = static_cast<char>('A' + ((Size > 10 + i ? Data[10 + i] : i) % 26));
  purpose[purpose_len] = '\0';

  for (size_t i = 0; i < units_len; ++i)
    units[i] = static_cast<char>('a' + ((Size > 26 + i ? Data[26 + i] : i) % 26));
  units[units_len] = '\0';

  char param_storage[4][16];
  char* params[4];
  for (int i = 0; i < 4; ++i)
  {
    std::memset(param_storage[i], 0, sizeof(param_storage[i]));
    for (int j = 0; j < 15; ++j)
    {
      size_t idx = 42 + static_cast<size_t>(i) * 15 + static_cast<size_t>(j);
      unsigned char v = (idx < Size) ? Data[idx] : static_cast<unsigned char>(j + i);
      param_storage[i][j] = static_cast<char>('0' + (v % 10));
    }
    param_storage[i][15] = '\0';
    params[i] = param_storage[i];
  }

  int pcal_type = (Size > 3) ? (Data[3] % 6) : 0;
  int nparams = (Size > 2) ? (Data[2] % 5) : 0;
  png_set_pCAL(png_ptr, info_ptr, purpose, off_x, off_y,
               pcal_type, nparams, units, params);

  png_write_info(png_ptr, info_ptr);

  png_byte chunk_name[4] = {'f', 'U', 'Z', 'Z'};
  if (Size >= 4)
    std::memcpy(chunk_name, Data, 4);

  png_const_bytep chunk_data =
      (Size > 4) ? reinterpret_cast<png_const_bytep>(Data + 4) : nullptr;
  size_t chunk_len = (Size > 4) ? (Size - 4) : 0;
  png_write_chunk(png_ptr, chunk_name, chunk_data, chunk_len);

  std::vector<png_byte> row(static_cast<size_t>(width) * 3);
  size_t pos = 0;
  for (int y = 0; y < height; ++y)
  {
    for (size_t x = 0; x < row.size(); ++x)
    {
      row[x] = (Size > 0) ? Data[pos % Size] : 0;
      ++pos;
    }
    png_write_row(png_ptr, row.data());
  }

  png_write_end(png_ptr, info_ptr);
  png_destroy_write_struct(&png_ptr, &info_ptr);
  return 0;
}