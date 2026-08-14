#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <cstddef>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <png.h>

static void PngErrorFn(png_structp, png_const_charp) {
  std::longjmp(png_jmpbuf((png_structp)0), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE *fp = std::fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    if (Size > 0 && Data != nullptr) {
      std::fwrite(Data, 1, Size, fp);
    }
    std::fclose(fp);
  }

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (png_ptr == nullptr) {
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr) {
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    return 0;
  }

  fp = std::fopen("./dummy_file", "rb");
  if (fp != nullptr) {
    png_init_io(png_ptr, fp);

    if (Size > 0) {
      png_read_info(png_ptr, info_ptr);

      (void)png_get_bit_depth(png_ptr, info_ptr);
      (void)png_get_interlace_type(png_ptr, info_ptr);
      (void)png_get_filter_type(png_ptr, info_ptr);

      png_uint_32 width = 0;
      png_uint_32 height = 0;
      int bit_depth = 0;
      int color_type = 0;
      int interlace_method = 0;
      int compression_method = 0;
      int filter_method = 0;

      (void)png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth,
                         &color_type, &interlace_method, &compression_method,
                         &filter_method);

      (void)png_get_IHDR(png_ptr, info_ptr, nullptr, &height, nullptr,
                         &color_type, nullptr, &compression_method, nullptr);
      (void)png_get_IHDR(png_ptr, info_ptr, &width, nullptr, &bit_depth,
                         nullptr, &interlace_method, nullptr, &filter_method);

      png_uint_32 flags[] = {
          0,
          PNG_INFO_sBIT,
          PNG_INFO_IHDR,
          0xffffffffu,
          static_cast<png_uint_32>(Size),
          (Size >= sizeof(png_uint_32))
              ? *reinterpret_cast<const png_uint_32 *>(Data)
              : 0u,
      };

      for (png_uint_32 flag : flags) {
        (void)png_get_valid(png_ptr, info_ptr, flag);
      }

      png_color_8p sig_bit = nullptr;
      png_color_8 sig_bit_storage;
      std::memset(&sig_bit_storage, 0, sizeof(sig_bit_storage));

      (void)png_get_sBIT(png_ptr, info_ptr, &sig_bit);
      sig_bit = &sig_bit_storage;
      (void)png_get_sBIT(png_ptr, info_ptr, &sig_bit);
    }

    std::fclose(fp);
  }

  (void)png_get_bit_depth(nullptr, info_ptr);
  (void)png_get_bit_depth(png_ptr, nullptr);
  (void)png_get_interlace_type(nullptr, info_ptr);
  (void)png_get_interlace_type(png_ptr, nullptr);
  (void)png_get_filter_type(nullptr, info_ptr);
  (void)png_get_filter_type(png_ptr, nullptr);
  (void)png_get_valid(nullptr, info_ptr, PNG_INFO_sBIT);
  (void)png_get_valid(png_ptr, nullptr, PNG_INFO_sBIT);

  png_uint_32 width2 = 0;
  png_uint_32 height2 = 0;
  int bit_depth2 = 0;
  int color_type2 = 0;
  int interlace_method2 = 0;
  int compression_method2 = 0;
  int filter_method2 = 0;

  (void)png_get_IHDR(nullptr, info_ptr, &width2, &height2, &bit_depth2,
                     &color_type2, &interlace_method2, &compression_method2,
                     &filter_method2);
  (void)png_get_IHDR(png_ptr, nullptr, &width2, &height2, &bit_depth2,
                     &color_type2, &interlace_method2, &compression_method2,
                     &filter_method2);

  png_color_8p sig_bit2 = nullptr;
  (void)png_get_sBIT(nullptr, info_ptr, &sig_bit2);
  (void)png_get_sBIT(png_ptr, nullptr, &sig_bit2);
  (void)png_get_sBIT(png_ptr, info_ptr, nullptr);

  png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
  return 0;
}