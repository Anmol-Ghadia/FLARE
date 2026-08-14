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
#include <cstring>
#include <vector>

static void PngWriteCallback(png_structp png_ptr, png_bytep data, png_size_t length) {
  std::vector<unsigned char>* out =
      reinterpret_cast<std::vector<unsigned char>*>(png_get_io_ptr(png_ptr));
  if (out == nullptr || data == nullptr) return;
  out->insert(out->end(), data, data + length);
}

static void PngFlushCallback(png_structp) {
}

static png_uint_32 ReadU32(const uint8_t* data, size_t size, size_t& off) {
  png_uint_32 v = 0;
  for (int i = 0; i < 4; ++i) {
    v <<= 8;
    if (off < size) v |= data[off++];
  }
  return v;
}

static unsigned int ReadU16Like(const uint8_t* data, size_t size, size_t& off) {
  unsigned int v = 0;
  for (int i = 0; i < 2; ++i) {
    v <<= 8;
    if (off < size) v |= data[off++];
  }
  return v;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  unsigned char buf32[4] = {0, 0, 0, 0};
  unsigned char buf16[2] = {0, 0};

  size_t off = 0;
  png_uint_32 v32 = ReadU32(Data, Size, off);
  unsigned int v16in = ReadU16Like(Data, Size, off);

  png_save_uint_32(buf32, v32);
  png_save_uint_16(buf16, v16in);
  (void)png_get_uint_16(buf16);

  FILE* fp = std::fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    if (Size > 0) std::fwrite(Data, 1, Size, fp);
    std::fclose(fp);
  }

  png_structp png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (png_ptr == nullptr) {
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr) {
    png_destroy_write_struct(&png_ptr, nullptr);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    return 0;
  }

  std::vector<unsigned char> out;
  png_set_write_fn(png_ptr, &out, PngWriteCallback, PngFlushCallback);

  if (Size > off) {
    png_ptr->sig_bytes = static_cast<int>(Data[off++] % 8);
  } else {
    png_ptr->sig_bytes = 0;
  }

  png_write_sig(png_ptr);

  unsigned char chunk_name[5] = {'t', 'E', 'S', 'T', 0};
  if (Size >= off + 4) {
    for (int i = 0; i < 4; ++i) {
      unsigned char c = Data[off++];
      if (c < 32 || c > 126) c = static_cast<unsigned char>('A' + (c % 26));
      chunk_name[i] = c;
    }
  }

  size_t remaining = (off <= Size) ? (Size - off) : 0;
  png_uint_32 chunk_len32 = ReadU32(Data, Size, off);
  size_t chunk_len = remaining;
  if (remaining > 0) {
    chunk_len = static_cast<size_t>(chunk_len32 % (remaining + 1));
  } else {
    chunk_len = 0;
  }

  const png_bytep chunk_data =
      chunk_len > 0 ? reinterpret_cast<const png_bytep>(Data + off) : nullptr;

  png_write_chunk_start(png_ptr, chunk_name, static_cast<png_uint_32>(chunk_len));

  if (Size > 0) {
    size_t alt_off = off;
    png_uint_32 alt_len32 = ReadU32(Data, Size, alt_off);
    size_t alt_remaining = (alt_off <= Size) ? (Size - alt_off) : 0;
    size_t alt_len = alt_remaining;
    if (alt_remaining > 0) {
      alt_len = static_cast<size_t>(alt_len32 % (alt_remaining + 1));
    } else {
      alt_len = 0;
    }

    unsigned char alt_name[5] = {'f', 'U', 'Z', 'Z', 0};
    if (Size >= alt_off + 4) {
      for (int i = 0; i < 4; ++i) {
        unsigned char c = Data[alt_off++];
        if (c < 32 || c > 126) c = static_cast<unsigned char>('a' + (c % 26));
        alt_name[i] = c;
      }
    }

    const png_bytep alt_data =
        alt_len > 0 ? reinterpret_cast<const png_bytep>(Data + alt_off) : nullptr;
    png_write_chunk(png_ptr, alt_name, alt_data, alt_len);
  } else {
    png_write_chunk(png_ptr,
                    reinterpret_cast<const png_bytep>("IEND"),
                    nullptr, 0);
  }

  png_destroy_write_struct(&png_ptr, &info_ptr);
  return 0;
}