// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_write_chunk_start at pngwutil.c:115:1 in png.h
// png_write_chunk at pngwutil.c:192:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_write_chunk at pngwutil.c:192:1 in png.h
// png_write_chunk at pngwutil.c:192:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_save_uint_32 at pngwutil.c:24:1 in png.h
// png_save_uint_16 at pngwutil.c:37:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_write_fn at pngwio.c:122:1 in png.h
// png_write_sig at pngwutil.c:51:1 in png.h
#include <png.h>

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <vector>

static void PngWriteCallback(png_structp png_ptr, png_bytep data, png_size_t length) {
  std::vector<png_byte>* out =
      reinterpret_cast<std::vector<png_byte>*>(png_get_io_ptr(png_ptr));
  if (out == nullptr || data == nullptr) return;
  out->insert(out->end(), data, data + length);
}

static void PngFlushCallback(png_structp) {}

static png_uint_32 ReadU32(const uint8_t* data, size_t size, size_t& off) {
  png_uint_32 v = 0;
  for (int i = 0; i < 4; ++i) {
    v <<= 8;
    if (off < size) v |= data[off++];
  }
  return v;
}

static unsigned int ReadU16(const uint8_t* data, size_t size, size_t& off) {
  unsigned int v = 0;
  for (int i = 0; i < 2; ++i) {
    v <<= 8;
    if (off < size) v |= data[off++];
  }
  return v;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* Data, size_t Size) {
  unsigned char buf32[4] = {0, 0, 0, 0};
  unsigned char buf16[2] = {0, 0};

  size_t off = 0;
  png_uint_32 v32 = ReadU32(Data, Size, off);
  unsigned int v16 = ReadU16(Data, Size, off);

  png_save_uint_32(buf32, v32);
  png_save_uint_16(buf16, v16);
  (void)png_get_uint_16(buf16);
  if (Size >= 2) {
    (void)png_get_uint_16(reinterpret_cast<png_const_bytep>(Data));
  }

  FILE* fp = std::fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    if (Size > 0) std::fwrite(Data, 1, Size, fp);
    std::fclose(fp);
  }

  png_structp png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (png_ptr == nullptr) return 0;

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr) {
    png_destroy_write_struct(&png_ptr, nullptr);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    return 0;
  }

  std::vector<png_byte> out;
  png_set_write_fn(png_ptr, &out, PngWriteCallback, PngFlushCallback);

  png_write_sig(png_ptr);

  png_byte chunk_name1[4] = {'t', 'E', 'S', 'T'};
  png_byte chunk_name2[4] = {'f', 'U', 'Z', 'Z'};

  if (Size >= off + 4) {
    for (int i = 0; i < 4; ++i) {
      png_byte c = static_cast<png_byte>(Data[off++]);
      if (c < 32 || c > 126) c = static_cast<png_byte>('A' + (c % 26));
      chunk_name1[i] = c;
    }
  }

  if (Size >= off + 4) {
    for (int i = 0; i < 4; ++i) {
      png_byte c = static_cast<png_byte>(Data[off++]);
      if (c < 32 || c > 126) c = static_cast<png_byte>('a' + (c % 26));
      chunk_name2[i] = c;
    }
  }

  size_t remaining = off <= Size ? Size - off : 0;
  png_uint_32 len1_raw = ReadU32(Data, Size, off);
  remaining = off <= Size ? Size - off : 0;
  size_t len1 = remaining == 0 ? 0 : static_cast<size_t>(len1_raw % (remaining + 1));

  png_write_chunk_start(png_ptr, chunk_name1, static_cast<png_uint_32>(len1));

  const png_const_bytep data1 =
      len1 > 0 ? reinterpret_cast<png_const_bytep>(Data + off) : nullptr;
  png_write_chunk(png_ptr, chunk_name1, data1, len1);

  size_t off2 = off + len1;
  if (off2 > Size) off2 = Size;

  png_uint_32 len2_raw = ReadU32(Data, Size, off2);
  size_t remaining2 = off2 <= Size ? Size - off2 : 0;
  size_t len2 = remaining2 == 0 ? 0 : static_cast<size_t>(len2_raw % (remaining2 + 1));

  const png_const_bytep data2 =
      len2 > 0 ? reinterpret_cast<png_const_bytep>(Data + off2) : nullptr;
  png_write_chunk(png_ptr, chunk_name2, data2, len2);

  static const png_byte iend_name[4] = {'I', 'E', 'N', 'D'};
  png_write_chunk(png_ptr, iend_name, nullptr, 0);

  png_destroy_write_struct(&png_ptr, &info_ptr);
  return 0;
}