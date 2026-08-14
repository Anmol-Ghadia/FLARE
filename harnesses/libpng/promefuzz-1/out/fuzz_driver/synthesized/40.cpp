// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_expand at pngrtran.c:872:1 in png.h
// png_set_gray_to_rgb at pngrtran.c:955:1 in png.h
// png_set_add_alpha at pngtrans.c:205:1 in png.h
// png_read_info at pngread.c:92:1 in png.h
// png_set_expand at pngrtran.c:872:1 in png.h
// png_set_gray_to_rgb at pngrtran.c:955:1 in png.h
// png_set_add_alpha at pngtrans.c:205:1 in png.h
// png_set_interlace_handling at pngtrans.c:99:1 in png.h
// png_read_update_info at pngread.c:267:1 in png.h
// png_get_image_width at pngget.c:52:1 in png.h
// png_get_image_height at pngget.c:61:1 in png.h
// png_get_rowbytes at pngget.c:30:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_read_row at pngread.c:383:1 in png.h
// png_get_current_row_number at pngtrans.c:844:1 in png.h
// png_read_end at pngread.c:766:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_get_current_row_number at pngtrans.c:844:1 in png.h
// png_get_current_row_number at pngtrans.c:844:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_read_fn at pngrio.c:87:1 in png.h
// png_set_read_user_transform_fn at pngrtran.c:1073:1 in png.h
// png_set_user_transform_info at pngtrans.c:802:1 in png.h
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
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <vector>
#include <algorithm>

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

static void UserReadFn(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead) {
  png_voidp io_ptr = png_get_io_ptr(png_ptr);
  if (io_ptr == nullptr) {
    png_error(png_ptr, "no io_ptr");
    return;
  }

  struct BufferState {
    const uint8_t* data;
    size_t size;
    size_t offset;
  };

  BufferState* st = static_cast<BufferState*>(io_ptr);
  if (st->offset > st->size || byteCountToRead > st->size - st->offset) {
    png_error(png_ptr, "read overflow");
    return;
  }

  for (png_size_t i = 0; i < byteCountToRead; ++i)
    outBytes[i] = st->data[st->offset + i];
  st->offset += byteCountToRead;
}

static void UserTransformFn(png_structp png_ptr, png_row_infop, png_bytep) {
  (void)png_get_current_row_number(png_ptr);
}

extern "C" int LLVMFuzzerTestOneInput_40(const uint8_t *Data, size_t Size) {
  FILE* fp = fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    if (Size > 0) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  (void)png_get_current_row_number(nullptr);

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn, PngWarningFn);
  if (png_ptr == nullptr)
    return 0;

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr) {
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    return 0;
  }

  struct BufferState {
    const uint8_t* data;
    size_t size;
    size_t offset;
  } state = {Data, Size, 0};

  png_set_read_fn(png_ptr, &state, UserReadFn);

  png_set_read_user_transform_fn(png_ptr, UserTransformFn);

  void* user_ctx = nullptr;
  if (Size >= sizeof(uintptr_t)) {
    uintptr_t v = 0;
    size_t n = std::min(Size, sizeof(uintptr_t));
    for (size_t i = 0; i < n; ++i)
      v = (v << 8) ^ Data[i];
    user_ctx = reinterpret_cast<void*>(v);
  }

  int depth = Size > 0 ? static_cast<int>(Data[0]) : 8;
  int channels = Size > 1 ? static_cast<int>(Data[1]) : 3;
  png_set_user_transform_info(png_ptr, user_ctx, depth, channels);

  if (Size > 2 && (Data[2] & 1))
    png_set_expand(png_ptr);

  if (Size > 2 && (Data[2] & 2))
    png_set_gray_to_rgb(png_ptr);

  png_uint_32 filler = 0;
  if (Size >= 7) {
    filler = (static_cast<png_uint_32>(Data[3]) << 24) |
             (static_cast<png_uint_32>(Data[4]) << 16) |
             (static_cast<png_uint_32>(Data[5]) << 8) |
             static_cast<png_uint_32>(Data[6]);
  } else if (Size > 3) {
    for (size_t i = 3; i < Size; ++i)
      filler = (filler << 8) ^ Data[i];
  }

  int flags = (Size > 7 && (Data[7] & 1)) ? PNG_FILLER_AFTER : PNG_FILLER_BEFORE;
  png_set_add_alpha(png_ptr, filler, flags);

  png_read_info(png_ptr, info_ptr);

  if (Size > 8 && (Data[8] & 1))
    png_set_expand(png_ptr);
  if (Size > 8 && (Data[8] & 2))
    png_set_gray_to_rgb(png_ptr);
  if (Size > 8 && (Data[8] & 4))
    png_set_add_alpha(png_ptr, filler ^ 0xFFFFFFFFu, flags ^ 1);

  int passes = png_set_interlace_handling(png_ptr);
  if (passes < 1)
    passes = 1;

  png_read_update_info(png_ptr, info_ptr);

  png_uint_32 width = png_get_image_width(png_ptr, info_ptr);
  png_uint_32 height = png_get_image_height(png_ptr, info_ptr);
  png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);

  if (rowbytes == 0 || height == 0 || width == 0) {
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    return 0;
  }

  if (rowbytes > (1u << 20) || height > (1u << 16)) {
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    return 0;
  }

  std::vector<png_byte> row(rowbytes);

  for (int pass = 0; pass < passes; ++pass) {
    for (png_uint_32 y = 0; y < height; ++y) {
      png_read_row(png_ptr, row.data(), nullptr);
      (void)png_get_current_row_number(png_ptr);
    }
  }

  png_read_end(png_ptr, info_ptr);
  png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
  return 0;
}