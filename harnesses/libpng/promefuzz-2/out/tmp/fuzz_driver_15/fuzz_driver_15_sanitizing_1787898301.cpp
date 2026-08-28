#include <png.h>

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

static void PngErrorFn(png_structp png_ptr, png_const_charp)
{
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp)
{
}

static void WriteDataFn(png_structp png_ptr, png_bytep data, png_size_t length)
{
  FILE* fp = static_cast<FILE*>(png_get_io_ptr(png_ptr));
  if (fp != nullptr && data != nullptr && length > 0)
    (void)fwrite(data, 1, length, fp);
}

static void FlushDataFn(png_structp png_ptr)
{
  FILE* fp = static_cast<FILE*>(png_get_io_ptr(png_ptr));
  if (fp != nullptr)
    fflush(fp);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* Data, size_t Size)
{
  if (Data == nullptr)
    return 0;

  png_byte int_buf[4] = {0, 0, 0, 0};
  if (Size >= 4)
    memcpy(int_buf, Data, 4);
  else if (Size > 0)
    memcpy(int_buf, Data, Size);

  png_int_32 parsed = png_get_int_32(int_buf);

  png_byte saved_buf[4] = {0, 0, 0, 0};
  png_save_int_32(saved_buf, parsed);

  png_structp png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn, PngWarningFn);
  if (png_ptr == nullptr)
    return 0;

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr)
  {
    png_destroy_write_struct(&png_ptr, nullptr);
    return 0;
  }

  FILE* fp = nullptr;

  if (setjmp(png_jmpbuf(png_ptr)))
  {
    if (fp != nullptr)
      fclose(fp);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    return 0;
  }

  fp = fopen("./dummy_file", "wb");
  if (fp == nullptr)
  {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    return 0;
  }

  png_set_write_fn(png_ptr, fp, WriteDataFn, FlushDataFn);

  int width = 1 + (Size > 4 ? (Data[4] % 8) : 0);
  int height = 1 + (Size > 5 ? (Data[5] % 8) : 0);
  int color_type = PNG_COLOR_TYPE_RGB;
  int bit_depth = 8;
  int interlace = (Size > 6 && (Data[6] & 1)) ? PNG_INTERLACE_ADAM7 : PNG_INTERLACE_NONE;

  png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, color_type,
               interlace, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

  png_int_32 off_x = parsed;
  png_int_32 off_y = -parsed;
  if (Size >= 8)
  {
    png_byte tmp[4];
    memcpy(tmp, Data + 4, 4);
    off_y = png_get_int_32(tmp);
  }

  int unit_type = (Size > 8) ? (Data[8] % 3) : 0;
  png_set_oFFs(png_ptr, info_ptr, off_x, off_y, unit_type);

  const char* purpose = "fuzz";
  const char* units = "unit";
  std::vector<char> purpose_storage;
  std::vector<char> units_storage;

  if (Size > 9)
  {
    size_t p_len = Data[9] % 16;
    purpose_storage.resize(p_len + 1);
    for (size_t i = 0; i < p_len; ++i)
      purpose_storage[i] = static_cast<char>('A' + (Data[(10 + i) % Size] % 26));
    purpose_storage[p_len] = '\0';
    purpose = purpose_storage.data();
  }

  if (Size > 10)
  {
    size_t u_len = Data[10] % 16;
    units_storage.resize(u_len + 1);
    for (size_t i = 0; i < u_len; ++i)
      units_storage[i] = static_cast<char>('a' + (Data[(11 + i) % Size] % 26));
    units_storage[u_len] = '\0';
    units = units_storage.data();
  }

  int pcal_type = (Size > 11) ? (Data[11] % 4) : 0;

  int nparams = 0;
  std::vector<std::vector<char>> param_storage;
  std::vector<char*> params;

  if (Size > 12)
  {
    nparams = Data[12] % 4;
    if (nparams > 0)
    {
      param_storage.resize(static_cast<size_t>(nparams));
      params.resize(static_cast<size_t>(nparams));
      for (int i = 0; i < nparams; ++i)
      {
        size_t len = 1 + (Size > 0 ? (Data[(13 + static_cast<size_t>(i)) % Size] % 8) : 0);
        param_storage[static_cast<size_t>(i)].resize(len + 1);
        for (size_t j = 0; j < len; ++j)
        {
          param_storage[static_cast<size_t>(i)][j] =
              static_cast<char>('0' + (Data[(13 + static_cast<size_t>(i) + j) % Size] % 10));
        }
        param_storage[static_cast<size_t>(i)][len] = '\0';
        params[static_cast<size_t>(i)] = param_storage[static_cast<size_t>(i)].data();
      }
    }
  }

  if (nparams > 0)
  {
    png_set_pCAL(png_ptr, info_ptr, purpose, parsed, off_y, pcal_type, nparams, units,
                 params.data());
  }

  png_write_info(png_ptr, info_ptr);

  png_byte chunk_name[4] = {'f', 'U', 'Z', 'z'};
  if (Size >= 4)
    memcpy(chunk_name, Data, 4);

  const png_bytep chunk_data =
      Size > 0 ? reinterpret_cast<const png_byte*>(Data) : nullptr;
  size_t chunk_len = Size;
  if (chunk_len > 1024)
    chunk_len = 1024;

  png_write_chunk(png_ptr, chunk_name, chunk_data, chunk_len);
  png_write_chunk(png_ptr, reinterpret_cast<png_const_bytep>("tEXt"),
                  saved_buf, sizeof(saved_buf));

  fclose(fp);
  png_destroy_write_struct(&png_ptr, &info_ptr);
  return 0;
}