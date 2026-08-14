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

static void PngWriteCallback(png_structp png_ptr, png_bytep data, png_size_t length) {
  std::vector<unsigned char>* out =
      reinterpret_cast<std::vector<unsigned char>*>(png_get_io_ptr(png_ptr));
  if (out == nullptr || data == nullptr) return;
  out->insert(out->end(), data, data + length);
}

static void PngFlushCallback(png_structp) {
}

static png_uint_32 ReadU32(const uint8_t* p) {
  return (static_cast<png_uint_32>(p[0]) << 24) |
         (static_cast<png_uint_32>(p[1]) << 16) |
         (static_cast<png_uint_32>(p[2]) << 8) |
         static_cast<png_uint_32>(p[3]);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  unsigned char buf32[4] = {0, 0, 0, 0};
  unsigned char buf16[2] = {0, 0};

  png_uint_32 v32 = 0;
  unsigned int v16in = 0;

  if (Size >= 4) v32 = ReadU32(Data);
  else {
    for (size_t i = 0; i < Size; ++i) buf32[i] = Data[i];
    v32 = ReadU32(buf32);
  }

  if (Size >= 2) v16in = (static_cast<unsigned int>(Data[0]) << 8) | Data[1];
  else if (Size == 1) v16in = static_cast<unsigned int>(Data[0]) << 8;

  png_save_uint_32(buf32, v32);
  (void)png_get_uint_16(buf32);
  png_save_uint_16(buf16, v16in);
  (void)png_get_uint_16(buf16);

  FILE* fp = std::fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    if (Size > 0) std::fwrite(Data, 1, Size, fp);
    std::fclose(fp);
  }

  png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
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

  if (Size > 0) {
    png_ptr->sig_bytes = static_cast<int>(Data[0] % 8);
  } else {
    png_ptr->sig_bytes = 0;
  }

  png_write_sig(png_ptr);

  unsigned char chunk_name[4] = {'t', 'E', 'S', 'T'};
  if (Size >= 8) {
    chunk_name[0] = Data[4];
    chunk_name[1] = Data[5];
    chunk_name[2] = Data[6];
    chunk_name[3] = Data[7];
  } else if (Size >= 4) {
    chunk_name[0] = Data[0];
    chunk_name[1] = Data[1];
    chunk_name[2] = Data[2];
    chunk_name[3] = Data[3];
  }

  const png_bytep chunk_data =
      Size > 8 ? reinterpret_cast<const png_bytep>(Data + 8)
               : reinterpret_cast<const png_bytep>(Data);
  size_t available = Size > 8 ? Size - 8 : Size;

  png_uint_32 start_len = 0;
  if (Size >= 12) {
    start_len = ReadU32(Data + 8);
  } else if (available <= 32) {
    start_len = static_cast<png_uint_32>(available);
  } else {
    start_len = static_cast<png_uint_32>(available % 32);
  }

  png_write_chunk_start(png_ptr, chunk_name, start_len);

  size_t chunk_len = available;
  if (Size > 0) {
    switch (Data[0] % 4) {
      case 0:
        chunk_len = 0;
        break;
      case 1:
        chunk_len = available > 16 ? 16 : available;
        break;
      case 2:
        chunk_len = available;
        break;
      default:
        chunk_len = available > 64 ? 64 : available;
        break;
    }
  }

  png_write_chunk(png_ptr, chunk_name, chunk_data, chunk_len);

  if (available >= 4) {
    png_uint_32 alt32 = ReadU32(chunk_data);
    png_save_uint_32(buf32, alt32);
    (void)png_get_uint_16(buf32);
  }

  if (available >= 2) {
    png_save_uint_16(buf16,
                     (static_cast<unsigned int>(chunk_data[0]) << 8) |
                     static_cast<unsigned int>(chunk_data[1]));
    (void)png_get_uint_16(buf16);
  }

  png_destroy_write_struct(&png_ptr, &info_ptr);
  return 0;
}