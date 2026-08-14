// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_read_user_transform_fn at pngrtran.c:1073:1 in png.h
// png_set_read_user_transform_fn at pngrtran.c:1073:1 in png.h
// png_set_read_status_fn at pngread.c:1037:1 in png.h
// png_set_read_status_fn at pngread.c:1037:1 in png.h
// png_set_read_fn at pngrio.c:87:1 in png.h
// png_sig_cmp at png.c:74:1 in png.h
// png_set_sig_bytes at png.c:47:1 in png.h
// png_read_info at pngread.c:92:1 in png.h
// png_get_image_width at pngget.c:52:1 in png.h
// png_get_image_height at pngget.c:61:1 in png.h
// png_get_bit_depth at pngget.c:70:1 in png.h
// png_get_color_type at pngget.c:79:1 in png.h
// png_set_strip_16 at pngrtran.c:197:1 in png.h
// png_set_palette_to_rgb at pngrtran.c:902:1 in png.h
// png_set_add_alpha at pngtrans.c:205:1 in png.h
// png_set_expand_gray_1_2_4_to_8 at pngrtran.c:914:1 in png.h
// png_read_update_info at pngread.c:267:1 in png.h
// png_get_rowbytes at pngget.c:30:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_read_image at pngread.c:700:1 in png.h
// png_read_end at pngread.c:766:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_progressive_read_fn at pngpread.c:1074:1 in png.h
// png_set_read_user_transform_fn at pngrtran.c:1073:1 in png.h
// png_set_read_status_fn at pngread.c:1037:1 in png.h
// png_process_data at pngpread.c:36:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_write_user_transform_fn at pngwrite.c:1322:1 in png.h
// png_set_write_user_transform_fn at pngwrite.c:1322:1 in png.h
// png_set_write_status_fn at pngwrite.c:1312:1 in png.h
// png_set_write_status_fn at pngwrite.c:1312:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_error at pngerror.c:38:1 in png.h
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
#include <cstring>
#include <vector>
#include <algorithm>

struct FuzzInput {
  const uint8_t* data;
  size_t size;
  size_t offset;
};

static void ReadDataFn(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead) {
  FuzzInput* in = static_cast<FuzzInput*>(png_get_io_ptr(png_ptr));
  if (in == nullptr || outBytes == nullptr) {
    png_error(png_ptr, "invalid read context");
    return;
  }

  if (in->offset + byteCountToRead > in->size) {
    size_t remain = (in->offset < in->size) ? (in->size - in->offset) : 0;
    if (remain > 0) {
      std::memcpy(outBytes, in->data + in->offset, remain);
      std::memset(outBytes + remain, 0, byteCountToRead - remain);
      in->offset += remain;
    } else {
      std::memset(outBytes, 0, byteCountToRead);
    }
    png_error(png_ptr, "read beyond end");
    return;
  }

  std::memcpy(outBytes, in->data + in->offset, byteCountToRead);
  in->offset += byteCountToRead;
}

static void ReadUserTransformFn(png_structp png_ptr, png_row_infop row_info, png_bytep data) {
  (void)png_ptr;
  if (row_info == nullptr || data == nullptr)
    return;

  png_uint_32 limit = row_info->rowbytes;
  if (limit > 16) limit = 16;
  for (png_uint_32 i = 0; i < limit; ++i)
    data[i] ^= static_cast<png_byte>(i);
}

static void WriteUserTransformFn(png_structp png_ptr, png_row_infop row_info, png_bytep data) {
  (void)png_ptr;
  if (row_info == nullptr || data == nullptr)
    return;

  png_uint_32 limit = row_info->rowbytes;
  if (limit > 16) limit = 16;
  for (png_uint_32 i = 0; i < limit; ++i)
    data[i] = static_cast<png_byte>(data[i] + 1);
}

static void ReadStatusFn(png_structp png_ptr, png_uint_32 row_num, int pass) {
  (void)png_ptr;
  (void)row_num;
  (void)pass;
}

static void WriteStatusFn(png_structp png_ptr, png_uint_32 row_num, int pass) {
  (void)png_ptr;
  (void)row_num;
  (void)pass;
}

static void ProgressiveInfoFn(png_structp png_ptr, png_infop info_ptr) {
  (void)png_ptr;
  (void)info_ptr;
}

static void ProgressiveRowFn(png_structp png_ptr, png_bytep new_row, png_uint_32 row_num, int pass) {
  (void)png_ptr;
  (void)row_num;
  (void)pass;
  if (new_row != nullptr) {
    volatile png_byte sink = new_row[0];
    (void)sink;
  }
}

static void ProgressiveEndFn(png_structp png_ptr, png_infop info_ptr) {
  (void)png_ptr;
  (void)info_ptr;
}

