#include <cstdio>
#include <cstdint>
#include <cstddef>
#include <csetjmp>

#include <png.h>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
  FILE *fp = std::fopen("./dummy_file", "wb");
  if (fp != nullptr)
  {
    if (Size > 0)
      std::fwrite(Data, 1, Size, fp);
    std::fclose(fp);
  }

  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
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

  png_byte compression_type = png_get_compression_type(png_ptr, info_ptr);
  png_byte bit_depth = png_get_bit_depth(png_ptr, info_ptr);
  png_uint_32 image_height = png_get_image_height(png_ptr, info_ptr);
  png_byte interlace_type = png_get_interlace_type(png_ptr, info_ptr);

  (void)compression_type;
  (void)bit_depth;
  (void)image_height;
  (void)interlace_type;

  png_uint_32 width = 0;
  png_uint_32 height = 0;
  int ihdr_bit_depth = 0;
  int color_type = 0;
  int interlace_method = 0;
  int compression_method = 0;
  int filter_method = 0;

  (void)png_get_IHDR(png_ptr, info_ptr, &width, &height, &ihdr_bit_depth, &color_type,
                     &interlace_method, &compression_method, &filter_method);
  (void)png_get_IHDR(png_ptr, info_ptr, &width, nullptr, &ihdr_bit_depth, nullptr,
                     &interlace_method, nullptr, &filter_method);
  (void)png_get_IHDR(png_ptr, info_ptr, nullptr, &height, nullptr, &color_type,
                     nullptr, &compression_method, nullptr);
  (void)png_get_IHDR(png_ptr, info_ptr, nullptr, nullptr, nullptr, nullptr,
                     nullptr, nullptr, nullptr);

  png_color_8p sig_bit = nullptr;
  (void)png_get_sBIT(png_ptr, info_ptr, &sig_bit);

  (void)png_get_compression_type(nullptr, info_ptr);
  (void)png_get_compression_type(png_ptr, nullptr);
  (void)png_get_compression_type(nullptr, nullptr);

  (void)png_get_bit_depth(nullptr, info_ptr);
  (void)png_get_bit_depth(png_ptr, nullptr);
  (void)png_get_bit_depth(nullptr, nullptr);

  (void)png_get_image_height(nullptr, info_ptr);
  (void)png_get_image_height(png_ptr, nullptr);
  (void)png_get_image_height(nullptr, nullptr);

  (void)png_get_interlace_type(nullptr, info_ptr);
  (void)png_get_interlace_type(png_ptr, nullptr);
  (void)png_get_interlace_type(nullptr, nullptr);

  (void)png_get_IHDR(nullptr, info_ptr, &width, &height, &ihdr_bit_depth, &color_type,
                     &interlace_method, &compression_method, &filter_method);
  (void)png_get_IHDR(png_ptr, nullptr, &width, &height, &ihdr_bit_depth, &color_type,
                     &interlace_method, &compression_method, &filter_method);
  (void)png_get_IHDR(nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
                     nullptr, nullptr, nullptr);

  (void)png_get_sBIT(nullptr, info_ptr, &sig_bit);
  (void)png_get_sBIT(png_ptr, nullptr, &sig_bit);
  (void)png_get_sBIT(png_ptr, info_ptr, nullptr);
  (void)png_get_sBIT(nullptr, nullptr, nullptr);

  png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
  return 0;
}