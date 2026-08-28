// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_start_read_image at pngread.c:298:1 in png.h
// png_get_image_width at pngget.c:52:1 in png.h
// png_get_image_height at pngget.c:61:1 in png.h
// png_read_row at pngread.c:383:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
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
// png_set_read_fn at pngrio.c:87:1 in png.h
// png_set_shift at pngtrans.c:84:1 in png.h
// png_set_sBIT at pngset.c:638:1 in png.h
// png_get_sBIT at pngget.c:1031:1 in png.h
// png_read_info at pngread.c:92:1 in png.h
// png_get_sBIT at pngget.c:1031:1 in png.h
// png_read_update_info at pngread.c:267:1 in png.h
// png_get_rowbytes at pngget.c:30:1 in png.h
// png_start_read_image at pngread.c:298:1 in png.h
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

static void PngReadCallback(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead) {
  png_voidp io_ptr = png_get_io_ptr(png_ptr);
  if (io_ptr == nullptr) {
    png_error(png_ptr, "no io_ptr");
    return;
  }

  struct ReaderState {
    const uint8_t* data;
    size_t size;
    size_t offset;
  };

  ReaderState* st = static_cast<ReaderState*>(io_ptr);
  if (st->offset > st->size || byteCountToRead > st->size - st->offset) {
    png_error(png_ptr, "read beyond end");
    return;
  }

  if (byteCountToRead > 0) {
    memcpy(outBytes, st->data + st->offset, byteCountToRead);
    st->offset += byteCountToRead;
  }
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE* fp = fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    if (Size > 0) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  struct ReaderState {
    const uint8_t* data;
    size_t size;
    size_t offset;
  } state = {Data, Size, 0};

  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (png_ptr == nullptr) return 0;

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr) {
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    return 0;
  }

  png_infop end_info = png_create_info_struct(png_ptr);
  if (end_info == nullptr) {
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    return 0;
  }

  png_set_read_fn(png_ptr, &state, PngReadCallback);

  png_color_8 sig = {};
  if (Size > 0) sig.red = Data[0];
  if (Size > 1) sig.green = Data[1];
  if (Size > 2) sig.blue = Data[2];
  if (Size > 3) sig.gray = Data[3];
  if (Size > 4) sig.alpha = Data[4];

  png_set_shift(png_ptr, &sig);
  png_set_sBIT(png_ptr, info_ptr, &sig);

  png_color_8p out_sig = nullptr;
  (void)png_get_sBIT(png_ptr, info_ptr, &out_sig);

  png_read_info(png_ptr, info_ptr);

  png_color_8p read_sig = nullptr;
  (void)png_get_sBIT(png_ptr, info_ptr, &read_sig);

  png_read_update_info(png_ptr, info_ptr);

  size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);

  png_start_read_image(png_ptr);
  if (Size > 5 && (Data[5] & 1)) {
    png_start_read_image(png_ptr);
  }

  png_uint_32 width = png_get_image_width(png_ptr, info_ptr);
  png_uint_32 height = png_get_image_height(png_ptr, info_ptr);

  if (rowbytes > 0 && rowbytes < (1u << 26) && height > 0 && height < (1u << 20)) {
    std::vector<png_byte> row(rowbytes);
    png_uint_32 rows_to_read = height;
    if (rows_to_read > 32) rows_to_read = 32;

    for (png_uint_32 y = 0; y < rows_to_read; ++y) {
      png_read_row(png_ptr, row.data(), nullptr);
    }
  }

  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
  return 0;
}