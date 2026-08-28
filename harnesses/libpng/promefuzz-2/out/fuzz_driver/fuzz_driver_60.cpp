// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_get_pixel_aspect_ratio at pngget.c:181:1 in png.h
// png_get_iCCP at pngget.c:736:1 in png.h
// png_get_y_offset_pixels at pngget.c:296:1 in png.h
// png_get_y_offset_pixels at pngget.c:296:1 in png.h
// png_get_interlace_type at pngget.c:97:1 in png.h
// png_get_interlace_type at pngget.c:97:1 in png.h
// png_get_image_width at pngget.c:52:1 in png.h
// png_get_image_width at pngget.c:52:1 in png.h
// png_get_pixel_aspect_ratio at pngget.c:181:1 in png.h
// png_get_pixel_aspect_ratio at pngget.c:181:1 in png.h
// png_get_iCCP at pngget.c:736:1 in png.h
// png_get_iCCP at pngget.c:736:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_init_io at png.c:703:1 in png.h
// png_set_read_fn at pngrio.c:87:1 in png.h
// png_set_sig_bytes at png.c:47:1 in png.h
// png_set_sig_bytes at png.c:47:1 in png.h
// png_read_info at pngread.c:92:1 in png.h
// png_get_y_offset_pixels at pngget.c:296:1 in png.h
// png_get_interlace_type at pngget.c:97:1 in png.h
// png_get_image_width at pngget.c:52:1 in png.h
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

struct MemReader {
  const uint8_t* data;
  size_t size;
  size_t offset;
};

static void ReadFromMemory(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead) {
  MemReader* reader = static_cast<MemReader*>(png_get_io_ptr(png_ptr));
  if (reader == nullptr || outBytes == nullptr) {
    png_error(png_ptr, "invalid reader");
    return;
  }

  if (byteCountToRead > reader->size - reader->offset) {
    png_error(png_ptr, "read overflow");
    return;
  }

  memcpy(outBytes, reader->data + reader->offset, byteCountToRead);
  reader->offset += byteCountToRead;
}

static void ExerciseLibpng(const uint8_t* data, size_t size, bool use_file_io) {
  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn, PngWarningFn);
  if (png_ptr == nullptr) return;

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr) {
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    return;
  }

  png_infop end_info = png_create_info_struct(png_ptr);
  if (end_info == nullptr) {
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    return;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    return;
  }

  FILE* fp = nullptr;
  MemReader reader{data, size, 0};

  if (use_file_io) {
    fp = fopen("./dummy_file", "wb");
    if (fp != nullptr) {
      fwrite(data, 1, size, fp);
      fclose(fp);
      fp = nullptr;
    }

    fp = fopen("./dummy_file", "rb");
    if (fp == nullptr) {
      png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
      return;
    }
    png_init_io(png_ptr, fp);
  } else {
    png_set_read_fn(png_ptr, &reader, ReadFromMemory);
  }

  if (size > 8) {
    png_set_sig_bytes(png_ptr, static_cast<int>(data[0] & 7));
  } else {
    png_set_sig_bytes(png_ptr, 0);
  }

  png_read_info(png_ptr, info_ptr);

  (void)png_get_y_offset_pixels(png_ptr, info_ptr);
  (void)png_get_interlace_type(png_ptr, info_ptr);
  (void)png_get_image_width(png_ptr, info_ptr);
  (void)png_get_pixel_aspect_ratio(png_ptr, info_ptr);

  char* name = nullptr;
  int compression_type = -1;
  png_byte* profile = nullptr;
  png_uint_32 proflen = 0;
  (void)png_get_iCCP(png_ptr, info_ptr, &name, &compression_type, &profile, &proflen);

  (void)png_get_y_offset_pixels(nullptr, info_ptr);
  (void)png_get_y_offset_pixels(png_ptr, nullptr);
  (void)png_get_interlace_type(nullptr, info_ptr);
  (void)png_get_interlace_type(png_ptr, nullptr);
  (void)png_get_image_width(nullptr, info_ptr);
  (void)png_get_image_width(png_ptr, nullptr);
  (void)png_get_pixel_aspect_ratio(nullptr, info_ptr);
  (void)png_get_pixel_aspect_ratio(png_ptr, nullptr);
  (void)png_get_iCCP(nullptr, info_ptr, &name, &compression_type, &profile, &proflen);
  (void)png_get_iCCP(png_ptr, nullptr, &name, &compression_type, &profile, &proflen);

  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);

  if (fp != nullptr) fclose(fp);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  ExerciseLibpng(Data, Size, false);
  ExerciseLibpng(Data, Size, true);
  return 0;
}