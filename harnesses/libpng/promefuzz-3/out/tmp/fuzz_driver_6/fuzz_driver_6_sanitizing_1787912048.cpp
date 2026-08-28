#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <png.h>

struct BufferState {
  const uint8_t* data;
  size_t size;
  size_t offset;
};

static void PngReadCallback(png_structp png_ptr, png_bytep outBytes,
                            png_size_t byteCountToRead) {
  BufferState* st =
      static_cast<BufferState*>(png_get_io_ptr(png_ptr));
  if (st == nullptr) {
    png_error(png_ptr, "no io_ptr");
    return;
  }

  if (st->offset > st->size || byteCountToRead > st->size - st->offset) {
    png_error(png_ptr, "read overflow");
    return;
  }

  if (byteCountToRead > 0) {
    memcpy(outBytes, st->data + st->offset, byteCountToRead);
    st->offset += byteCountToRead;
  }
}

static int UserChunkCallback(png_structp png_ptr, png_unknown_chunkp chunk) {
  png_voidp user_ptr = png_get_user_chunk_ptr(png_ptr);
  int mode = 0;
  if (user_ptr != nullptr) {
    mode = *static_cast<int*>(user_ptr);
  }

  if (chunk == nullptr) return 0;

  size_t sum = 0;
  for (size_t i = 0; i < chunk->size; ++i) sum += chunk->data[i];

  switch (mode % 4) {
    case 0:
      return 0;
    case 1:
      return 1;
    case 2:
      return (sum & 1) ? 1 : 0;
    default:
      return 0;
  }
}

static void SilentErrorFn(png_structp, png_const_charp) {}
static void SilentWarningFn(png_structp, png_const_charp) {}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE* fp = fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    if (Size > 0) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  png_structp png_ptr = png_create_read_struct(
      PNG_LIBPNG_VER_STRING, nullptr, SilentErrorFn, SilentWarningFn);
  if (png_ptr == nullptr) {
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  png_infop end_info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr || end_info_ptr == nullptr) {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info_ptr);
    return 0;
  }

  BufferState state = {Data, Size, 0};
  int callback_mode = (Size > 0) ? (Data[0] & 0x7f) : 0;

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_unknown_chunkp entries = nullptr;
    (void)png_get_unknown_chunks(png_ptr, info_ptr, &entries);
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info_ptr);
    return 0;
  }

  png_set_read_fn(png_ptr, &state, PngReadCallback);
  png_set_read_user_chunk_fn(png_ptr, &callback_mode, UserChunkCallback);

  int keep_values[] = {
      PNG_HANDLE_CHUNK_AS_DEFAULT,
      PNG_HANDLE_CHUNK_NEVER,
      PNG_HANDLE_CHUNK_IF_SAFE,
      PNG_HANDLE_CHUNK_ALWAYS
  };
  int keep = keep_values[(Size > 1) ? (Data[1] % 4) : 0];

  png_byte chunk_list[12];
  memset(chunk_list, 0, sizeof(chunk_list));
  for (size_t i = 0; i < sizeof(chunk_list) && i + 2 < Size; ++i) {
    uint8_t c = Data[i + 2];
    if ((c < 'A' || c > 'Z') && (c < 'a' || c > 'z')) {
      c = static_cast<uint8_t>('A' + (c % 26));
    }
    chunk_list[i] = c;
  }

  int num_chunks_choice = 0;
  if (Size > 14) {
    num_chunks_choice = static_cast<int>(static_cast<int8_t>(Data[14]));
    if (num_chunks_choice > 3) num_chunks_choice = 3;
    if (num_chunks_choice < -1) num_chunks_choice = -1;
  }

  png_set_keep_unknown_chunks(png_ptr, keep, chunk_list, num_chunks_choice);

  png_byte query_name[5] = {'v', 'p', 'A', 'g', 0};
  (void)png_handle_as_unknown(png_ptr, query_name);

  png_read_info(png_ptr, info_ptr);

  png_uint_32 width = png_get_image_width(png_ptr, info_ptr);
  png_uint_32 height = png_get_image_height(png_ptr, info_ptr);

  if (width > 0 && height > 0 && width < 4096 && height < 4096) {
    png_read_update_info(png_ptr, info_ptr);
    png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);

    if (rowbytes > 0 && rowbytes < (1u << 20)) {
      std::vector<png_byte> image;
      if (height <= (1u << 20) / rowbytes) {
        image.resize(static_cast<size_t>(rowbytes) * height);
        std::vector<png_bytep> rows(height);
        for (png_uint_32 y = 0; y < height; ++y) {
          rows[y] = image.data() + static_cast<size_t>(y) * rowbytes;
        }
        png_read_image(png_ptr, rows.data());
      }
    }
  }

  png_read_end(png_ptr, end_info_ptr);

  png_unknown_chunkp entries = nullptr;
  (void)png_get_unknown_chunks(png_ptr, info_ptr, &entries);
  (void)png_get_unknown_chunks(png_ptr, end_info_ptr, &entries);

  png_unknown_chunk local_chunk;
  memset(&local_chunk, 0, sizeof(local_chunk));
  memcpy(local_chunk.name, "vpAg", 5);
  local_chunk.location = PNG_HAVE_IHDR;

  png_byte local_data[64];
  size_t local_size = 0;
  if (Size > 15) {
    local_size = Size - 15;
    if (local_size > sizeof(local_data)) local_size = sizeof(local_data);
    if (local_size > 0) {
      memcpy(local_data, Data + 15, local_size);
      local_chunk.data = local_data;
      local_chunk.size = local_size;
      png_set_unknown_chunks(png_ptr, info_ptr, &local_chunk, 1);
    }
  }

  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info_ptr);
  return 0;
}