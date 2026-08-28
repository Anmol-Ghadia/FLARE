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
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE *fp = std::fopen("./dummy_file", "wb");
  if (fp == nullptr) {
    return 0;
  }

  if (Size > 0) {
    std::fwrite(Data, 1, Size, fp);
  }
  std::fclose(fp);

  fp = std::fopen("./dummy_file", "rb");
  if (fp == nullptr) {
    return 0;
  }

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn, PngWarningFn);
  if (png_ptr == nullptr) {
    std::fclose(fp);
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr) {
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    std::fclose(fp);
    return 0;
  }

  png_infop end_info = png_create_info_struct(png_ptr);
  if (end_info == nullptr) {
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    std::fclose(fp);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    std::fclose(fp);
    return 0;
  }

  png_init_io(png_ptr, fp);

  png_read_info(png_ptr, info_ptr);

  if (Size > 0) {
    if (Data[0] & 0x01) png_set_expand(png_ptr);
    if (Data[0] & 0x02) png_set_gray_to_rgb(png_ptr);
    if (Data[0] & 0x04) png_set_packing(png_ptr);
  } else {
    png_set_expand(png_ptr);
    png_set_gray_to_rgb(png_ptr);
    png_set_packing(png_ptr);
  }

  int passes = 1;
  if (Size > 1 && (Data[1] & 0x01)) {
    passes = png_set_interlace_handling(png_ptr);
  }

  png_read_update_info(png_ptr, info_ptr);

  png_uint_32 width = png_get_image_width(png_ptr, info_ptr);
  png_uint_32 height = png_get_image_height(png_ptr, info_ptr);
  png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);

  if (rowbytes == 0) {
    rowbytes = width;
  }

  const png_uint_32 max_rows = 4096;
  const png_size_t max_rowbytes = 1 << 20;

  if (height > max_rows) height = max_rows;
  if (rowbytes > max_rowbytes) rowbytes = max_rowbytes;

  std::vector<png_byte> row(rowbytes ? rowbytes : 1);
  std::vector<png_byte> display_row(rowbytes ? rowbytes : 1);

  for (int pass = 0; pass < std::max(1, passes); ++pass) {
    png_uint_32 y = 0;
    while (y < height) {
      bool use_rows = false;
      bool use_display = false;
      png_uint_32 batch = 1;

      if (Size > 2) {
        use_rows = (Data[(2 + y) % Size] & 0x01) != 0;
        use_display = (Data[(2 + y) % Size] & 0x02) != 0;
        batch = static_cast<png_uint_32>((Data[(2 + y) % Size] % 4) + 1);
      }

      batch = std::min(batch, height - y);

      if (use_rows) {
        std::vector<std::vector<png_byte>> rows_storage(batch, std::vector<png_byte>(rowbytes ? rowbytes : 1));
        std::vector<std::vector<png_byte>> disp_storage;
        std::vector<png_bytep> row_ptrs(batch);
        std::vector<png_bytep> disp_ptrs;

        for (png_uint_32 i = 0; i < batch; ++i) {
          row_ptrs[i] = rows_storage[i].data();
        }

        png_bytepp disp_arg = nullptr;
        if (use_display) {
          disp_storage.assign(batch, std::vector<png_byte>(rowbytes ? rowbytes : 1));
          disp_ptrs.resize(batch);
          for (png_uint_32 i = 0; i < batch; ++i) {
            disp_ptrs[i] = disp_storage[i].data();
          }
          disp_arg = disp_ptrs.data();
        }

        png_read_rows(png_ptr, row_ptrs.data(), disp_arg, batch);
        y += batch;
      } else {
        png_bytep disp = use_display ? display_row.data() : nullptr;
        png_read_row(png_ptr, row.data(), disp);
        ++y;
      }
    }
  }

  png_read_end(png_ptr, end_info);

  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
  std::fclose(fp);
  return 0;
}