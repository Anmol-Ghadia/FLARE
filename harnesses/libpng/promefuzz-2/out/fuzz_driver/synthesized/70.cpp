// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_keep_unknown_chunks at pngset.c:1372:1 in png.h
// png_set_keep_unknown_chunks at pngset.c:1372:1 in png.h
// png_set_keep_unknown_chunks at pngset.c:1372:1 in png.h
// png_set_keep_unknown_chunks at pngset.c:1372:1 in png.h
// png_set_keep_unknown_chunks at pngset.c:1372:1 in png.h
// png_set_read_fn at pngrio.c:87:1 in png.h
// png_read_info at pngread.c:92:1 in png.h
// png_get_image_height at pngget.c:61:1 in png.h
// png_set_expand at pngrtran.c:872:1 in png.h
// png_set_strip_16 at pngrtran.c:197:1 in png.h
// png_set_packing at pngtrans.c:50:1 in png.h
// png_set_gray_to_rgb at pngrtran.c:955:1 in png.h
// png_set_interlace_handling at pngtrans.c:99:1 in png.h
// png_read_update_info at pngread.c:267:1 in png.h
// png_get_rowbytes at pngget.c:30:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_read_image at pngread.c:700:1 in png.h
// png_read_end at pngread.c:766:1 in png.h
// png_get_chunk_cache_max at pngget.c:1208:1 in png.h
// png_get_chunk_malloc_max at pngget.c:1215:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_set_keep_unknown_chunks at pngset.c:1372:1 in png.h
// png_get_chunk_cache_max at pngget.c:1208:1 in png.h
// png_set_chunk_cache_max at pngset.c:1652:1 in png.h
// png_set_chunk_malloc_max at pngset.c:1660:1 in png.h
// png_read_end at pngread.c:766:1 in png.h
// png_get_chunk_malloc_max at pngget.c:1215:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_chunk_cache_max at pngset.c:1652:1 in png.h
// png_get_chunk_cache_max at pngget.c:1208:1 in png.h
// png_set_chunk_malloc_max at pngset.c:1660:1 in png.h
// png_get_chunk_malloc_max at pngget.c:1215:1 in png.h
#include <cstdio>
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <vector>
#include <png.h>

struct ReaderState {
  const uint8_t* data;
  size_t size;
  size_t offset;
};

static void PngReadCallback(png_structp png_ptr, png_bytep outBytes,
                            png_size_t byteCountToRead) {
  png_voidp io_ptr = png_get_io_ptr(png_ptr);
  if (io_ptr == NULL) {
    png_error(png_ptr, "no io_ptr");
    return;
  }

  ReaderState* st = reinterpret_cast<ReaderState*>(io_ptr);
  if (st->offset > st->size || byteCountToRead > st->size - st->offset) {
    png_error(png_ptr, "read overflow");
    return;
  }

  memcpy(outBytes, st->data + st->offset, byteCountToRead);
  st->offset += byteCountToRead;
}

static png_uint_32 ReadU32(const uint8_t* data, size_t size, size_t off) {
  if (off + 4 > size) return 0;
  return (static_cast<png_uint_32>(data[off]) << 24) |
         (static_cast<png_uint_32>(data[off + 1]) << 16) |
         (static_cast<png_uint_32>(data[off + 2]) << 8) |
         (static_cast<png_uint_32>(data[off + 3]));
}

