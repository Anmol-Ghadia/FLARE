// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_get_io_ptr at png.c:686:1 in png.h
// png_set_IHDR at pngset.c:254:1 in png.h
// png_set_PLTE at pngset.c:572:1 in png.h
// png_set_text at pngset.c:762:1 in png.h
// png_write_chunk at pngwutil.c:192:1 in png.h
// png_write_info at pngwrite.c:192:1 in png.h
// png_write_row at pngwrite.c:693:1 in png.h
// png_write_chunk at pngwutil.c:192:1 in png.h
// png_write_chunk at pngwutil.c:192:1 in png.h
// png_write_chunk_end at pngwutil.c:146:1 in png.h
// png_write_end at pngwrite.c:358:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_write_fn at pngwio.c:122:1 in png.h
// png_set_sig_bytes at png.c:47:1 in png.h
// png_write_sig at pngwutil.c:51:1 in png.h
#include <cstdint>
#include <cstddef>
#include <vector>
#include <fstream>
#include <cstring>
#include <setjmp.h>
#include <png.h>

static void PngWriteCallback(png_structp png_ptr, png_bytep data, png_size_t length) {
  png_voidp io_ptr = png_get_io_ptr(png_ptr);
  if (io_ptr == nullptr || data == nullptr || length == 0) return;
  std::vector<unsigned char>* out =
      reinterpret_cast<std::vector<unsigned char>*>(io_ptr);
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

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  std::ofstream dummy("./dummy_file", std::ios::binary);
  if (dummy.is_open() && Data != nullptr && Size > 0) {
    dummy.write(reinterpret_cast<const char*>(Data),
                static_cast<std::streamsize>(Size));
  }
  dummy.close();

  if (Data == nullptr) return 0;

  size_t off = 0;
  const uint8_t mode = off < Size ? Data[off++] : 0;
  const uint8_t width_b = off < Size ? Data[off++] : 1;
  const uint8_t height_b = off < Size ? Data[off++] : 1;
  const uint8_t color_sel = off < Size ? Data[off++] : 0;
  const uint8_t bit_sel = off < Size ? Data[off++] : 8;
  const uint8_t interlace_sel = off < Size ? Data[off++] : 0;
  const uint8_t chunk_count = off < Size ? Data[off++] : 0;
  const uint8_t text_flag = off < Size ? Data[off++] : 0;

  png_structp png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (png_ptr == nullptr) return 0;

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr) {
    png_destroy_write_struct(&png_ptr, nullptr);
    return 0;
  }

  std::vector<unsigned char> out;
  std::vector<png_byte> row;

  if (setjmp(png_jmpbuf(png_ptr))) {
    row.clear();
    row.shrink_to_fit();
    out.clear();
    out.shrink_to_fit();
    png_destroy_write_struct(&png_ptr, &info_ptr);
    return 0;
  }

  png_set_write_fn(png_ptr, &out, PngWriteCallback, PngFlushCallback);

  if (mode & 0x01) {
    png_set_sig_bytes(png_ptr, mode & 0x07);
  }

  if (mode & 0x02) {
    png_write_sig(png_ptr);
  }

  png_uint_32 width = (width_b % 32) + 1;
  png_uint_32 height = (height_b % 32) + 1;

  int color_type;
  switch (color_sel % 5) {
    case 0: color_type = PNG_COLOR_TYPE_GRAY; break;
    case 1: color_type = PNG_COLOR_TYPE_RGB; break;
    case 2: color_type = PNG_COLOR_TYPE_PALETTE; break;
    case 3: color_type = PNG_COLOR_TYPE_GRAY_ALPHA; break;
    default: color_type = PNG_COLOR_TYPE_RGBA; break;
  }

  int bit_depth;
  switch (bit_sel % 5) {
    case 0: bit_depth = 1; break;
    case 1: bit_depth = 2; break;
    case 2: bit_depth = 4; break;
    case 3: bit_depth = 8; break;
    default: bit_depth = 16; break;
  }

  if (color_type == PNG_COLOR_TYPE_RGB ||
      color_type == PNG_COLOR_TYPE_GRAY_ALPHA ||
      color_type == PNG_COLOR_TYPE_RGBA) {
    if (bit_depth < 8) bit_depth = 8;
  }

  int interlace = (interlace_sel & 1) ? PNG_INTERLACE_ADAM7 : PNG_INTERLACE_NONE;

  png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, color_type,
               interlace, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

  png_color palette[256];
  if (color_type == PNG_COLOR_TYPE_PALETTE) {
    for (int i = 0; i < 256; ++i) {
      palette[i].red = static_cast<png_byte>(i);
      palette[i].green = static_cast<png_byte>(255 - i);
      palette[i].blue = static_cast<png_byte>((i * 3) & 0xff);
    }
    int palette_entries = 1 + ((off < Size ? Data[off++] : 0) % 256);
    png_set_PLTE(png_ptr, info_ptr, palette, palette_entries);
  }

  if (text_flag & 1) {
    png_text text;
    std::memset(&text, 0, sizeof(text));
    char key[] = "k";
    char value[] = "v";
    text.compression = PNG_TEXT_COMPRESSION_NONE;
    text.key = key;
    text.text = value;
    png_set_text(png_ptr, info_ptr, &text, 1);
  }

  if (mode & 0x04) {
    png_byte name[4] = {'f', 'U', 'Z', 'z'};
    if (off + 4 <= Size) {
      name[0] = Data[off++];
      name[1] = Data[off++];
      name[2] = Data[off++];
      name[3] = Data[off++];
    }
    size_t len = (off < Size) ? (Data[off++] % 32) : 0;
    if (off + len > Size) len = Size - off;
    png_write_chunk(png_ptr, name,
                    len ? reinterpret_cast<png_const_bytep>(Data + off) : nullptr,
                    len);
    off += len;
  }

  png_write_info(png_ptr, info_ptr);

  int channels = 1;
  switch (color_type) {
    case PNG_COLOR_TYPE_GRAY: channels = 1; break;
    case PNG_COLOR_TYPE_PALETTE: channels = 1; break;
    case PNG_COLOR_TYPE_RGB: channels = 3; break;
    case PNG_COLOR_TYPE_GRAY_ALPHA: channels = 2; break;
    case PNG_COLOR_TYPE_RGBA: channels = 4; break;
    default: channels = 1; break;
  }

  size_t bits_per_pixel =
      static_cast<size_t>(channels) * static_cast<size_t>(bit_depth);
  size_t rowbytes = (static_cast<size_t>(width) * bits_per_pixel + 7) / 8;
  if (rowbytes == 0) rowbytes = 1;

  row.resize(rowbytes);
  for (png_uint_32 y = 0; y < height; ++y) {
    for (size_t i = 0; i < rowbytes; ++i) {
      row[i] = off < Size ? Data[off++] : static_cast<uint8_t>(i + y);
    }
    png_write_row(png_ptr, row.data());

    if ((mode & 0x08) && y == 0) {
      png_byte cname[4] = {'r', 'O', 'W', '1'};
      size_t len = (off < Size) ? (Data[off++] % 16) : 0;
      if (off + len > Size) len = Size - off;
      png_write_chunk(png_ptr, cname,
                      len ? reinterpret_cast<png_const_bytep>(Data + off) : nullptr,
                      len);
      off += len;
    }
  }

  for (uint8_t i = 0; i < chunk_count; ++i) {
    png_byte cname[4] = {'A', 'a', '0', '0'};
    if (off + 4 <= Size) {
      cname[0] = Data[off++];
      cname[1] = Data[off++];
      cname[2] = Data[off++];
      cname[3] = Data[off++];
    }
    size_t len = ReadU32(Data, Size, off) % 64;
    if (off + len > Size) len = Size - off;
    png_write_chunk(png_ptr, cname,
                    len ? reinterpret_cast<png_const_bytep>(Data + off) : nullptr,
                    len);
    off += len;
  }

  if (mode & 0x10) {
    png_write_chunk_end(png_ptr);
  }

  png_write_end(png_ptr, (mode & 0x20) ? nullptr : info_ptr);
  png_destroy_write_struct(&png_ptr, &info_ptr);
  return 0;
}