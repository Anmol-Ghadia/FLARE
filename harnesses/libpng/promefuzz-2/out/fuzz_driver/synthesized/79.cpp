// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_init_io at png.c:703:1 in png.h
// png_set_IHDR at pngset.c:254:1 in png.h
// png_set_oFFs at pngset.c:295:1 in png.h
// png_set_pCAL at pngset.c:312:1 in png.h
// png_write_info at pngwrite.c:192:1 in png.h
// png_write_chunk at pngwutil.c:192:1 in png.h
// png_write_chunk at pngwutil.c:192:1 in png.h
// png_write_row at pngwrite.c:693:1 in png.h
// png_write_end at pngwrite.c:358:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_save_int_32 at png.c:726:1 in png.h
// png_save_int_32 at png.c:726:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
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
#include <vector>
#include <string>
#include <cstring>

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

static png_uint_32 ReadU32(const uint8_t *data, size_t size, size_t &off) {
  if (off + 4 > size) return 0;
  png_uint_32 v = (static_cast<png_uint_32>(data[off]) << 24) |
                  (static_cast<png_uint_32>(data[off + 1]) << 16) |
                  (static_cast<png_uint_32>(data[off + 2]) << 8) |
                  (static_cast<png_uint_32>(data[off + 3]));
  off += 4;
  return v;
}

static int ReadI32(const uint8_t *data, size_t size, size_t &off) {
  return static_cast<int>(ReadU32(data, size, off));
}

static uint8_t ReadU8(const uint8_t *data, size_t size, size_t &off) {
  if (off >= size) return 0;
  return data[off++];
}

static std::string ReadString(const uint8_t *data, size_t size, size_t &off, size_t max_len) {
  size_t len = 0;
  if (off < size) len = ReadU8(data, size, off) % (max_len + 1);
  if (off + len > size) len = size - off;
  std::string s(reinterpret_cast<const char*>(data + off), len);
  off += len;
  for (size_t i = 0; i < s.size(); ++i) {
    if (s[i] == '\0') s[i] = 'A';
  }
  return s;
}