static void RunReadPath(const uint8_t* Data, size_t Size, uint8_t mode) {
  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (png_ptr == nullptr)
    return;

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr) {
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    return;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    return;
  }

  FuzzInput in{Data, Size, 0};

  if (mode & 1)
    png_set_read_user_transform_fn(png_ptr, ReadUserTransformFn);
  else
    png_set_read_user_transform_fn(png_ptr, nullptr);

  if (mode & 2)
    png_set_read_status_fn(png_ptr, ReadStatusFn);
  else
    png_set_read_status_fn(png_ptr, nullptr);

  png_set_read_fn(png_ptr, &in, ReadDataFn);

  if (Size >= 8 && (mode & 4) == 0) {
    png_byte sig[8];
    png_size_t n = static_cast<png_size_t>(std::min<size_t>(8, Size));
    std::memcpy(sig, Data, n);
    if (n == 8 && png_sig_cmp(sig, 0, 8) == 0) {
      png_set_sig_bytes(png_ptr, 0);
    }
  }

  png_read_info(png_ptr, info_ptr);

  png_uint_32 width = png_get_image_width(png_ptr, info_ptr);
  png_uint_32 height = png_get_image_height(png_ptr, info_ptr);
  int bit_depth = png_get_bit_depth(png_ptr, info_ptr);
  int color_type = png_get_color_type(png_ptr, info_ptr);

  if (bit_depth == 16 && (mode & 8))
    png_set_strip_16(png_ptr);
  if (color_type == PNG_COLOR_TYPE_PALETTE && (mode & 16))
    png_set_palette_to_rgb(png_ptr);
  if ((color_type & PNG_COLOR_MASK_ALPHA) == 0 && (mode & 32))
    png_set_add_alpha(png_ptr, 0xff, PNG_FILLER_AFTER);
  if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8 && (mode & 64))
    png_set_expand_gray_1_2_4_to_8(png_ptr);

  png_read_update_info(png_ptr, info_ptr);

  png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);
  if (rowbytes == 0 || width == 0 || height == 0 || rowbytes > (1U << 20) || height > 4096) {
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    return;
  }

  std::vector<png_byte> image;
  if (rowbytes > 0 && height > 0) {
    image.resize(rowbytes * height);
    std::vector<png_bytep> rows(height);
    for (png_uint_32 y = 0; y < height; ++y)
      rows[y] = image.data() + y * rowbytes;
    png_read_image(png_ptr, rows.data());
  }

  png_read_end(png_ptr, info_ptr);
  png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
}

static void RunProgressivePath(const uint8_t* Data, size_t Size, uint8_t mode) {
  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (png_ptr == nullptr)
    return;

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr) {
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    return;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    return;
  }

  FuzzInput in{Data, Size, 0};

  png_set_progressive_read_fn(
      png_ptr,
      &in,
      (mode & 1) ? ProgressiveInfoFn : nullptr,
      (mode & 2) ? ProgressiveRowFn : nullptr,
      (mode & 4) ? ProgressiveEndFn : nullptr);

  if (mode & 8)
    png_set_read_user_transform_fn(png_ptr, ReadUserTransformFn);
  if (mode & 16)
    png_set_read_status_fn(png_ptr, ReadStatusFn);

  size_t pos = 0;
  while (pos < Size) {
    size_t chunk = 1 + ((mode + pos) % 32);
    if (chunk > Size - pos)
      chunk = Size - pos;
    png_process_data(png_ptr, info_ptr, const_cast<png_bytep>(Data + pos), static_cast<png_size_t>(chunk));
    pos += chunk;
  }

  png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
}

static void RunWriteConfigPath(uint8_t mode) {
  png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (png_ptr == nullptr)
    return;

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr) {
    png_destroy_write_struct(&png_ptr, nullptr);
    return;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    return;
  }

  if (mode & 1)
    png_set_write_user_transform_fn(png_ptr, WriteUserTransformFn);
  else
    png_set_write_user_transform_fn(png_ptr, nullptr);

  if (mode & 2)
    png_set_write_status_fn(png_ptr, WriteStatusFn);
  else
    png_set_write_status_fn(png_ptr, nullptr);

  png_destroy_write_struct(&png_ptr, &info_ptr);
}

extern "C" int LLVMFuzzerTestOneInput_80(const uint8_t *Data, size_t Size) {
  uint8_t mode = Size ? Data[0] : 0;

  RunWriteConfigPath(mode);

  if (Size > 1)
    RunReadPath(Data + 1, Size - 1, mode ^ 0x5a);
  else
    RunReadPath(Data, Size, mode ^ 0x5a);

  RunProgressivePath(Data, Size, mode ^ 0xa5);

  return 0;
}