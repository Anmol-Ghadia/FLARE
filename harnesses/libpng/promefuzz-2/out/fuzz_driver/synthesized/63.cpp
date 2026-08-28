// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_process_data_skip at pngpread.c:78:1 in png.h
// png_set_read_fn at pngrio.c:87:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_set_read_user_chunk_fn at pngset.c:1531:1 in png.h
// png_set_progressive_read_fn at pngpread.c:1074:1 in png.h
// png_process_data at pngpread.c:36:1 in png.h
// png_process_data_skip at pngpread.c:78:1 in png.h
// png_process_data at pngpread.c:36:1 in png.h
// png_process_data_skip at pngpread.c:78:1 in png.h
// png_process_data at pngpread.c:36:1 in png.h
// png_process_data_skip at pngpread.c:78:1 in png.h
// png_process_data at pngpread.c:36:1 in png.h
// png_process_data_skip at pngpread.c:78:1 in png.h
// png_process_data at pngpread.c:36:1 in png.h
// png_process_data_skip at pngpread.c:78:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
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
#include <cstring>
#include <vector>
#include <fstream>

struct FuzzContext {
  const uint8_t* data;
  size_t size;
  size_t offset;
  png_uint_32 callback_count;
};

static void FuzzReadFn(png_structp png_ptr, png_bytep outBytes, size_t byteCountToRead) {
  FuzzContext* ctx =
      static_cast<FuzzContext*>(png_get_io_ptr(reinterpret_cast<png_const_structrp>(png_ptr)));
  if (ctx == nullptr || outBytes == nullptr) {
    png_error(png_ptr, "invalid read context");
    return;
  }

  size_t remaining = (ctx->offset <= ctx->size) ? (ctx->size - ctx->offset) : 0;
  size_t to_copy = byteCountToRead < remaining ? byteCountToRead : remaining;

  if (to_copy > 0) {
    memcpy(outBytes, ctx->data + ctx->offset, to_copy);
    ctx->offset += to_copy;
  }

  if (to_copy < byteCountToRead) {
    memset(outBytes + to_copy, 0, byteCountToRead - to_copy);
    png_error(png_ptr, "short read");
  }
}

static int FuzzUserChunkFn(png_structp png_ptr, png_unknown_chunkp chunk) {
  FuzzContext* ctx =
      static_cast<FuzzContext*>(png_get_io_ptr(reinterpret_cast<png_const_structrp>(png_ptr)));
  if (ctx != nullptr) {
    ctx->callback_count++;
    if (chunk != nullptr && chunk->size > 0) {
      uint8_t v = chunk->data[0];
      if ((v % 3) == 0) return 1;
      if ((v % 3) == 1) return 0;
      return -1;
    }
  }
  return 0;
}

static void FuzzInfoFn(png_structp png_ptr, png_infop info_ptr) {
  (void)png_ptr;
  (void)info_ptr;
}

static void FuzzRowFn(png_structp png_ptr, png_bytep new_row, png_uint_32 row_num, int pass) {
  (void)png_ptr;
  (void)new_row;
  (void)row_num;
  (void)pass;
}

static void FuzzEndFn(png_structp png_ptr, png_infop info_ptr) {
  (void)png_ptr;
  (void)info_ptr;
}

extern "C" int LLVMFuzzerTestOneInput_63(const uint8_t *Data, size_t Size) {
  std::ofstream ofs("./dummy_file", std::ios::binary);
  if (ofs) {
    ofs.write(reinterpret_cast<const char*>(Data), static_cast<std::streamsize>(Size));
  }
  ofs.close();

  FuzzContext ctx;
  ctx.data = Data;
  ctx.size = Size;
  ctx.offset = 0;
  ctx.callback_count = 0;

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

  png_get_io_ptr(nullptr);
  png_process_data_skip(png_ptr);

  png_set_read_fn(png_ptr, &ctx, FuzzReadFn);
  (void)png_get_io_ptr(reinterpret_cast<png_const_structrp>(png_ptr));

  png_set_read_user_chunk_fn(png_ptr, &ctx, FuzzUserChunkFn);
  png_set_progressive_read_fn(png_ptr, &ctx, FuzzInfoFn, FuzzRowFn, FuzzEndFn);

  if (Size > 0) {
    size_t mode = Data[0] % 3;

    if (mode == 0) {
      png_process_data(png_ptr, info_ptr, const_cast<png_bytep>(reinterpret_cast<const png_byte*>(Data)), Size);
      (void)png_process_data_skip(png_ptr);
    } else if (mode == 1) {
      size_t pos = 0;
      while (pos < Size) {
        size_t chunk = 1 + (Data[pos] % 32);
        if (chunk > Size - pos) chunk = Size - pos;
        png_process_data(png_ptr, info_ptr,
                         const_cast<png_bytep>(reinterpret_cast<const png_byte*>(Data + pos)),
                         chunk);
        (void)png_process_data_skip(png_ptr);
        pos += chunk;
      }
    } else {
      size_t split1 = Size / 2;
      size_t split2 = Size - split1;
      png_process_data(png_ptr, info_ptr,
                       const_cast<png_bytep>(reinterpret_cast<const png_byte*>(Data)),
                       split1);
      (void)png_process_data_skip(png_ptr);
      png_process_data(png_ptr, info_ptr,
                       const_cast<png_bytep>(reinterpret_cast<const png_byte*>(Data + split1)),
                       split2);
      (void)png_process_data_skip(png_ptr);
    }
  } else {
    png_process_data(png_ptr, info_ptr, nullptr, 0);
    (void)png_process_data_skip(png_ptr);
  }

  png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
  return 0;
}