extern "C" int LLVMFuzzerTestOneInput_79(const uint8_t *Data, size_t Size) {
  if (Data == nullptr) return 0;

  size_t off = 0;

  // Exercise png_get_int_32 on multiple offsets/buffers.
  if (Size >= 4) {
    for (size_t i = 0; i + 4 <= Size && i < 16; ++i) {
      (void)png_get_int_32(reinterpret_cast<png_const_bytep>(Data + i));
    }
  } else {
    uint8_t tmp[4] = {0, 0, 0, 0};
    if (Size > 0) memcpy(tmp, Data, Size);
    (void)png_get_int_32(tmp);
  }

  // Exercise png_save_int_32 with values derived from input.
  uint8_t out32[4] = {0, 0, 0, 0};
  png_int_32 val1 = static_cast<png_int_32>(ReadI32(Data, Size, off));
  png_save_int_32(out32, val1);
  (void)png_get_int_32(out32);

  png_int_32 val2 = static_cast<png_int_32>(0x80000000u | (Size ? Data[0] : 0));
  png_save_int_32(out32, val2);
  (void)png_get_int_32(out32);

  // Write input to required dummy file.
  FILE *fp = fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    if (Size > 0) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  fp = fopen("./dummy_file", "wb+");
  if (fp == nullptr) return 0;

  png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr,
                                                PngErrorFn, PngWarningFn);
  if (png_ptr == nullptr) {
    fclose(fp);
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr) {
    png_destroy_write_struct(&png_ptr, nullptr);
    fclose(fp);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
    return 0;
  }

  png_init_io(png_ptr, fp);

  // Build a minimal valid PNG configuration for png_write_info.
  int width = static_cast<int>((ReadU8(Data, Size, off) % 8) + 1);
  int height = static_cast<int>((ReadU8(Data, Size, off) % 8) + 1);
  int color_choice = ReadU8(Data, Size, off) % 3;
  int color_type = PNG_COLOR_TYPE_GRAY;
  if (color_choice == 1) color_type = PNG_COLOR_TYPE_RGB;
  else if (color_choice == 2) color_type = PNG_COLOR_TYPE_RGBA;

  png_set_IHDR(png_ptr, info_ptr,
               static_cast<png_uint_32>(width),
               static_cast<png_uint_32>(height),
               8,
               color_type,
               PNG_INTERLACE_NONE,
               PNG_COMPRESSION_TYPE_BASE,
               PNG_FILTER_TYPE_BASE);

  // Exercise png_set_oFFs.
  png_int_32 offset_x = static_cast<png_int_32>(ReadI32(Data, Size, off));
  png_int_32 offset_y = static_cast<png_int_32>(ReadI32(Data, Size, off));
  int unit_type = ReadU8(Data, Size, off) % 3;
  png_set_oFFs(png_ptr, info_ptr, offset_x, offset_y, unit_type);

  // Exercise png_set_pCAL with controlled valid pointers and type range 0..3.
  std::string purpose = ReadString(Data, Size, off, 32);
  if (purpose.empty()) purpose = "p";
  png_int_32 X0 = static_cast<png_int_32>(ReadI32(Data, Size, off));
  png_int_32 X1 = static_cast<png_int_32>(ReadI32(Data, Size, off));
  int pcal_type = ReadU8(Data, Size, off) % 4;
  int nparams = ReadU8(Data, Size, off) % 4;
  std::string units = ReadString(Data, Size, off, 16);
  if (units.empty()) units = "u";

  std::vector<std::string> param_storage;
  std::vector<char*> params;
  param_storage.reserve(nparams);
  params.reserve(nparams);
  for (int i = 0; i < nparams; ++i) {
    std::string p = ReadString(Data, Size, off, 16);
    if (p.empty()) p = "1";
    param_storage.push_back(p);
  }
  for (int i = 0; i < nparams; ++i) {
    params.push_back(const_cast<char*>(param_storage[i].c_str()));
  }

  png_set_pCAL(png_ptr, info_ptr,
               purpose.c_str(),
               X0, X1,
               pcal_type,
               nparams,
               units.c_str(),
               nparams ? params.data() : nullptr);

  // Exercise png_write_info.
  png_write_info(png_ptr, info_ptr);

  // Exercise png_write_chunk with diverse chunk names and payloads.
  uint8_t chunk_name1[4] = {'f', 'U', 'Z', 'Z'};
  size_t chunk_len1 = 0;
  if (off < Size) chunk_len1 = ReadU8(Data, Size, off) % 32;
  if (off + chunk_len1 > Size) chunk_len1 = Size - off;
  png_write_chunk(png_ptr, chunk_name1,
                  chunk_len1 ? reinterpret_cast<png_const_bytep>(Data + off) : nullptr,
                  chunk_len1);
  off += chunk_len1;

  uint8_t chunk_name2[4] = {'t', 'E', 'S', 't'};
  size_t chunk_len2 = 0;
  if (off < Size) chunk_len2 = ReadU8(Data, Size, off) % 32;
  if (off + chunk_len2 > Size) chunk_len2 = Size - off;
  png_write_chunk(png_ptr, chunk_name2,
                  chunk_len2 ? reinterpret_cast<png_const_bytep>(Data + off) : nullptr,
                  chunk_len2);
  off += chunk_len2;

  // Finish a minimal image to keep writer state consistent.
  size_t rowbytes = 0;
  switch (color_type) {
    case PNG_COLOR_TYPE_GRAY: rowbytes = static_cast<size_t>(width); break;
    case PNG_COLOR_TYPE_RGB: rowbytes = static_cast<size_t>(width) * 3; break;
    case PNG_COLOR_TYPE_RGBA: rowbytes = static_cast<size_t>(width) * 4; break;
    default: rowbytes = static_cast<size_t>(width); break;
  }

  std::vector<png_byte> row(rowbytes);
  for (int y = 0; y < height; ++y) {
    for (size_t x = 0; x < rowbytes; ++x) {
      row[x] = (off < Size) ? Data[off++] : static_cast<uint8_t>(x + y);
    }
    png_write_row(png_ptr, row.data());
  }

  png_write_end(png_ptr, info_ptr);
  png_destroy_write_struct(&png_ptr, &info_ptr);
  fclose(fp);
  return 0;
}