// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_get_IHDR at pngget.c:825:1 in png.h
// png_get_IHDR at pngget.c:825:1 in png.h
// png_get_IHDR at pngget.c:825:1 in png.h
// png_get_tRNS at pngget.c:1093:1 in png.h
// png_get_tRNS at pngget.c:1093:1 in png.h
// png_get_tRNS at pngget.c:1093:1 in png.h
// png_get_text at pngget.c:1049:1 in png.h
// png_get_text at pngget.c:1049:1 in png.h
// png_get_text at pngget.c:1049:1 in png.h
// png_set_tRNS at pngset.c:994:1 in png.h
// png_set_tRNS at pngset.c:994:1 in png.h
// png_set_tRNS at pngset.c:994:1 in png.h
// png_set_tRNS at pngset.c:994:1 in png.h
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
// png_set_sig_bytes at png.c:47:1 in png.h
// png_read_info at pngread.c:92:1 in png.h
// png_get_signature at pngget.c:477:1 in png.h
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
  if (png_ptr == nullptr || outBytes == nullptr) {
    return;
  }

  png_bytep io_ptr = static_cast<png_bytep>(png_get_io_ptr(png_ptr));
  if (io_ptr == nullptr) {
    png_error(png_ptr, "No IO pointer");
    return;
  }

  struct BufferState {
    const uint8_t* data;
    size_t size;
    size_t offset;
  };

  BufferState* state = reinterpret_cast<BufferState*>(io_ptr);
  if (state->offset > state->size || byteCountToRead > state->size - state->offset) {
    png_error(png_ptr, "Read beyond end of buffer");
    return;
  }

  memcpy(outBytes, state->data + state->offset, byteCountToRead);
  state->offset += byteCountToRead;
}

extern "C" int LLVMFuzzerTestOneInput_56(const uint8_t *Data, size_t Size) {
  if (Data == nullptr) {
    return 0;
  }

  FILE* fp = fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  struct BufferState {
    const uint8_t* data;
    size_t size;
    size_t offset;
  } state = {Data, Size, 0};

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

  png_set_read_fn(png_ptr, &state, PngReadCallback);

  if (Size >= 8) {
    png_set_sig_bytes(png_ptr, 0);
  }

  png_read_info(png_ptr, info_ptr);

  png_const_bytep sig = png_get_signature(png_ptr, info_ptr);
  if (sig != nullptr) {
    volatile png_byte b0 = sig[0];
    volatile png_byte b1 = sig[1];
    volatile png_byte b2 = sig[2];
    volatile png_byte b3 = sig[3];
    (void)b0;
    (void)b1;
    (void)b2;
    (void)b3;
  }

  png_uint_32 width = 0, height = 0;
  int bit_depth = 0, color_type = 0, interlace_method = 0, compression_method = 0, filter_method = 0;
  png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
               &interlace_method, &compression_method, &filter_method);

  png_get_IHDR(png_ptr, info_ptr, nullptr, &height, nullptr, &color_type,
               nullptr, &compression_method, nullptr);
  png_get_IHDR(png_ptr, info_ptr, &width, nullptr, &bit_depth, nullptr,
               &interlace_method, nullptr, &filter_method);

  png_bytep trans_alpha = nullptr;
  int num_trans = 0;
  png_color_16p trans_color = nullptr;
  png_get_tRNS(png_ptr, info_ptr, &trans_alpha, &num_trans, &trans_color);
  png_get_tRNS(png_ptr, info_ptr, nullptr, &num_trans, nullptr);
  png_get_tRNS(png_ptr, info_ptr, &trans_alpha, nullptr, &trans_color);

  png_textp text_ptr = nullptr;
  int num_text = 0;
  int text_count = png_get_text(png_ptr, info_ptr, &text_ptr, &num_text);
  if (text_count > 0 && text_ptr != nullptr) {
    for (int i = 0; i < text_count; ++i) {
      volatile int compression = text_ptr[i].compression;
      volatile size_t text_length = text_ptr[i].text_length;
      (void)compression;
      (void)text_length;
      if (text_ptr[i].key != nullptr) {
        volatile char c = text_ptr[i].key[0];
        (void)c;
      }
    }
  }
  png_get_text(png_ptr, info_ptr, nullptr, &num_text);
  png_get_text(png_ptr, info_ptr, &text_ptr, nullptr);

  png_color_16 local_color;
  memset(&local_color, 0, sizeof(local_color));
  local_color.index = static_cast<png_byte>(Size > 0 ? Data[0] : 0);
  local_color.red = static_cast<png_uint_16>(Size > 1 ? (Data[1] << 8) | Data[0] : 0);

  png_byte local_alpha[256];
  int alpha_count = 0;
  if (Size > 0) {
    alpha_count = static_cast<int>(Size > 256 ? 256 : Size);
    memcpy(local_alpha, Data, static_cast<size_t>(alpha_count));
  }

  png_set_tRNS(png_ptr, info_ptr, nullptr, 0, nullptr);
  png_set_tRNS(png_ptr, info_ptr, local_alpha, alpha_count, &local_color);
  png_set_tRNS(png_ptr, info_ptr, nullptr, 0, &local_color);
  if (alpha_count > 0) {
    png_set_tRNS(png_ptr, info_ptr, local_alpha, alpha_count / 2, nullptr);
  }

  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
  return 0;
}