extern "C" int LLVMFuzzerTestOneInput_70(const uint8_t *Data, size_t Size) {
  png_set_keep_unknown_chunks(NULL, 0, NULL, 0);
  (void)png_get_chunk_cache_max(NULL);
  png_set_chunk_cache_max(NULL, 0);
  png_set_chunk_malloc_max(NULL, 0);
  png_read_end(NULL, NULL);
  (void)png_get_chunk_malloc_max(NULL);

  FILE* fp = fopen("./dummy_file", "wb");
  if (fp != NULL) {
    if (Size > 0) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (png_ptr == NULL) return 0;

  png_infop info_ptr = png_create_info_struct(png_ptr);
  png_infop end_info_ptr = png_create_info_struct(png_ptr);

  if (info_ptr == NULL || end_info_ptr == NULL) {
    png_destroy_read_struct(&png_ptr, info_ptr ? &info_ptr : NULL,
                            end_info_ptr ? &end_info_ptr : NULL);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info_ptr);
    return 0;
  }

  int keep_values[] = {
      0, 1, 2, 3, 4, 5, -1, 99,
#ifdef PNG_HANDLE_CHUNK_AS_DEFAULT
      PNG_HANDLE_CHUNK_AS_DEFAULT,
#endif
#ifdef PNG_HANDLE_CHUNK_NEVER
      PNG_HANDLE_CHUNK_NEVER,
#endif
#ifdef PNG_HANDLE_CHUNK_IF_SAFE
      PNG_HANDLE_CHUNK_IF_SAFE,
#endif
#ifdef PNG_HANDLE_CHUNK_ALWAYS
      PNG_HANDLE_CHUNK_ALWAYS,
#endif
  };

  int keep = keep_values[(Size > 0 ? Data[0] : 0) %
                         (sizeof(keep_values) / sizeof(keep_values[0]))];

  png_uint_32 cache_max = ReadU32(Data, Size, 1);
  png_alloc_size_t malloc_max =
      static_cast<png_alloc_size_t>(ReadU32(Data, Size, 5));

  png_set_chunk_cache_max(png_ptr, cache_max);
  (void)png_get_chunk_cache_max(png_ptr);

  png_set_chunk_malloc_max(png_ptr, malloc_max);
  (void)png_get_chunk_malloc_max(png_ptr);

  std::vector<png_byte> chunk_list;
  int exact_chunks = 0;
  if (Size > 9) {
    size_t available = Size - 9;
    size_t chunk_names_bytes = (available / 4) * 4;
    size_t max_chunk_names_bytes = (64 / 5) * 4; /* keep resulting list <= 64 bytes */
    if (chunk_names_bytes > max_chunk_names_bytes)
      chunk_names_bytes = max_chunk_names_bytes;

    exact_chunks = static_cast<int>(chunk_names_bytes / 4);
    chunk_list.reserve(static_cast<size_t>(exact_chunks) * 5);

    for (int i = 0; i < exact_chunks; ++i) {
      const uint8_t* src = Data + 9 + static_cast<size_t>(i) * 4;
      chunk_list.push_back(src[0]);
      chunk_list.push_back(src[1]);
      chunk_list.push_back(src[2]);
      chunk_list.push_back(src[3]);
      chunk_list.push_back(0); /* handling byte placeholder */
    }
  }

  png_const_bytep chunk_ptr = chunk_list.empty() ? NULL : chunk_list.data();

  if (exact_chunks > 0 && chunk_ptr != NULL) {
    png_set_keep_unknown_chunks(png_ptr, keep, chunk_ptr, exact_chunks);

    int partial_chunks[] = {1, 2, 4, 8};
    for (size_t i = 0; i < sizeof(partial_chunks) / sizeof(partial_chunks[0]); ++i) {
      if (partial_chunks[i] <= exact_chunks) {
        png_set_keep_unknown_chunks(png_ptr, keep, chunk_ptr, partial_chunks[i]);
      }
    }
  }

  png_set_keep_unknown_chunks(png_ptr, keep, NULL, 0);
  png_set_keep_unknown_chunks(png_ptr, keep, chunk_ptr, -1);
  png_set_keep_unknown_chunks(png_ptr, keep, NULL, -3);

  ReaderState reader = {Data, Size, 0};
  png_set_read_fn(png_ptr, &reader, PngReadCallback);

  png_read_info(png_ptr, info_ptr);

  png_uint_32 height = png_get_image_height(png_ptr, info_ptr);

  png_set_expand(png_ptr);
  png_set_strip_16(png_ptr);
  png_set_packing(png_ptr);
  png_set_gray_to_rgb(png_ptr);
#ifdef PNG_READ_INTERLACING_SUPPORTED
  (void)png_set_interlace_handling(png_ptr);
#endif
  png_read_update_info(png_ptr, info_ptr);

  png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);
  if (height > 0 && rowbytes > 0) {
    const png_uint_32 kMaxRows = 1024;
    const png_size_t kMaxRowBytes = 1 << 20;

    if (height > kMaxRows) height = kMaxRows;
    if (rowbytes > kMaxRowBytes) rowbytes = kMaxRowBytes;

    if (static_cast<size_t>(height) <= (SIZE_MAX / sizeof(png_bytep)) &&
        static_cast<size_t>(height) <= (SIZE_MAX / rowbytes)) {
      std::vector<png_byte> image(static_cast<size_t>(height) * rowbytes);
      std::vector<png_bytep> rows(height);
      for (png_uint_32 y = 0; y < height; ++y) {
        rows[y] = image.data() + static_cast<size_t>(y) * rowbytes;
      }
      png_read_image(png_ptr, rows.data());
    }
  }

  png_read_end(png_ptr, end_info_ptr);

  (void)png_get_chunk_cache_max(png_ptr);
  (void)png_get_chunk_malloc_max(png_ptr);

  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info_ptr);
  return 0;
}