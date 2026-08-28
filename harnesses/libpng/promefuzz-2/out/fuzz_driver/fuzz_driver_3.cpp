// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_read_row at pngread.c:383:1 in png.h
// png_read_end at pngread.c:766:1 in png.h
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
// png_read_info at pngread.c:92:1 in png.h
// png_set_expand at pngrtran.c:872:1 in png.h
// png_set_packing at pngtrans.c:50:1 in png.h
// png_set_scale_16 at pngrtran.c:183:1 in png.h
// png_set_tRNS_to_alpha at pngrtran.c:926:1 in png.h
// png_set_interlace_handling at pngtrans.c:99:1 in png.h
// png_read_update_info at pngread.c:267:1 in png.h
// png_get_IHDR at pngget.c:825:1 in png.h
// png_get_rowbytes at pngget.c:30:1 in png.h
// png_read_end at pngread.c:766:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
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

static void PngReadCallback(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead) {
  png_voidp io_ptr = png_get_io_ptr(png_ptr);
  if (io_ptr == nullptr) {
    png_error(png_ptr, "No IO pointer");
    return;
  }

  struct BufferState {
    const uint8_t* data;
    size_t size;
    size_t offset;
  };

  BufferState* state = static_cast<BufferState*>(io_ptr);
  if (state->offset > state->size || byteCountToRead > state->size - state->offset) {
    png_error(png_ptr, "Read beyond end of buffer");
    return;
  }

  memcpy(outBytes, state->data + state->offset, byteCountToRead);
  state->offset += byteCountToRead;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  if (Data == nullptr || Size == 0) {
    return 0;
  }

  FILE* fp = fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    fwrite(Data, 1, Size, fp);
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

  png_infop end_info = png_create_info_struct(png_ptr);
  if (end_info == nullptr) {
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    return 0;
  }

  struct BufferState {
    const uint8_t* data;
    size_t size;
    size_t offset;
  } state = {Data, Size, 0};

  png_set_read_fn(png_ptr, &state, PngReadCallback);

  png_read_info(png_ptr, info_ptr);

  png_set_expand(png_ptr);
  png_set_packing(png_ptr);
  png_set_scale_16(png_ptr);
  png_set_tRNS_to_alpha(png_ptr);
  int passes = png_set_interlace_handling(png_ptr);
  (void)passes;

  png_read_update_info(png_ptr, info_ptr);

  png_uint_32 width = 0, height = 0;
  int bit_depth = 0, color_type = 0, interlace_type = 0, compression_type = 0, filter_method = 0;
  png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
               &interlace_type, &compression_type, &filter_method);

  png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);
  if (rowbytes == 0 || height == 0) {
    png_read_end(png_ptr, end_info);
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    return 0;
  }

  if (height > 1000000 || rowbytes > 10000000) {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    return 0;
  }

  std::vector<png_byte> row(rowbytes);

  int effective_passes = passes > 0 ? passes : 1;
  for (int pass = 0; pass < effective_passes; ++pass) {
    for (png_uint_32 y = 0; y < height; ++y) {
      png_read_row(png_ptr, row.data(), nullptr);
    }
  }

  png_read_end(png_ptr, end_info);
  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
  return 0;
}