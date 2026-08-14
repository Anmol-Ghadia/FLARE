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
#include <vector>

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE *fp = std::fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    if (Size > 0) {
      std::fwrite(Data, 1, Size, fp);
    }
    std::fclose(fp);
  }

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn, PngWarningFn);
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

  FILE *rfp = std::fopen("./dummy_file", "rb");
  if (rfp == nullptr) {
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    return 0;
  }

  png_init_io(png_ptr, rfp);

  png_read_info(png_ptr, info_ptr);

  png_byte color_type = png_get_color_type(png_ptr, info_ptr);
  png_byte bit_depth = png_get_bit_depth(png_ptr, info_ptr);

  if ((color_type & PNG_COLOR_MASK_COLOR) == 0) {
    png_set_gray_to_rgb(png_ptr);
  }

  if (bit_depth < 8) {
    png_set_packing(png_ptr);
    png_set_packswap(png_ptr);
  }

  if (bit_depth == 16) {
    png_set_swap(png_ptr);
  }

  png_set_bgr(png_ptr);

  png_read_update_info(png_ptr, info_ptr);

  png_uint_32 width = png_get_image_width(png_ptr, info_ptr);
  png_uint_32 height = png_get_image_height(png_ptr, info_ptr);
  png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);

  if (width > 0 && height > 0 && rowbytes > 0 && height < 1000000) {
    std::vector<png_byte> image;
    std::vector<png_bytep> rows;

    if (rowbytes <= (static_cast<size_t>(-1) / static_cast<size_t>(height))) {
      image.resize(static_cast<size_t>(rowbytes) * static_cast<size_t>(height));
      rows.resize(static_cast<size_t>(height));
      for (png_uint_32 y = 0; y < height; ++y) {
        rows[y] = image.data() + static_cast<size_t>(y) * rowbytes;
      }
      png_read_image(png_ptr, rows.data());
    } else {
      for (png_uint_32 y = 0; y < height; ++y) {
        std::vector<png_byte> row(rowbytes);
        png_read_row(png_ptr, row.data(), nullptr);
      }
    }
  }

  png_read_end(png_ptr, info_ptr);

  std::fclose(rfp);
  png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
  return 0;
}