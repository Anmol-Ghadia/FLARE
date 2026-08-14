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
#include <setjmp.h>
#include <stdio.h>
#include <string.h>

static void DummyWarning(png_structp, png_const_charp) {}

static void DummyError(png_structp png_ptr, png_const_charp)
{
  longjmp(png_jmpbuf(png_ptr), 1);
}

static png_uint_32 ReadU32(const uint8_t *data, size_t size, size_t offset)
{
  if (offset + 4 > size) return 0;
  return ((png_uint_32)data[offset] << 24) |
         ((png_uint_32)data[offset + 1] << 16) |
         ((png_uint_32)data[offset + 2] << 8) |
         ((png_uint_32)data[offset + 3]);
}

static void BuildMessage(const uint8_t *data, size_t size, char *out, size_t out_size)
{
  if (out_size == 0) return;
  size_t n = size < (out_size - 1) ? size : (out_size - 1);
  for (size_t i = 0; i < n; ++i)
  {
    uint8_t c = data[i];
    out[i] = (c >= 32 && c <= 126) ? (char)c : 'A';
  }
  out[n] = '\0';
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
  FILE *fp = fopen("./dummy_file", "wb");
  if (fp != NULL)
  {
    if (Size > 0) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, DummyError, DummyWarning);
  if (png_ptr == NULL)
    return 0;

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == NULL)
  {
    png_destroy_read_struct(&png_ptr, NULL, NULL);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr)) != 0)
  {
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    return 0;
  }

  char msg1[256];
  char msg2[256];
  char msg3[256];
  BuildMessage(Data, Size, msg1, sizeof(msg1));
  BuildMessage(Data + (Size > 8 ? 8 : 0), Size > 8 ? Size - 8 : 0, msg2, sizeof(msg2));
  BuildMessage(Data + (Size > 16 ? 16 : 0), Size > 16 ? Size - 16 : 0, msg3, sizeof(msg3));

  if (Size > 0)
  {
    png_uint_32 chunk_name = ReadU32(Data, Size, 0);
    png_ptr->chunk_name = chunk_name;
  }

  if (Size > 4)
  {
    if (Data[4] & 1)
      png_ptr->flags |= PNG_FLAG_BENIGN_ERRORS_WARN;
    else
      png_ptr->flags &= ~PNG_FLAG_BENIGN_ERRORS_WARN;
  }

  switch (Size > 5 ? (Data[5] % 8) : 0)
  {
    case 0:
      png_warning(png_ptr, msg1);
      break;

    case 1:
      png_chunk_warning(png_ptr, msg2);
      break;

    case 2:
      png_benign_error(png_ptr, msg3);
      break;

    case 3:
      png_chunk_benign_error(png_ptr, msg1);
      break;

    case 4:
      (void)png_process_data_skip(png_ptr);
      break;

    case 5:
      png_warning(png_ptr, "#1234 stripped-prefix warning");
      png_chunk_warning(png_ptr, "#9999 stripped-prefix chunk warning");
      break;

    case 6:
      (void)png_process_data_skip(png_ptr);
      png_benign_error(png_ptr, msg2);
      png_chunk_warning(png_ptr, msg3);
      break;

    case 7:
      png_chunk_error(png_ptr, msg1);
      break;
  }

  if (Size > 6)
  {
    if (Data[6] & 1)
      png_warning(png_ptr, msg2);
    else
      png_chunk_warning(png_ptr, msg3);
  }

  png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
  return 0;
}