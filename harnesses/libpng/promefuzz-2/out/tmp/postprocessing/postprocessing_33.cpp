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
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <setjmp.h>

struct FuzzState {
  int callback_mode;
};

static void FuzzErrorFn(png_structp png_ptr, png_const_charp) {
  jmp_buf* jb = reinterpret_cast<jmp_buf*>(png_get_error_ptr(png_ptr));
  if (jb) longjmp(*jb, 1);
}

static void FuzzWarningFn(png_structp, png_const_charp) {
}

static int FuzzUserChunkFn(png_structp png_ptr, png_unknown_chunkp chunk) {
  FuzzState* state =
      reinterpret_cast<FuzzState*>(png_get_user_chunk_ptr(png_ptr));
  if (state == nullptr || chunk == nullptr) return 0;

  switch (state->callback_mode % 3) {
    case 0:
      return 0;
    case 1:
      return 1;
    default:
      return -1;
  }
}

static uint32_t ReadU32(const uint8_t* data, size_t size, size_t& off) {
  uint32_t v = 0;
  for (int i = 0; i < 4; ++i) {
    v <<= 8;
    if (off < size) v |= data[off++];
  }
  return v;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE* fp = std::fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    if (Size > 0) std::fwrite(Data, 1, Size, fp);
    std::fclose(fp);
  }

  jmp_buf png_jmpbuf_local;
  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                               &png_jmpbuf_local,
                                               FuzzErrorFn,
                                               FuzzWarningFn);
  if (png_ptr == nullptr) return 0;

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr) {
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    return 0;
  }

  if (setjmp(png_jmpbuf_local)) {
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    return 0;
  }

  size_t off = 0;

  FuzzState state;
  state.callback_mode = (Size > 0) ? Data[off++ % Size] : 0;
  png_set_read_user_chunk_fn(png_ptr, &state, FuzzUserChunkFn);

  int keep_values[] = {
      PNG_HANDLE_CHUNK_AS_DEFAULT,
      PNG_HANDLE_CHUNK_NEVER,
      PNG_HANDLE_CHUNK_IF_SAFE,
      PNG_HANDLE_CHUNK_ALWAYS,
      -1,
      127
  };

  int keep = keep_values[(Size > 0 ? Data[off++ % Size] : 0) %
                         (sizeof(keep_values) / sizeof(keep_values[0]))];

  std::vector<png_byte> chunk_list_storage;
  int num_chunks_for_policy = 0;
  if (off < Size) {
    num_chunks_for_policy = static_cast<int>(Data[off++] % 8);
    size_t needed = static_cast<size_t>(num_chunks_for_policy) * 5;
    chunk_list_storage.resize(needed);
    for (size_t i = 0; i < needed; ++i) {
      chunk_list_storage[i] = (off < Size) ? Data[off++] : 0;
    }
  }

  png_const_bytep chunk_list_ptr =
      chunk_list_storage.empty() ? nullptr : chunk_list_storage.data();

  png_set_keep_unknown_chunks(png_ptr, keep, chunk_list_ptr,
                              num_chunks_for_policy);
  png_set_keep_unknown_chunks(png_ptr, keep, nullptr, 0);
  if (!chunk_list_storage.empty()) {
    png_set_keep_unknown_chunks(png_ptr, keep, chunk_list_storage.data(), -1);
  }

  png_byte query_name[5] = {0, 0, 0, 0, 0};
  for (int i = 0; i < 4; ++i) {
    query_name[i] = (off < Size) ? Data[off++] : 0;
  }
  (void)png_handle_as_unknown(png_ptr, query_name);
  (void)png_handle_as_unknown(png_ptr, reinterpret_cast<png_const_bytep>("IHDR"));
  (void)png_handle_as_unknown(png_ptr, reinterpret_cast<png_const_bytep>("tEXt"));

  int num_unknowns = 0;
  if (off < Size) {
    num_unknowns = static_cast<int>(Data[off++] % 4);
  }

  std::vector<png_unknown_chunk> unknowns;
  std::vector<std::vector<png_byte>> payloads;
  unknowns.resize(num_unknowns);
  payloads.resize(num_unknowns);

  for (int i = 0; i < num_unknowns; ++i) {
    for (int j = 0; j < 4; ++j) {
      unknowns[i].name[j] = (off < Size) ? Data[off++] : 0;
    }
    unknowns[i].name[4] = 0;

    uint32_t payload_size = ReadU32(Data, Size, off) % 32;
    payloads[i].resize(payload_size);
    for (uint32_t j = 0; j < payload_size; ++j) {
      payloads[i][j] = (off < Size) ? Data[off++] : 0;
    }

    unknowns[i].data = payloads[i].empty() ? nullptr : payloads[i].data();
    unknowns[i].size = payloads[i].size();

    int loc_selector = (off < Size) ? Data[off++] % 6 : 0;
    switch (loc_selector) {
      case 0:
        unknowns[i].location = PNG_HAVE_IHDR;
        break;
      case 1:
        unknowns[i].location = PNG_HAVE_PLTE;
        break;
      case 2:
        unknowns[i].location = PNG_AFTER_IDAT;
        break;
      case 3:
        unknowns[i].location = static_cast<png_byte>(PNG_HAVE_IHDR | PNG_HAVE_PLTE);
        break;
      case 4:
        unknowns[i].location = 0;
        break;
      default:
        unknowns[i].location = 0xFF;
        break;
    }
  }

  if (!unknowns.empty()) {
    png_set_unknown_chunks(png_ptr, info_ptr, unknowns.data(),
                           static_cast<int>(unknowns.size()));
  }

  for (int i = 0; i < num_unknowns; ++i) {
    int loc = (off < Size) ? static_cast<int>(Data[off++]) : 0;
    png_set_unknown_chunk_location(png_ptr, info_ptr, i, loc);
  }

  if (num_unknowns > 0) {
    png_set_unknown_chunk_location(png_ptr, info_ptr, -1, PNG_HAVE_IHDR);
    png_set_unknown_chunk_location(png_ptr, info_ptr, num_unknowns, PNG_AFTER_IDAT);
  }

  png_unknown_chunkp returned_unknowns = nullptr;
  (void)png_get_unknown_chunks(png_ptr, info_ptr, &returned_unknowns);

  png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
  return 0;
}