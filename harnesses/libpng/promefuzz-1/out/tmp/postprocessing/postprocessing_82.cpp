#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstddef>
#include <vector>
#include <algorithm>
#include <setjmp.h>
#include <png.h>

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

static void ProgressiveInfoCallback(png_structp, png_infop) {
}

static void ProgressiveEndCallback(png_structp, png_infop) {
}

static void ProgressiveRowCallback(png_structp png_ptr, png_bytep new_row,
                                   png_uint_32, int) {
  png_infop info_ptr =
      static_cast<png_infop>(png_get_progressive_ptr(png_ptr));

  png_uint_32 width = 0, height = 0;
  int bit_depth = 0, color_type = 0, interlace_type = 0;
  int compression_type = 0, filter_method = 0;

  if (info_ptr != nullptr) {
    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
                 &interlace_type, &compression_type, &filter_method);
  }

  png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);
  if (rowbytes == 0 && width > 0) rowbytes = width * 8;

  if (rowbytes > 0 && rowbytes < (1u << 20)) {
    std::vector<png_byte> old_row(rowbytes);
    png_progressive_combine_row(png_ptr, old_row.data(), new_row);
    png_progressive_combine_row(png_ptr, old_row.data(), nullptr);
  } else {
    png_progressive_combine_row(png_ptr, nullptr, new_row);
    png_progressive_combine_row(png_ptr, nullptr, nullptr);
  }
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  png_progressive_combine_row(nullptr, nullptr, nullptr);
  png_read_image(nullptr, nullptr);
  png_read_update_info(nullptr, nullptr);
  png_set_expand(nullptr);
  (void)png_set_interlace_handling(nullptr);
  png_read_row(nullptr, nullptr, nullptr);

  FILE *fp = std::fopen("./dummy_file", "wb");
  if (!fp) return 0;
  if (Size > 0) std::fwrite(Data, 1, Size, fp);
  std::fclose(fp);

  fp = std::fopen("./dummy_file", "rb");
  if (!fp) return 0;

  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr,
                                               PngErrorFn, PngWarningFn);
  if (!png_ptr) {
    std::fclose(fp);
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    std::fclose(fp);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    std::fclose(fp);
    return 0;
  }

  png_set_progressive_read_fn(png_ptr, info_ptr,
                              ProgressiveInfoCallback,
                              ProgressiveRowCallback,
                              ProgressiveEndCallback);

  size_t offset = 0;
  while (offset < Size) {
    size_t chunk = 1 + (Data[offset] % 32);
    if (chunk > Size - offset) chunk = Size - offset;
    png_process_data(png_ptr, info_ptr,
                     const_cast<png_bytep>(Data + offset),
                     static_cast<png_size_t>(chunk));
    offset += chunk;
  }

  std::fclose(fp);

  fp = std::fopen("./dummy_file", "rb");
  if (!fp) {
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    return 0;
  }

  png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);

  png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr,
                                   PngErrorFn, PngWarningFn);
  if (!png_ptr) {
    std::fclose(fp);
    return 0;
  }

  info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    std::fclose(fp);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    std::fclose(fp);
    return 0;
  }

  png_init_io(png_ptr, fp);
  png_read_info(png_ptr, info_ptr);

  if (Size > 0 && (Data[0] & 1)) png_set_expand(png_ptr);
  if (Size > 1 && (Data[1] & 1)) png_set_expand(png_ptr);

  int passes = 1;
  if (Size > 2 && (Data[2] & 1)) {
    passes = png_set_interlace_handling(png_ptr);
  } else if (Size > 3 && (Data[3] & 1)) {
    passes = png_set_interlace_handling(png_ptr);
  }

  png_read_update_info(png_ptr, info_ptr);

  png_uint_32 width = 0, height = 0;
  int bit_depth = 0, color_type = 0, interlace_type = 0;
  int compression_type = 0, filter_method = 0;
  png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
               &interlace_type, &compression_type, &filter_method);

  png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);
  if (rowbytes > (1u << 20) || height > (1u << 16)) {
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    std::fclose(fp);
    return 0;
  }

  if (Size > 4 && (Data[4] & 1)) {
    std::vector<std::vector<png_byte>> rows_storage(height);
    std::vector<png_bytep> row_ptrs(height);

    for (png_uint_32 y = 0; y < height; ++y) {
      rows_storage[y].resize(rowbytes);
      row_ptrs[y] = rows_storage[y].data();
    }

    png_read_image(png_ptr, row_ptrs.empty() ? nullptr : row_ptrs.data());
  } else {
    std::vector<png_byte> row(rowbytes ? rowbytes : 1);
    std::vector<png_byte> display_row(rowbytes ? rowbytes : 1);

    png_uint_32 effective_height = std::min<png_uint_32>(height, 4096);
    int effective_passes = std::max(1, std::min(passes, 7));

    for (int p = 0; p < effective_passes; ++p) {
      for (png_uint_32 y = 0; y < effective_height; ++y) {
        if (Size > 5 && (Data[5] & 1)) {
          png_read_row(png_ptr, row.data(), display_row.data());
        } else if (Size > 6 && (Data[6] & 1)) {
          png_read_row(png_ptr, row.data(), nullptr);
        } else {
          png_read_row(png_ptr, nullptr, nullptr);
        }
      }
    }
  }

  png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
  std::fclose(fp);
  return 0;
}