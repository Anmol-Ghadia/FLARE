// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_get_user_width_max at pngget.c:1195:1 in png.h
// png_get_user_height_max at pngget.c:1201:1 in png.h
// png_set_chunk_cache_max at pngset.c:1652:1 in png.h
// png_set_user_limits at pngset.c:1636:1 in png.h
// png_get_user_width_max at pngget.c:1195:1 in png.h
// png_get_user_height_max at pngget.c:1201:1 in png.h
// png_set_read_fn at pngrio.c:87:1 in png.h
// png_read_info at pngread.c:92:1 in png.h
// png_get_IHDR at pngget.c:825:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_interlace_handling at pngtrans.c:99:1 in png.h
// png_read_update_info at pngread.c:267:1 in png.h
// png_get_rowbytes at pngget.c:30:1 in png.h
// png_get_image_height at pngget.c:61:1 in png.h
// png_read_image at pngread.c:700:1 in png.h
// png_read_end at pngread.c:766:1 in png.h
// png_get_rows at pngget.c:40:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_set_user_limits at pngset.c:1636:1 in png.h
// png_get_user_width_max at pngget.c:1195:1 in png.h
// png_get_user_height_max at pngget.c:1201:1 in png.h
// png_set_chunk_cache_max at pngset.c:1652:1 in png.h
// png_get_rows at pngget.c:40:1 in png.h
// png_read_image at pngread.c:700:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_user_limits at pngset.c:1636:1 in png.h
#include <png.h>
#include <cstdint>
#include <cstddef>
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

struct MemoryReader
{
  const uint8_t* data;
  size_t size;
  size_t offset;
};

static void ReadDataFn(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead)
{
  MemoryReader* reader = static_cast<MemoryReader*>(png_get_io_ptr(png_ptr));
  if (reader == nullptr || outBytes == nullptr)
    png_error(png_ptr, "invalid read state");

  if (reader->offset > reader->size || byteCountToRead > reader->size - reader->offset)
    png_error(png_ptr, "read overflow");

  std::memcpy(outBytes, reader->data + reader->offset, byteCountToRead);
  reader->offset += byteCountToRead;
}

extern "C" int LLVMFuzzerTestOneInput_12(const uint8_t *Data, size_t Size)
{
  FILE* fp = std::fopen("./dummy_file", "wb");
  if (fp != nullptr)
  {
    if (Size > 0)
      std::fwrite(Data, 1, Size, fp);
    std::fclose(fp);
  }

  png_set_user_limits(nullptr, 0, 0);
  (void)png_get_user_width_max(nullptr);
  (void)png_get_user_height_max(nullptr);
  png_set_chunk_cache_max(nullptr, 0);
  (void)png_get_rows(nullptr, nullptr);
  png_read_image(nullptr, nullptr);

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn, PngWarningFn);
  if (png_ptr == nullptr)
    return 0;

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr)
  {
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr)))
  {
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    return 0;
  }

  png_uint_32 wlim1 = 0x7fffffffU;
  png_uint_32 hlim1 = 0x7fffffffU;
  png_uint_32 cache1 = 0x7fffffffU;

  if (Size >= 4)
  {
    wlim1 = (static_cast<png_uint_32>(Data[0]) << 24) |
            (static_cast<png_uint_32>(Data[1]) << 16) |
            (static_cast<png_uint_32>(Data[2]) << 8) |
            static_cast<png_uint_32>(Data[3]);
  }
  if (Size >= 8)
  {
    hlim1 = (static_cast<png_uint_32>(Data[4]) << 24) |
            (static_cast<png_uint_32>(Data[5]) << 16) |
            (static_cast<png_uint_32>(Data[6]) << 8) |
            static_cast<png_uint_32>(Data[7]);
  }
  if (Size >= 12)
  {
    cache1 = (static_cast<png_uint_32>(Data[8]) << 24) |
             (static_cast<png_uint_32>(Data[9]) << 16) |
             (static_cast<png_uint_32>(Data[10]) << 8) |
             static_cast<png_uint_32>(Data[11]);
  }

  png_set_user_limits(png_ptr, wlim1, hlim1);
  (void)png_get_user_width_max(png_ptr);
  (void)png_get_user_height_max(png_ptr);
  png_set_chunk_cache_max(png_ptr, cache1);

  png_uint_32 wlim2 = (Size > 12) ? static_cast<png_uint_32>(Data[12]) : 0;
  png_uint_32 hlim2 = (Size > 13) ? static_cast<png_uint_32>(Data[13]) : 0;
  png_set_user_limits(png_ptr, wlim2, hlim2);
  (void)png_get_user_width_max(png_ptr);
  (void)png_get_user_height_max(png_ptr);

  MemoryReader reader{Data, Size, 0};
  png_set_read_fn(png_ptr, &reader, ReadDataFn);

  png_read_info(png_ptr, info_ptr);

  png_uint_32 width = 0, height = 0;
  int bit_depth = 0, color_type = 0, interlace_type = 0, compression_type = 0, filter_method = 0;
  if (!png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
                    &interlace_type, &compression_type, &filter_method))
  {
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    return 0;
  }

  if (interlace_type != PNG_INTERLACE_NONE)
    png_set_interlace_handling(png_ptr);

  png_read_update_info(png_ptr, info_ptr);

  png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);
  png_uint_32 rows = png_get_image_height(png_ptr, info_ptr);

  if (rows > 0 && rowbytes > 0 && rowbytes <= (1u << 20) && rows <= 4096)
  {
    std::vector<png_bytep> image(rows);
    std::vector<std::vector<png_byte>> storage(rows);

    for (png_uint_32 i = 0; i < rows; ++i)
    {
      storage[i].resize(rowbytes);
      image[i] = storage[i].data();
    }

    png_read_image(png_ptr, image.data());
  }

  png_read_end(png_ptr, info_ptr);

  png_bytepp rows_ptr = png_get_rows(png_ptr, info_ptr);
  if (rows_ptr != nullptr && rows > 0 && rows_ptr[0] != nullptr)
  {
    volatile png_byte sink = rows_ptr[0][0];
    (void)sink;
  }

  png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
  return 0;
}