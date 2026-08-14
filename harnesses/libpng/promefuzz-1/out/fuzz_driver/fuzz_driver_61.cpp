// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_interlace_handling at pngtrans.c:99:1 in png.h
// png_set_interlace_handling at pngtrans.c:99:1 in png.h
// png_get_current_pass_number at pngtrans.c:856:1 in png.h
// png_set_interlace_handling at pngtrans.c:99:1 in png.h
// png_read_update_info at pngread.c:267:1 in png.h
// png_get_image_width at pngget.c:52:1 in png.h
// png_get_image_height at pngget.c:61:1 in png.h
// png_get_rowbytes at pngget.c:30:1 in png.h
// png_get_current_pass_number at pngtrans.c:856:1 in png.h
// png_read_row at pngread.c:383:1 in png.h
// png_get_current_pass_number at pngtrans.c:856:1 in png.h
// png_read_end at pngread.c:766:1 in png.h
// png_read_end at pngread.c:766:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_get_current_pass_number at pngtrans.c:856:1 in png.h
// png_read_info at pngread.c:92:1 in png.h
// png_read_update_info at pngread.c:267:1 in png.h
// png_read_end at pngread.c:766:1 in png.h
// png_set_interlace_handling at pngtrans.c:99:1 in png.h
// png_read_row at pngread.c:383:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_read_fn at pngrio.c:87:1 in png.h
// png_read_info at pngread.c:92:1 in png.h
// png_get_current_pass_number at pngtrans.c:856:1 in png.h
// png_set_interlace_handling at pngtrans.c:99:1 in png.h
// png_set_interlace_handling at pngtrans.c:99:1 in png.h
// png_get_current_pass_number at pngtrans.c:856:1 in png.h
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
#include <setjmp.h>

static void PngReadCallback(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead) {
  if (png_ptr == nullptr || outBytes == nullptr) return;

  struct ReaderState {
    const uint8_t* data;
    size_t size;
    size_t offset;
  };

  ReaderState* state =
      static_cast<ReaderState*>(png_get_io_ptr(png_ptr));
  if (state == nullptr || state->offset > state->size ||
      byteCountToRead > state->size - state->offset) {
    png_error(png_ptr, "read error");
    return;
  }

  memcpy(outBytes, state->data + state->offset, byteCountToRead);
  state->offset += byteCountToRead;
}

static void PngErrorCallback(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningCallback(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  {
    FILE* fp = std::fopen("./dummy_file", "wb");
    if (fp != nullptr) {
      if (Size > 0) std::fwrite(Data, 1, Size, fp);
      std::fclose(fp);
    }
  }

  png_get_current_pass_number(nullptr);
  png_read_info(nullptr, nullptr);
  png_read_update_info(nullptr, nullptr);
  png_read_end(nullptr, nullptr);
  png_set_interlace_handling(nullptr);
  png_read_row(nullptr, nullptr, nullptr);

  struct ReaderState {
    const uint8_t* data;
    size_t size;
    size_t offset;
  } state = {Data, Size, 0};

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr,
                             PngErrorCallback, PngWarningCallback);
  if (png_ptr == nullptr) return 0;

  png_infop info_ptr = png_create_info_struct(png_ptr);
  png_infop end_info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr || end_info_ptr == nullptr) {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info_ptr);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info_ptr);
    return 0;
  }

  png_set_read_fn(png_ptr, &state, PngReadCallback);

  png_read_info(png_ptr, info_ptr);

  png_get_current_pass_number(png_ptr);

  if (Size > 0) {
    switch (Data[0] % 6) {
      case 0:
        png_set_interlace_handling(png_ptr);
        break;
      case 1:
        png_set_interlace_handling(png_ptr);
        png_get_current_pass_number(png_ptr);
        break;
      case 2:
        break;
      case 3:
        png_set_interlace_handling(png_ptr);
        png_set_interlace_handling(png_ptr);
        break;
      case 4:
        png_get_current_pass_number(png_ptr);
        break;
      case 5:
        png_set_interlace_handling(png_ptr);
        break;
    }
  }

  png_read_update_info(png_ptr, info_ptr);

  png_uint_32 width = png_get_image_width(png_ptr, info_ptr);
  png_uint_32 height = png_get_image_height(png_ptr, info_ptr);
  png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);

  png_get_current_pass_number(png_ptr);

  if (rowbytes > 0 && height > 0) {
    std::vector<png_byte> row(rowbytes);
    std::vector<png_byte> display_row(rowbytes);

    png_uint_32 rows_to_read = height;
    if (rows_to_read > 64) rows_to_read = 64;

    bool use_display = (Size > 1) ? ((Data[1] & 1) != 0) : false;
    bool alternate_display = (Size > 2) ? ((Data[2] & 1) != 0) : false;

    for (png_uint_32 y = 0; y < rows_to_read; ++y) {
      png_bytep display_ptr = nullptr;
      if (use_display) {
        display_ptr = display_row.data();
        if (alternate_display && (y & 1)) display_ptr = nullptr;
      }
      png_read_row(png_ptr, row.data(), display_ptr);
      png_get_current_pass_number(png_ptr);
    }
  }

  if (Size > 3 && (Data[3] & 1) != 0) {
    png_read_end(png_ptr, end_info_ptr);
  } else {
    png_read_end(png_ptr, nullptr);
  }

  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info_ptr);
  return 0;
}