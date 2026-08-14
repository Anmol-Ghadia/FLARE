// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_expand at pngrtran.c:872:1 in png.h
// png_set_packing at pngtrans.c:50:1 in png.h
// png_set_scale_16 at pngrtran.c:183:1 in png.h
// png_set_tRNS_to_alpha at pngrtran.c:926:1 in png.h
// png_set_interlace_handling at pngtrans.c:99:1 in png.h
// png_read_update_info at pngread.c:267:1 in png.h
// png_get_image_width at pngget.c:52:1 in png.h
// png_get_image_height at pngget.c:61:1 in png.h
// png_get_rowbytes at pngget.c:30:1 in png.h
// png_read_end at pngread.c:766:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_read_row at pngread.c:383:1 in png.h
// png_read_end at pngread.c:766:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_read_fn at pngrio.c:87:1 in png.h
// png_set_sig_bytes at png.c:47:1 in png.h
// png_set_sig_bytes at png.c:47:1 in png.h
// png_read_info at pngread.c:92:1 in png.h
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
#include <setjmp.h>

static void PngReadCallback(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead) {
  if (png_ptr == nullptr || outBytes == nullptr) return;

  struct ReaderState {
    const uint8_t* data;
    size_t size;
    size_t offset;
  };

  ReaderState* state =
      reinterpret_cast<ReaderState*>(png_get_io_ptr(png_ptr));
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
  if (Data == nullptr || Size == 0) return 0;

  {
    FILE* fp = fopen("./dummy_file", "wb");
    if (fp != nullptr) {
      fwrite(Data, 1, Size, fp);
      fclose(fp);
    }
  }

  struct ReaderState {
    const uint8_t* data;
    size_t size;
    size_t offset;
  };

  for (int mode = 0; mode < 3; ++mode) {
    png_structp png_ptr =
        png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr,
                               PngErrorCallback, PngWarningCallback);
    if (png_ptr == nullptr) continue;

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == nullptr) {
      png_destroy_read_struct(&png_ptr, nullptr, nullptr);
      continue;
    }

    png_infop end_info = png_create_info_struct(png_ptr);
    if (end_info == nullptr) {
      png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
      continue;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
      png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
      continue;
    }

    ReaderState state{Data, Size, 0};
    png_set_read_fn(png_ptr, &state, PngReadCallback);

    if (mode == 1 && Size > 8) {
      png_set_sig_bytes(png_ptr, 8);
      state.offset = 8;
    } else if (mode == 2 && Size > 4) {
      png_set_sig_bytes(png_ptr, 4);
      state.offset = 4;
    }

    png_read_info(png_ptr, info_ptr);

    png_set_expand(png_ptr);
    png_set_packing(png_ptr);
    png_set_scale_16(png_ptr);
    png_set_tRNS_to_alpha(png_ptr);
    int passes = png_set_interlace_handling(png_ptr);
    (void)passes;

    png_read_update_info(png_ptr, info_ptr);

    png_uint_32 width = png_get_image_width(png_ptr, info_ptr);
    png_uint_32 height = png_get_image_height(png_ptr, info_ptr);
    png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);

    if (rowbytes == 0 || height == 0) {
      png_read_end(png_ptr, end_info);
      png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
      continue;
    }

    const png_uint_32 kMaxDim = 4096;
    const png_size_t kMaxRowbytes = 1u << 20;

    if (width > kMaxDim || height > kMaxDim || rowbytes > kMaxRowbytes) {
      png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
      continue;
    }

    std::vector<png_byte> row(rowbytes);
    int total_passes = passes > 0 ? passes : 1;

    for (int pass = 0; pass < total_passes; ++pass) {
      for (png_uint_32 y = 0; y < height; ++y) {
        png_read_row(png_ptr, row.data(), nullptr);
      }
    }

    png_read_end(png_ptr, end_info);
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
  }

  return 0;
}