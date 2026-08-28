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
#include <string>
#include <fstream>
#include <cstring>
#include <setjmp.h>

static uint32_t ReadU32(const uint8_t *data, size_t size, size_t &off) {
  uint32_t v = 0;
  for (int i = 0; i < 4; ++i) {
    v <<= 8;
    if (off < size) v |= data[off++];
  }
  return v;
}

static int32_t ReadS32(const uint8_t *data, size_t size, size_t &off) {
  return static_cast<int32_t>(ReadU32(data, size, off));
}

static uint8_t ReadU8(const uint8_t *data, size_t size, size_t &off) {
  if (off < size) return data[off++];
  return 0;
}

static std::string ReadString(const uint8_t *data, size_t size, size_t &off, size_t max_len) {
  size_t remaining = off < size ? size - off : 0;
  size_t len = remaining > 0 ? (ReadU8(data, size, off) % (max_len + 1)) : 0;
  if (len > size - off) len = size - off;
  std::string s(reinterpret_cast<const char*>(data + off), len);
  off += len;
  return s;
}

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  size_t off = 0;

  std::ofstream dummy("./dummy_file", std::ios::binary);
  if (dummy) {
    dummy.write(reinterpret_cast<const char*>(Data), static_cast<std::streamsize>(Size));
  }
  dummy.close();

  png_structp png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn, PngWarningFn);
  if (!png_ptr) return 0;

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    png_destroy_write_struct(&png_ptr, nullptr);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    return 0;
  }

  FILE *fp = fopen("./dummy_file", "wb");
  if (!fp) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    return 0;
  }

  png_init_io(png_ptr, fp);

  png_uint_32 width = (ReadU8(Data, Size, off) % 8) + 1;
  png_uint_32 height = (ReadU8(Data, Size, off) % 8) + 1;

  int color_types[] = {
      PNG_COLOR_TYPE_GRAY,
      PNG_COLOR_TYPE_RGB,
      PNG_COLOR_TYPE_PALETTE,
      PNG_COLOR_TYPE_GRAY_ALPHA,
      PNG_COLOR_TYPE_RGB_ALPHA
  };
  int bit_depths[] = {1, 2, 4, 8, 16};

  int color_type = color_types[ReadU8(Data, Size, off) % (sizeof(color_types) / sizeof(color_types[0]))];
  int bit_depth = bit_depths[ReadU8(Data, Size, off) % (sizeof(bit_depths) / sizeof(bit_depths[0]))];

  if (color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_RGB_ALPHA ||
      color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
    if (bit_depth < 8) bit_depth = 8;
  }
  if (color_type == PNG_COLOR_TYPE_PALETTE && bit_depth == 16) bit_depth = 8;
  if ((color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_PALETTE) &&
      !(bit_depth == 1 || bit_depth == 2 || bit_depth == 4 || bit_depth == 8)) {
    bit_depth = 8;
  }

  png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, color_type,
               PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

  if (color_type == PNG_COLOR_TYPE_PALETTE) {
    png_color palette[256];
    for (int i = 0; i < 256; ++i) {
      palette[i].red = static_cast<png_byte>(i);
      palette[i].green = static_cast<png_byte>(255 - i);
      palette[i].blue = static_cast<png_byte>((i * 37) & 0xff);
    }
    int palette_size = 1 << bit_depth;
    if (palette_size < 1) palette_size = 1;
    if (palette_size > 256) palette_size = 256;
    png_set_PLTE(png_ptr, info_ptr, palette, palette_size);
  }

  png_set_oFFs(png_ptr, info_ptr, ReadS32(Data, Size, off), ReadS32(Data, Size, off),
               static_cast<int>(ReadU8(Data, Size, off)));

  png_set_cHRM_fixed(
      png_ptr, info_ptr,
      ReadS32(Data, Size, off), ReadS32(Data, Size, off),
      ReadS32(Data, Size, off), ReadS32(Data, Size, off),
      ReadS32(Data, Size, off), ReadS32(Data, Size, off),
      ReadS32(Data, Size, off), ReadS32(Data, Size, off));

  png_unknown_chunk unknowns[2];
  std::memset(unknowns, 0, sizeof(unknowns));
  for (int i = 0; i < 2; ++i) {
    std::string name = ReadString(Data, Size, off, 4);
    for (size_t j = 0; j < 4; ++j) {
      unknowns[i].name[j] = (j < name.size() && name[j] != 0) ? static_cast<png_byte>(name[j]) : static_cast<png_byte>('A' + i + j);
    }
    size_t chunk_len = (off < Size) ? (ReadU8(Data, Size, off) % 16) : 0;
    if (chunk_len > Size - off) chunk_len = Size - off;
    unknowns[i].data = const_cast<png_bytep>(Data + off);
    unknowns[i].size = static_cast<png_size_t>(chunk_len);
    off += chunk_len;
    unknowns[i].location = static_cast<png_byte>(ReadU8(Data, Size, off));
  }
  png_set_unknown_chunks(png_ptr, info_ptr, unknowns, 2);
  png_set_unknown_chunk_location(png_ptr, info_ptr, static_cast<int>(ReadU8(Data, Size, off) % 4),
                                 static_cast<int>(ReadU8(Data, Size, off)));
  png_set_unknown_chunk_location(png_ptr, info_ptr, static_cast<int>(ReadU8(Data, Size, off) % 4),
                                 static_cast<int>(ReadU8(Data, Size, off)));

  std::vector<std::string> storage;
  std::vector<png_text> texts;
  int num_text = ReadU8(Data, Size, off) % 4;
  storage.reserve(static_cast<size_t>(num_text) * 2);
  texts.resize(num_text);

  for (int i = 0; i < num_text; ++i) {
    storage.push_back(ReadString(Data, Size, off, 16));
    if (storage.back().empty()) storage.back() = "K";
    storage.push_back(ReadString(Data, Size, off, 32));

    std::memset(&texts[i], 0, sizeof(texts[i]));
    texts[i].compression = static_cast<int>(ReadU8(Data, Size, off) % 4) - 1;
    texts[i].key = const_cast<png_charp>(storage[storage.size() - 2].c_str());
    texts[i].text = const_cast<png_charp>(storage[storage.size() - 1].c_str());
    texts[i].text_length = storage[storage.size() - 1].size();
#ifdef PNG_iTXt_SUPPORTED
    texts[i].itxt_length = storage[storage.size() - 1].size();
    texts[i].lang = const_cast<png_charp>("");
    texts[i].lang_key = const_cast<png_charp>("");
#endif
  }

  if (!texts.empty()) {
    png_set_text(png_ptr, info_ptr, texts.data(), static_cast<int>(texts.size()));
  }

  png_write_info_before_PLTE(png_ptr, info_ptr);
  png_write_info_before_PLTE(png_ptr, info_ptr);

  int channels = 1;
  switch (color_type) {
    case PNG_COLOR_TYPE_GRAY: channels = 1; break;
    case PNG_COLOR_TYPE_PALETTE: channels = 1; break;
    case PNG_COLOR_TYPE_RGB: channels = 3; break;
    case PNG_COLOR_TYPE_GRAY_ALPHA: channels = 2; break;
    case PNG_COLOR_TYPE_RGB_ALPHA: channels = 4; break;
    default: channels = 1; break;
  }

  size_t rowbytes = 0;
  if (bit_depth >= 8) {
    rowbytes = static_cast<size_t>(width) * channels * static_cast<size_t>(bit_depth / 8);
  } else {
    rowbytes = (static_cast<size_t>(width) * channels * static_cast<size_t>(bit_depth) + 7) / 8;
  }
  if (rowbytes == 0) rowbytes = 1;

  std::vector<png_byte> image(rowbytes * height);
  for (size_t i = 0; i < image.size(); ++i) {
    image[i] = (off < Size) ? Data[off++] : static_cast<uint8_t>(i);
  }

  std::vector<png_bytep> rows(height);
  for (png_uint_32 y = 0; y < height; ++y) {
    rows[y] = image.data() + y * rowbytes;
  }
  png_set_rows(png_ptr, info_ptr, rows.data());

  int transforms = static_cast<int>(ReadU32(Data, Size, off));
  png_write_png(png_ptr, info_ptr, transforms, nullptr);

  fclose(fp);
  png_destroy_write_struct(&png_ptr, &info_ptr);
  return 0;
}