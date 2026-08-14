// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_keep_unknown_chunks at pngset.c:1372:1 in png.h
// png_set_unknown_chunks at pngset.c:1200:1 in png.h
// png_set_unknown_chunks at pngset.c:1200:1 in png.h
// png_get_unknown_chunks at pngget.c:1139:1 in png.h
// png_get_unknown_chunks at pngget.c:1139:1 in png.h
// png_get_unknown_chunks at pngget.c:1139:1 in png.h
// png_get_unknown_chunks at pngget.c:1139:1 in png.h
// png_set_unknown_chunk_location at pngset.c:1299:1 in png.h
// png_set_unknown_chunk_location at pngset.c:1299:1 in png.h
// png_set_unknown_chunk_location at pngset.c:1299:1 in png.h
// png_handle_as_unknown at png.c:926:1 in png.h
// png_handle_as_unknown at png.c:926:1 in png.h
// png_handle_as_unknown at png.c:926:1 in png.h
// png_handle_as_unknown at png.c:926:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_read_user_chunk_fn at pngset.c:1531:1 in png.h
// png_set_keep_unknown_chunks at pngset.c:1372:1 in png.h
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
#include <vector>
#include <cstring>
#include <cstdio>
#include <setjmp.h>

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

static int UserChunkCallback(png_structp, png_unknown_chunkp chunk) {
  if (chunk == nullptr)
    return 0;
  unsigned v = 0;
  if (chunk->size > 0 && chunk->data != nullptr)
    v = chunk->data[0];
  if ((chunk->name[0] & 1) != 0)
    return -1;
  return static_cast<int>(v % 3) - 1; // -1,0,1
}

static png_byte MakeChunkNameChar(uint8_t c, bool upper) {
  c = static_cast<uint8_t>('A' + (c % 26));
  if (!upper)
    c = static_cast<uint8_t>(c - 'A' + 'a');
  return c;
}

extern "C" int LLVMFuzzerTestOneInput_44(const uint8_t *Data, size_t Size) {
  FILE *fp = std::fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    if (Data != nullptr && Size > 0)
      std::fwrite(Data, 1, Size, fp);
    std::fclose(fp);
  }

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn, PngWarningFn);
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

  png_set_read_user_chunk_fn(png_ptr,
                             const_cast<uint8_t*>(Data),
                             UserChunkCallback);

  int keep_modes[] = {
      PNG_HANDLE_CHUNK_AS_DEFAULT,
      PNG_HANDLE_CHUNK_NEVER,
      PNG_HANDLE_CHUNK_IF_SAFE,
      PNG_HANDLE_CHUNK_ALWAYS,
      -1,
      99
  };

  int keep = keep_modes[0];
  if (Size > 0)
    keep = keep_modes[Data[0] % (sizeof(keep_modes) / sizeof(keep_modes[0]))];

  png_byte chunk_list_buf[20];
  int chunk_list_count = 0;
  if (Size > 1) {
    chunk_list_count = static_cast<int>((Data[1] % 5));
    for (int i = 0; i < chunk_list_count; ++i) {
      size_t base = 2 + static_cast<size_t>(i) * 4;
      for (int j = 0; j < 4; ++j) {
        uint8_t src = (base + j < Size) ? Data[base + j] : static_cast<uint8_t>(i * 4 + j);
        chunk_list_buf[i * 4 + j] = MakeChunkNameChar(src, (src & 1) == 0);
      }
    }
  }

  png_set_keep_unknown_chunks(
      png_ptr, keep,
      chunk_list_count > 0 ? chunk_list_buf : nullptr,
      chunk_list_count);

  if (Size > 2) {
    int neg_num = -static_cast<int>(Data[2] % 4);
    png_set_keep_unknown_chunks(
        png_ptr, PNG_HANDLE_CHUNK_ALWAYS,
        chunk_list_count > 0 ? chunk_list_buf : nullptr,
        neg_num);
  }

  std::vector<png_unknown_chunk> chunks;
  std::vector<std::vector<png_byte>> storage;

  int num_unknowns = 0;
  if (Size > 3)
    num_unknowns = static_cast<int>(Data[3] % 6);

  chunks.resize(num_unknowns);
  storage.resize(num_unknowns);

  size_t off = 4;
  for (int i = 0; i < num_unknowns; ++i) {
    uint8_t n0 = (off < Size) ? Data[off++] : static_cast<uint8_t>(i);
    uint8_t n1 = (off < Size) ? Data[off++] : static_cast<uint8_t>(i + 1);
    uint8_t n2 = (off < Size) ? Data[off++] : static_cast<uint8_t>(i + 2);
    uint8_t n3 = (off < Size) ? Data[off++] : static_cast<uint8_t>(i + 3);

    chunks[i].name[0] = MakeChunkNameChar(n0, true);
    chunks[i].name[1] = MakeChunkNameChar(n1, false);
    chunks[i].name[2] = MakeChunkNameChar(n2, true);
    chunks[i].name[3] = MakeChunkNameChar(n3, false);
    chunks[i].name[4] = 0;

    size_t len = 0;
    if (off < Size)
      len = Data[off++] % 16;

    storage[i].resize(len);
    for (size_t j = 0; j < len; ++j)
      storage[i][j] = (off < Size) ? Data[off++] : static_cast<uint8_t>(j);

    chunks[i].data = storage[i].empty() ? nullptr : storage[i].data();
    chunks[i].size = storage[i].size();
    chunks[i].location = (off < Size) ? Data[off++] : static_cast<png_byte>(i);
  }

  if (!chunks.empty())
    png_set_unknown_chunks(png_ptr, info_ptr, chunks.data(), num_unknowns);
  else
    png_set_unknown_chunks(png_ptr, info_ptr, nullptr, 0);

  png_unknown_chunkp unknowns = nullptr;
  int count = png_get_unknown_chunks(png_ptr, info_ptr, &unknowns);

  png_get_unknown_chunks(nullptr, info_ptr, &unknowns);
  png_get_unknown_chunks(png_ptr, nullptr, &unknowns);
  png_get_unknown_chunks(png_ptr, info_ptr, nullptr);

  for (int i = 0; i < count; ++i) {
    int location = (off < Size) ? static_cast<int>(Data[off++]) : i;
    png_set_unknown_chunk_location(png_ptr, info_ptr, i, location);
  }

  if (count > 0) {
    png_set_unknown_chunk_location(png_ptr, info_ptr, 0, -1);
    png_set_unknown_chunk_location(png_ptr, info_ptr, count - 1, 0x7fffffff);
  }

  for (int i = 0; i < count; ++i) {
    png_byte namebuf[5] = {0, 0, 0, 0, 0};
    if (unknowns != nullptr) {
      std::memcpy(namebuf, unknowns[i].name, 4);
      namebuf[4] = 0;
      (void)png_handle_as_unknown(png_ptr, namebuf);
    }
  }

  if (chunk_list_count > 0) {
    for (int i = 0; i < chunk_list_count; ++i)
      (void)png_handle_as_unknown(png_ptr, &chunk_list_buf[i * 4]);
  }

  png_byte random_name[5] = {'a', 'B', 'c', 'D', 0};
  (void)png_handle_as_unknown(png_ptr, random_name);
  (void)png_handle_as_unknown(png_ptr, nullptr);

  png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
  return 0;
}