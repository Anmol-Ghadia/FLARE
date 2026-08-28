#include <png.h>
#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <setjmp.h>

struct BufferState
{
  const uint8_t* data;
  size_t size;
  size_t offset;
};

static void PngErrorFn(png_structp png_ptr, png_const_charp)
{
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp)
{
}

static void ReadDataFn(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead)
{
  png_voidp io_ptr = png_get_io_ptr(png_ptr);
  if (io_ptr == NULL)
    png_error(png_ptr, "No IO pointer");

  BufferState* st = reinterpret_cast<BufferState*>(io_ptr);
  if (st->offset > st->size || byteCountToRead > st->size - st->offset)
    png_error(png_ptr, "Read overflow");

  memcpy(outBytes, st->data + st->offset, byteCountToRead);
  st->offset += byteCountToRead;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
  FILE* fp = fopen("./dummy_file", "wb");
  if (fp != NULL)
  {
    fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  (void)png_get_color_type(NULL, NULL);
  (void)png_get_filter_type(NULL, NULL);
  (void)png_get_channels(NULL, NULL);
  (void)png_get_interlace_type(NULL, NULL);
  (void)png_get_bit_depth(NULL, NULL);
  (void)png_get_compression_type(NULL, NULL);

  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL,
                                               PngErrorFn, PngWarningFn);
  if (png_ptr == NULL)
    return 0;

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == NULL)
  {
    png_destroy_read_struct(&png_ptr, NULL, NULL);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr)))
  {
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    return 0;
  }

  BufferState state = {Data, Size, 0};
  png_set_read_fn(png_ptr, &state, ReadDataFn);

  if (Size >= 8 && png_sig_cmp(const_cast<png_bytep>(Data), 0, 8) == 0)
  {
    png_set_sig_bytes(png_ptr, 8);
    state.offset = 8;
  }

  png_read_info(png_ptr, info_ptr);

  volatile png_byte color_type = png_get_color_type(png_ptr, info_ptr);
  volatile png_byte filter_type = png_get_filter_type(png_ptr, info_ptr);
  volatile png_byte channels = png_get_channels(png_ptr, info_ptr);
  volatile png_byte interlace_type = png_get_interlace_type(png_ptr, info_ptr);
  volatile png_byte bit_depth = png_get_bit_depth(png_ptr, info_ptr);
  volatile png_byte compression_type = png_get_compression_type(png_ptr, info_ptr);

  (void)color_type;
  (void)filter_type;
  (void)channels;
  (void)interlace_type;
  (void)bit_depth;
  (void)compression_type;

  png_read_update_info(png_ptr, info_ptr);

  volatile png_byte color_type2 = png_get_color_type(png_ptr, info_ptr);
  volatile png_byte filter_type2 = png_get_filter_type(png_ptr, info_ptr);
  volatile png_byte channels2 = png_get_channels(png_ptr, info_ptr);
  volatile png_byte interlace_type2 = png_get_interlace_type(png_ptr, info_ptr);
  volatile png_byte bit_depth2 = png_get_bit_depth(png_ptr, info_ptr);
  volatile png_byte compression_type2 = png_get_compression_type(png_ptr, info_ptr);

  (void)color_type2;
  (void)filter_type2;
  (void)channels2;
  (void)interlace_type2;
  (void)bit_depth2;
  (void)compression_type2;

  png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
  return 0;
}