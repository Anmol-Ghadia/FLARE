// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_IHDR at pngset.c:254:1 in png.h
// png_set_PLTE at pngset.c:572:1 in png.h
// png_set_oFFs at pngset.c:295:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_set_pCAL at pngset.c:312:1 in png.h
// png_write_info at pngwrite.c:192:1 in png.h
// png_write_chunk at pngwutil.c:192:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_write_row at pngwrite.c:693:1 in png.h
// png_write_end at pngwrite.c:358:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_save_int_32 at png.c:726:1 in png.h
// png_save_int_32 at png.c:726:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_write_fn at pngwio.c:122:1 in png.h
#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

#include <png.h>

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

static void WriteDataFn(png_structp png_ptr, png_bytep data, png_size_t length) {
  FILE* fp = reinterpret_cast<FILE*>(png_get_io_ptr(png_ptr));
  if (fp != nullptr && data != nullptr && length > 0) {
    fwrite(data, 1, length, fp);
  }
}

static void FlushDataFn(png_structp png_ptr) {
  FILE* fp = reinterpret_cast<FILE*>(png_get_io_ptr(png_ptr));
  if (fp != nullptr) {
    fflush(fp);
  }
}

static png_uint_32 ReadU32(const uint8_t* data, size_t size, size_t offset) {
  if (offset + 4 > size) return 0;
  return (static_cast<png_uint_32>(data[offset]) << 24) |
         (static_cast<png_uint_32>(data[offset + 1]) << 16) |
         (static_cast<png_uint_32>(data[offset + 2]) << 8) |
         (static_cast<png_uint_32>(data[offset + 3]));
}

static png_int_32 ReadI32(const uint8_t* data, size_t size, size_t offset) {
  return static_cast<png_int_32>(ReadU32(data, size, offset));
}

