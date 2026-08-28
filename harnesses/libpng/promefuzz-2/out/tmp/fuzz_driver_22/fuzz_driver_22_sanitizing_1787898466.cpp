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

static void FuzzPngError(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void FuzzPngWarning(png_structp, png_const_charp) {
}

static void ProgressiveInfoCallback(png_structp, png_infop) {
}

static void ProgressiveRowCallback(png_structp, png_bytep, png_uint_32, int) {
}

static void ProgressiveEndCallback(png_structp, png_infop) {
}

static void ReadDataFromFile(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead) {
  png_voidp io_ptr = png_get_io_ptr(png_ptr);
  if (io_ptr == nullptr) {
    png_error(png_ptr, "No IO ptr");
    return;
  }

  FILE* fp = static_cast<FILE*>(io_ptr);
  if (fp == nullptr) {
    png_error(png_ptr, "Invalid file ptr");
    return;
  }

  size_t n = fread(outBytes, 1, byteCountToRead, fp);
  if (n != byteCountToRead) {
    png_error(png_ptr, "Short read");
  }
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE* fp = fopen("./dummy_file", "wb");
  if (fp == nullptr) {
    return 0;
  }
  if (Size > 0) {
    fwrite(Data, 1, Size, fp);
  }
  fclose(fp);

  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr,
                                               FuzzPngError, FuzzPngWarning);
  if (png_ptr == nullptr) {
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  png_infop end_info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr || end_info_ptr == nullptr) {
    if (info_ptr) png_destroy_info_struct(png_ptr, &info_ptr);
    if (end_info_ptr) png_destroy_info_struct(png_ptr, &end_info_ptr);
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info_ptr);
    return 0;
  }

  png_set_progressive_read_fn(
      png_ptr,
      const_cast<uint8_t*>(Data),
      ProgressiveInfoCallback,
      ProgressiveRowCallback,
      ProgressiveEndCallback);

  int benign_allowed = (Size > 0) ? (Data[0] & 1) : 0;
  png_set_benign_errors(png_ptr, benign_allowed);

  int invalid_index_allowed = (Size > 1) ? static_cast<int>(Data[1]) : 0;
  png_set_check_for_invalid_index(png_ptr, invalid_index_allowed);

  FILE* rf = fopen("./dummy_file", "rb");
  if (rf == nullptr) {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info_ptr);
    return 0;
  }

  png_set_read_fn(png_ptr, rf, ReadDataFromFile);

  png_read_info(png_ptr, info_ptr);

  png_set_expand(png_ptr);

  if (Size > 2) {
    png_set_benign_errors(png_ptr, (Data[2] & 1));
  }
  if (Size > 3) {
    png_set_check_for_invalid_index(png_ptr, static_cast<int>(static_cast<int8_t>(Data[3])));
  }

  png_read_update_info(png_ptr, info_ptr);

  png_uint_32 width = png_get_image_width(png_ptr, info_ptr);
  png_uint_32 height = png_get_image_height(png_ptr, info_ptr);
  png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);

  const png_uint_32 kMaxDim = 4096;
  const png_size_t kMaxRowbytes = 1u << 20;
  const png_uint_32 kMaxRows = 4096;

  if (width == 0 || height == 0 || height > kMaxRows || width > kMaxDim || rowbytes == 0 ||
      rowbytes > kMaxRowbytes) {
    fclose(rf);
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info_ptr);
    return 0;
  }

  std::vector<png_byte> image_storage;
  std::vector<png_bytep> rows;

  size_t total_size = 0;
  if (rowbytes > 0 && height > 0) {
    if (rowbytes > static_cast<png_size_t>(SIZE_MAX / height)) {
      fclose(rf);
      png_destroy_read_struct(&png_ptr, &info_ptr, &end_info_ptr);
      return 0;
    }
    total_size = rowbytes * height;
  }

  if (total_size == 0 || total_size > (1u << 26)) {
    fclose(rf);
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info_ptr);
    return 0;
  }

  image_storage.resize(total_size);
  rows.resize(height);

  for (png_uint_32 y = 0; y < height; ++y) {
    rows[y] = image_storage.data() + static_cast<size_t>(y) * rowbytes;
  }

  png_read_image(png_ptr, rows.data());
  png_read_end(png_ptr, end_info_ptr);

  fclose(rf);
  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info_ptr);
  return 0;
}