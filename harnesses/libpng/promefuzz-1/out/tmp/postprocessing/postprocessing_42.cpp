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
#include <cstring>
#include <vector>

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

static void ReadDataFn(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead) {
  FILE* fp = static_cast<FILE*>(png_get_io_ptr(png_ptr));
  if (fp == nullptr || outBytes == nullptr) {
    png_error(png_ptr, "invalid read state");
    return;
  }

  if (byteCountToRead == 0) return;

  size_t n = fread(outBytes, 1, byteCountToRead, fp);
  if (n != byteCountToRead) {
    png_error(png_ptr, "short read");
  }
}

static void ConsumeInfoQueries(png_structp png_ptr, png_infop info_ptr) {
  png_unknown_chunkp unknowns = nullptr;
  int unknown_count = png_get_unknown_chunks(png_ptr, info_ptr, &unknowns);
  if (unknown_count > 0 && unknowns != nullptr) {
    volatile png_byte b = 0;
    for (int i = 0; i < unknown_count; ++i) {
      b ^= unknowns[i].name[0];
      if (unknowns[i].size > 0 && unknowns[i].data != nullptr) {
        b ^= unknowns[i].data[0];
      }
      b ^= unknowns[i].location;
    }
    (void)b;
  } else {
    png_unknown_chunkp null_unknowns = nullptr;
    (void)png_get_unknown_chunks(nullptr, info_ptr, &null_unknowns);
    (void)png_get_unknown_chunks(png_ptr, nullptr, &null_unknowns);
  }

  png_textp text_ptr = nullptr;
  int num_text = -1;
  int text_count = png_get_text(png_ptr, info_ptr, &text_ptr, &num_text);
  if (text_count > 0 && text_ptr != nullptr) {
    volatile size_t acc = 0;
    for (int i = 0; i < text_count; ++i) {
      if (text_ptr[i].key != nullptr) acc ^= static_cast<unsigned char>(text_ptr[i].key[0]);
      acc ^= text_ptr[i].text_length;
      acc ^= static_cast<size_t>(text_ptr[i].compression);
    }
    (void)acc;
  } else {
    int dummy_num = -1;
    png_textp dummy_text = nullptr;
    (void)png_get_text(nullptr, info_ptr, &dummy_text, &dummy_num);
    (void)png_get_text(png_ptr, nullptr, &dummy_text, &dummy_num);
  }
}

static void FuzzReadPath(const uint8_t* Data, size_t Size, int mode) {
  FILE* fp = std::fopen("./dummy_file", "wb");
  if (fp == nullptr) return;
  if (Size > 0) {
    (void)std::fwrite(Data, 1, Size, fp);
  }
  std::fclose(fp);

  fp = std::fopen("./dummy_file", "rb");
  if (fp == nullptr) return;

  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn, PngWarningFn);
  if (png_ptr == nullptr) {
    std::fclose(fp);
    return;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr) {
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    std::fclose(fp);
    return;
  }

  png_infop end_info_ptr = png_create_info_struct(png_ptr);

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, end_info_ptr ? &end_info_ptr : nullptr);
    std::fclose(fp);
    return;
  }

  png_set_read_fn(png_ptr, fp, ReadDataFn);

  if (mode & 1) png_set_expand(png_ptr);
  if (mode & 2) png_set_gray_to_rgb(png_ptr);
  if (mode & 4) png_set_strip_16(png_ptr);
  if (mode & 8) png_set_packing(png_ptr);
  if (mode & 16) png_set_tRNS_to_alpha(png_ptr);

  switch (mode % 3) {
    case 0: {
      png_read_info(png_ptr, info_ptr);
      ConsumeInfoQueries(png_ptr, info_ptr);
      png_read_update_info(png_ptr, info_ptr);
      ConsumeInfoQueries(png_ptr, info_ptr);
      png_read_end(png_ptr, end_info_ptr ? end_info_ptr : info_ptr);
      if (end_info_ptr) ConsumeInfoQueries(png_ptr, end_info_ptr);
      break;
    }

    case 1: {
      png_read_info(png_ptr, info_ptr);
      ConsumeInfoQueries(png_ptr, info_ptr);
      png_read_update_info(png_ptr, info_ptr);

      png_uint_32 width = png_get_image_width(png_ptr, info_ptr);
      png_uint_32 height = png_get_image_height(png_ptr, info_ptr);
      png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);

      if (height > 0 && rowbytes > 0 && height < 4096 && rowbytes < (1u << 20)) {
        std::vector<png_byte> image;
        std::vector<png_bytep> rows;
        if (rowbytes <= SIZE_MAX / height) {
          image.resize(static_cast<size_t>(rowbytes) * static_cast<size_t>(height));
          rows.resize(static_cast<size_t>(height));
          for (png_uint_32 y = 0; y < height; ++y) {
            rows[y] = image.data() + static_cast<size_t>(y) * rowbytes;
          }
          png_read_image(png_ptr, rows.data());
        }
      } else {
        if (width == 0 || height == 0) {
          // no-op
        }
      }

      png_read_end(png_ptr, end_info_ptr ? end_info_ptr : info_ptr);
      if (end_info_ptr) ConsumeInfoQueries(png_ptr, end_info_ptr);
      break;
    }

    default: {
      int transforms = 0;
      if (mode & 1) transforms |= PNG_TRANSFORM_EXPAND;
      if (mode & 2) transforms |= PNG_TRANSFORM_GRAY_TO_RGB;
      if (mode & 4) transforms |= PNG_TRANSFORM_STRIP_16;
      if (mode & 8) transforms |= PNG_TRANSFORM_PACKING;
      if (mode & 16) transforms |= PNG_TRANSFORM_STRIP_ALPHA;
      png_read_png(png_ptr, info_ptr, transforms, nullptr);
      ConsumeInfoQueries(png_ptr, info_ptr);
      png_read_end(png_ptr, end_info_ptr ? end_info_ptr : info_ptr);
      if (end_info_ptr) ConsumeInfoQueries(png_ptr, end_info_ptr);
      break;
    }
  }

  png_destroy_read_struct(&png_ptr, &info_ptr, end_info_ptr ? &end_info_ptr : nullptr);
  std::fclose(fp);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FuzzReadPath(Data, Size, 0);
  FuzzReadPath(Data, Size, Size > 0 ? Data[0] : 1);
  FuzzReadPath(Data, Size, Size > 1 ? Data[1] : 2);
  return 0;
}