// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_keep_unknown_chunks at pngset.c:1372:1 in png.h
// png_handle_as_unknown at png.c:926:1 in png.h
// png_handle_as_unknown at png.c:926:1 in png.h
// png_handle_as_unknown at png.c:926:1 in png.h
// png_set_unknown_chunks at pngset.c:1200:1 in png.h
// png_set_unknown_chunks at pngset.c:1200:1 in png.h
// png_set_unknown_chunks at pngset.c:1200:1 in png.h
// png_get_unknown_chunks at pngget.c:1139:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_get_user_chunk_ptr at pngget.c:1162:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_read_user_chunk_fn at pngset.c:1531:1 in png.h
// png_get_user_chunk_ptr at pngget.c:1162:1 in png.h
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
#include <string.h>
#include <vector>

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarnFn(png_structp, png_const_charp) {
}

static int UserChunkCallback(png_structp png_ptr, png_unknown_chunkp chunk) {
  png_voidp user_ptr = png_get_user_chunk_ptr(png_ptr);
  uintptr_t mode = reinterpret_cast<uintptr_t>(user_ptr);

  if (chunk == nullptr)
    return 0;

  if ((mode & 1u) != 0 && chunk->size > 0 && chunk->data != nullptr) {
    volatile png_byte b = chunk->data[0];
    (void)b;
  }

  if ((mode & 2u) != 0)
    return 1;   // handled/discard

  if ((mode & 4u) != 0)
    return -1;  // error

  return 0;     // save
}

extern "C" int LLVMFuzzerTestOneInput_7(const uint8_t *Data, size_t Size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    if (Size > 0)
      fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn, PngWarnFn);
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

  uintptr_t user_mode = 0;
  if (Size > 0)
    user_mode = Data[0] & 7u;

  png_set_read_user_chunk_fn(
      png_ptr, reinterpret_cast<png_voidp>(user_mode), UserChunkCallback);

  png_voidp got_user = png_get_user_chunk_ptr(png_ptr);
  (void)got_user;

  int keep_values[] = {
      PNG_HANDLE_CHUNK_AS_DEFAULT,
      PNG_HANDLE_CHUNK_NEVER,
      PNG_HANDLE_CHUNK_IF_SAFE,
      PNG_HANDLE_CHUNK_ALWAYS
  };

  int keep = keep_values[(Size > 1 ? Data[1] : 0) % 4];

  png_byte chunk_list[12];
  memset(chunk_list, 0, sizeof(chunk_list));
  if (Size > 5) {
    for (int i = 0; i < 8; ++i)
      chunk_list[i] = Data[2 + (i % (Size - 2))];
  } else {
    memcpy(chunk_list, "vpAgteST", 8);
  }

  int num_chunks = 0;
  if (Size > 2) {
    uint8_t selector = Data[2] % 4;
    if (selector == 0)
      num_chunks = 0;
    else if (selector == 1)
      num_chunks = 1;
    else if (selector == 2)
      num_chunks = 2;
    else
      num_chunks = -1;
  }

  png_set_keep_unknown_chunks(png_ptr, keep, chunk_list, num_chunks);

  png_byte query_name1[5] = { 'v', 'p', 'A', 'g', 0 };
  png_byte query_name2[5] = { 't', 'e', 'S', 'T', 0 };
  png_byte query_name3[5] = { 0, 0, 0, 0, 0 };

  (void)png_handle_as_unknown(png_ptr, query_name1);
  (void)png_handle_as_unknown(png_ptr, query_name2);
  (void)png_handle_as_unknown(png_ptr, query_name3);

  std::vector<png_byte> storage;
  if (Size > 0)
    storage.assign(Data, Data + Size);
  else
    storage.push_back(0);

  png_unknown_chunk chunks[2];
  memset(chunks, 0, sizeof(chunks));

  memcpy(chunks[0].name, "vpAg", 4);
  chunks[0].name[4] = 0;
  chunks[0].data = storage.data();
  chunks[0].size = storage.size() > 16 ? 16 : storage.size();
  chunks[0].location = 1;

  memcpy(chunks[1].name, "teST", 4);
  chunks[1].name[4] = 0;
  chunks[1].data = storage.data();
  chunks[1].size = storage.size() > 32 ? 32 : storage.size();
  chunks[1].location = 2;

  png_set_unknown_chunks(png_ptr, info_ptr, chunks, 1);
  png_set_unknown_chunks(png_ptr, info_ptr, chunks, 2);
  png_set_unknown_chunks(png_ptr, info_ptr, chunks, 0);

  png_unknown_chunkp unknowns = nullptr;
  int count = png_get_unknown_chunks(png_ptr, info_ptr, &unknowns);
  if (count > 0 && unknowns != nullptr) {
    for (int i = 0; i < count; ++i) {
      volatile png_byte c0 = unknowns[i].name[0];
      volatile size_t sz = unknowns[i].size;
      volatile png_byte loc = unknowns[i].location;
      (void)c0;
      (void)sz;
      (void)loc;
      if (unknowns[i].data != nullptr && unknowns[i].size > 0) {
        volatile png_byte d0 = unknowns[i].data[0];
        (void)d0;
      }
    }
  }

  png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
  return 0;
}