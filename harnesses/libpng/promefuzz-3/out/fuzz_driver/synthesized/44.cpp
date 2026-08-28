// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_get_filter_type at pngget.c:88:1 in png.h
// png_get_channels at pngget.c:467:1 in png.h
// png_get_interlace_type at pngget.c:97:1 in png.h
// png_get_bit_depth at pngget.c:70:1 in png.h
// png_get_compression_type at pngget.c:106:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_get_color_type at pngget.c:79:1 in png.h
// png_get_filter_type at pngget.c:88:1 in png.h
// png_get_channels at pngget.c:467:1 in png.h
// png_get_interlace_type at pngget.c:97:1 in png.h
// png_get_bit_depth at pngget.c:70:1 in png.h
// png_get_compression_type at pngget.c:106:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_read_fn at pngrio.c:87:1 in png.h
// png_sig_cmp at png.c:74:1 in png.h
// png_set_sig_bytes at png.c:47:1 in png.h
// png_read_info at pngread.c:92:1 in png.h
// png_get_color_type at pngget.c:79:1 in png.h
// png_get_filter_type at pngget.c:88:1 in png.h
// png_get_channels at pngget.c:467:1 in png.h
// png_get_interlace_type at pngget.c:97:1 in png.h
// png_get_bit_depth at pngget.c:70:1 in png.h
// png_get_compression_type at pngget.c:106:1 in png.h
// png_read_update_info at pngread.c:267:1 in png.h
// png_get_color_type at pngget.c:79:1 in png.h
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

extern "C" int LLVMFuzzerTestOneInput_44(const uint8_t *Data, size_t Size)
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