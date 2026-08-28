// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_read_fn at pngrio.c:87:1 in png.h
// png_set_sig_bytes at png.c:47:1 in png.h
// png_set_crc_action at pngrtran.c:36:1 in png.h
// png_set_packing at pngtrans.c:50:1 in png.h
// png_set_check_for_invalid_index at pngset.c:1701:1 in png.h
// png_set_option at png.c:4332:1 in png.h
// png_read_info at pngread.c:92:1 in png.h
// png_get_image_width at pngget.c:52:1 in png.h
// png_get_image_height at pngget.c:61:1 in png.h
// png_get_bit_depth at pngget.c:70:1 in png.h
// png_get_color_type at pngget.c:79:1 in png.h
// png_set_packing at pngtrans.c:50:1 in png.h
// png_set_check_for_invalid_index at pngset.c:1701:1 in png.h
// png_set_option at png.c:4332:1 in png.h
// png_read_update_info at pngread.c:267:1 in png.h
// png_get_rowbytes at pngget.c:30:1 in png.h
// png_read_image at pngread.c:700:1 in png.h
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

static void FuzzReadCallback(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead) {
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

  for (png_size_t i = 0; i < byteCountToRead; ++i) {
    outBytes[i] = state->data[state->offset + i];
  }
  state->offset += byteCountToRead;
}

static int ConsumeInt(const uint8_t* data, size_t size, size_t* offset, int fallback) {
  if (*offset + 4 > size) return fallback;
  int v = static_cast<int>(data[*offset]) |
          (static_cast<int>(data[*offset + 1]) << 8) |
          (static_cast<int>(data[*offset + 2]) << 16) |
          (static_cast<int>(data[*offset + 3]) << 24);
  *offset += 4;
  return v;
}

extern "C" int LLVMFuzzerTestOneInput_79(const uint8_t *Data, size_t Size) {
  if (Data == nullptr || Size == 0) {
    return 0;
  }

  std::FILE* fp = std::fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    (void)std::fwrite(Data, 1, Size, fp);
    std::fclose(fp);
  }

  size_t ctrl = 0;
  int crit_action = ConsumeInt(Data, Size, &ctrl, PNG_CRC_DEFAULT);
  int ancil_action = ConsumeInt(Data, Size, &ctrl, PNG_CRC_DEFAULT);
  int invalid_index_allowed = ConsumeInt(Data, Size, &ctrl, 0);
  int option = ConsumeInt(Data, Size, &ctrl, 0);
  int onoff = ConsumeInt(Data, Size, &ctrl, 0);
  int pre_read_sig = ConsumeInt(Data, Size, &ctrl, 0);

  struct BufferState {
    const uint8_t* data;
    size_t size;
    size_t offset;
  } state{Data, Size, 0};

  if ((pre_read_sig & 1) && Size >= 8) {
    state.offset = 8;
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

  png_infop end_info_ptr = png_create_info_struct(png_ptr);
  if (end_info_ptr == nullptr) {
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info_ptr);
    return 0;
  }

  png_set_read_fn(png_ptr, &state, FuzzReadCallback);

  if ((pre_read_sig & 1) && Size >= 8) {
    png_set_sig_bytes(png_ptr, 8);
  }

  png_set_crc_action(png_ptr, crit_action, ancil_action);
  png_set_packing(png_ptr);
  png_set_check_for_invalid_index(png_ptr, invalid_index_allowed);
  (void)png_set_option(png_ptr, option, onoff);

  png_read_info(png_ptr, info_ptr);

  png_uint_32 width = png_get_image_width(png_ptr, info_ptr);
  png_uint_32 height = png_get_image_height(png_ptr, info_ptr);
  int bit_depth = png_get_bit_depth(png_ptr, info_ptr);
  int color_type = png_get_color_type(png_ptr, info_ptr);

  if ((ctrl < Size) && (Data[ctrl] & 1)) {
    png_set_packing(png_ptr);
  }
  if ((ctrl + 1 < Size)) {
    png_set_check_for_invalid_index(png_ptr, static_cast<int>(Data[ctrl + 1]));
  }
  if ((ctrl + 2 < Size)) {
    (void)png_set_option(png_ptr, static_cast<int>(Data[ctrl + 2] & 0xFE), static_cast<int>(Data[ctrl + 2] & 1));
  }

  if (width > 0 && height > 0 && width < 4096 && height < 4096 &&
      bit_depth > 0 &&
      (color_type == PNG_COLOR_TYPE_GRAY ||
       color_type == PNG_COLOR_TYPE_GRAY_ALPHA ||
       color_type == PNG_COLOR_TYPE_PALETTE ||
       color_type == PNG_COLOR_TYPE_RGB ||
       color_type == PNG_COLOR_TYPE_RGB_ALPHA)) {
    png_read_update_info(png_ptr, info_ptr);
    png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);
    if (rowbytes > 0 && rowbytes < (1u << 20)) {
      std::vector<png_byte> image;
      std::vector<png_bytep> rows;
      if (height < 4096) {
        image.resize(rowbytes * height);
        rows.resize(height);
        for (png_uint_32 y = 0; y < height; ++y) {
          rows[y] = image.data() + y * rowbytes;
        }
        png_read_image(png_ptr, rows.data());
      }
    }
  }

  png_read_end(png_ptr, end_info_ptr);
  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info_ptr);
  return 0;
}