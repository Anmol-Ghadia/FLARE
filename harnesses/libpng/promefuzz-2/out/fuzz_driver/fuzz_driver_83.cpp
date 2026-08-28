// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_get_iCCP at pngget.c:736:1 in png.h
// png_set_expand at pngrtran.c:872:1 in png.h
// png_set_gray_to_rgb at pngrtran.c:955:1 in png.h
// png_set_strip_16 at pngrtran.c:197:1 in png.h
// png_read_update_info at pngread.c:267:1 in png.h
// png_read_update_info at pngread.c:267:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_get_interlace_type at pngget.c:97:1 in png.h
// png_get_image_width at pngget.c:52:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_read_fn at pngrio.c:87:1 in png.h
// png_set_sig_bytes at png.c:47:1 in png.h
// png_set_sig_bytes at png.c:47:1 in png.h
// png_read_info at pngread.c:92:1 in png.h
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
#include <setjmp.h>

static void PngReadCallback(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead) {
  png_bytep io_ptr = static_cast<png_bytep>(png_get_io_ptr(png_ptr));
  if (io_ptr == nullptr) {
    png_error(png_ptr, "No IO pointer");
    return;
  }

  struct State {
    const uint8_t* data;
    size_t size;
    size_t offset;
  };

  State* st = reinterpret_cast<State*>(io_ptr);
  if (st->offset > st->size || byteCountToRead > st->size - st->offset) {
    png_error(png_ptr, "Read beyond end");
    return;
  }

  if (byteCountToRead > 0) {
    memcpy(outBytes, st->data + st->offset, byteCountToRead);
    st->offset += byteCountToRead;
  }
}

static void ExerciseNullSafeApis(const uint8_t* Data, size_t Size) {
  (void)png_get_interlace_type(nullptr, nullptr);
  (void)png_get_image_width(nullptr, nullptr);

  if (Size >= 2) {
    (void)png_get_uint_16(Data);
  } else {
    uint8_t tmp[2] = {0, 0};
    if (Size == 1) tmp[0] = Data[0];
    (void)png_get_uint_16(tmp);
  }
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  ExerciseNullSafeApis(Data, Size);

  FILE* fp = fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    if (Size > 0) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
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

  struct State {
    const uint8_t* data;
    size_t size;
    size_t offset;
  } state = {Data, Size, 0};

  png_set_read_fn(png_ptr, &state, PngReadCallback);

  if (Size >= 8) {
    png_set_sig_bytes(png_ptr, 0);
  } else {
    png_set_sig_bytes(png_ptr, 0);
  }

  png_read_info(png_ptr, info_ptr);

  (void)png_get_interlace_type(png_ptr, info_ptr);
  (void)png_get_image_width(png_ptr, info_ptr);

  png_charp name = nullptr;
  int compression_type = -1;
  png_bytep profile = nullptr;
  png_uint_32 proflen = 0;
  (void)png_get_iCCP(png_ptr, info_ptr, &name, &compression_type, &profile, &proflen);

  if (Size > 0) {
    switch (Data[0] % 4) {
      case 0:
        break;
      case 1:
        png_set_expand(png_ptr);
        break;
      case 2:
        png_set_gray_to_rgb(png_ptr);
        break;
      case 3:
        png_set_strip_16(png_ptr);
        break;
    }
  }

  png_read_update_info(png_ptr, info_ptr);

  if (Size > 1 && (Data[1] & 1)) {
    png_read_update_info(png_ptr, info_ptr);
  }

  if (Size >= 2) {
    (void)png_get_uint_16(Data + (Size >= 2 ? 0 : Size));
  }

  png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
  return 0;
}