static png_byte SafeByte(const uint8_t* data, size_t size, size_t offset,
                         png_byte defval) {
  if (offset < size) return static_cast<png_byte>(data[offset]);
  return defval;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  png_byte int_buf[4] = {0, 0, 0, 0};
  if (Size >= 4) {
    memcpy(int_buf, Data, 4);
  } else if (Size > 0) {
    memcpy(int_buf, Data, Size);
  }

  png_int_32 parsed_val = png_get_int_32(int_buf);

  png_byte saved_buf[4] = {0, 0, 0, 0};
  png_save_int_32(saved_buf, parsed_val);

  if (Size >= 8) {
    png_get_int_32(saved_buf);
    png_save_int_32(saved_buf, png_get_int_32(Data + (Size - 4)));
  }

  FILE* fp = fopen("./dummy_file", "wb");
  if (fp == nullptr) {
    return 0;
  }

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

  png_set_write_fn(png_ptr, fp, WriteDataFn, FlushDataFn);

  png_uint_32 width = (ReadU32(Data, Size, 8) % 32) + 1;
  png_uint_32 height = (ReadU32(Data, Size, 12) % 32) + 1;
  int bit_depth = (SafeByte(Data, Size, 16, 8) & 1) ? 8 : 1;

  int color_type_choices[] = {
      PNG_COLOR_TYPE_GRAY,
      PNG_COLOR_TYPE_RGB,
      PNG_COLOR_TYPE_PALETTE,
      PNG_COLOR_TYPE_GRAY_ALPHA,
      PNG_COLOR_TYPE_RGB_ALPHA
  };
  int color_type = color_type_choices[SafeByte(Data, Size, 17, 0) % 5];

  int interlace = (SafeByte(Data, Size, 18, 0) & 1) ? PNG_INTERLACE_ADAM7
                                                     : PNG_INTERLACE_NONE;
  png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, color_type,
               interlace, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

  png_color palette[256];
  if (color_type == PNG_COLOR_TYPE_PALETTE) {
    int palette_entries =
        static_cast<int>((SafeByte(Data, Size, 19, 1) % 16) + 1);
    for (int i = 0; i < palette_entries; ++i) {
      size_t base = 20 + static_cast<size_t>(i) * 3;
      palette[i].red = SafeByte(Data, Size, base, static_cast<png_byte>(i));
      palette[i].green =
          SafeByte(Data, Size, base + 1, static_cast<png_byte>(i * 3));
      palette[i].blue =
          SafeByte(Data, Size, base + 2, static_cast<png_byte>(255 - i));
    }
    png_set_PLTE(png_ptr, info_ptr, palette, palette_entries);
  }

  png_set_oFFs(png_ptr, info_ptr, parsed_val, ReadI32(Data, Size, 24),
               SafeByte(Data, Size, 28, 0) % 3);

  std::string purpose = "p";
  std::string units = "u";

  if (Size > 29) {
    size_t purpose_len = (SafeByte(Data, Size, 29, 0) % 16) + 1;
    for (size_t i = 0; i < purpose_len; ++i) {
      purpose.push_back(
          static_cast<char>('A' + (SafeByte(Data, Size, 30 + i, 0) % 26)));
    }
  }

  if (Size > 46) {
    size_t units_len = (SafeByte(Data, Size, 46, 0) % 16) + 1;
    for (size_t i = 0; i < units_len; ++i) {
      units.push_back(
          static_cast<char>('a' + (SafeByte(Data, Size, 47 + i, 0) % 26)));
    }
  }

  int pcal_type = SafeByte(Data, Size, 63, 0) % 6;
  int nparams = SafeByte(Data, Size, 64, 0) % 4;

  std::vector<std::string> param_storage;
  std::vector<char*> param_ptrs;
  for (int i = 0; i < nparams; ++i) {
    std::string s = "v";
    size_t base = 65 + static_cast<size_t>(i) * 4;
    size_t len = (SafeByte(Data, Size, base, 0) % 8) + 1;
    for (size_t j = 0; j < len; ++j) {
      s.push_back(
          static_cast<char>('0' + (SafeByte(Data, Size, base + 1 + j, 0) % 10)));
    }
    param_storage.push_back(s);
  }
  for (size_t i = 0; i < param_storage.size(); ++i) {
    param_ptrs.push_back(const_cast<char*>(param_storage[i].c_str()));
  }

  png_set_pCAL(png_ptr, info_ptr, purpose.c_str(), ReadI32(Data, Size, 69),
               ReadI32(Data, Size, 73), pcal_type, nparams, units.c_str(),
               nparams > 0 ? param_ptrs.data() : nullptr);

  png_write_info(png_ptr, info_ptr);

  png_byte chunk_name[4] = {'f', 'U', 'Z', 'z'};
  if (Size >= 4) {
    chunk_name[0] = Data[Size - 4];
    chunk_name[1] = Data[Size - 3];
    chunk_name[2] = Data[Size - 2];
    chunk_name[3] = Data[Size - 1];
  }

  const png_byte* chunk_data = saved_buf;
  size_t chunk_len = sizeof(saved_buf);
  if (Size > 80) {
    chunk_data = reinterpret_cast<const png_byte*>(Data + 80);
    chunk_len = (Size - 80) % 64;
  }
  png_write_chunk(png_ptr, chunk_name, chunk_data, chunk_len);

  int channels = 1;
  switch (color_type) {
    case PNG_COLOR_TYPE_GRAY: channels = 1; break;
    case PNG_COLOR_TYPE_PALETTE: channels = 1; break;
    case PNG_COLOR_TYPE_RGB: channels = 3; break;
    case PNG_COLOR_TYPE_GRAY_ALPHA: channels = 2; break;
    case PNG_COLOR_TYPE_RGB_ALPHA: channels = 4; break;
    default: channels = 1; break;
  }

  size_t rowbytes = width * channels * static_cast<size_t>(bit_depth) / 8;
  if (rowbytes == 0) rowbytes = width;
  std::vector<png_byte> row(rowbytes);

  for (png_uint_32 y = 0; y < height; ++y) {
    for (size_t x = 0; x < rowbytes; ++x) {
      row[x] = SafeByte(Data, Size,
                        (Size == 0) ? 0 : ((x + y) % Size),
                        static_cast<png_byte>(x + y));
    }
    png_write_row(png_ptr, row.data());
  }

  png_write_end(png_ptr, info_ptr);
  png_destroy_write_struct(&png_ptr, &info_ptr);
  fclose(fp);
  return 0;
}