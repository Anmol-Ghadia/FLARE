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
  FILE* fp = reinterpret_cast<FILE*>(png_get_io_ptr(png_ptr));
  if (fp != nullptr && data != nullptr && length > 0) {
    fwrite(data, 1, length, fp);
  }
}

static void PngFlushCallback(png_structp png_ptr) {
  FILE* fp = reinterpret_cast<FILE*>(png_get_io_ptr(png_ptr));
  if (fp != nullptr) {
    fflush(fp);
  }
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
  if (Data == nullptr) return 0;

  FILE* fp = fopen("./dummy_file", "wb");
  if (fp == nullptr) return 0;

  png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
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

  png_set_write_fn(png_ptr, fp, PngWriteCallback, PngFlushCallback);

  size_t off = 0;
  png_uint_32 width = (ReadU32(Data, Size, off) % 32) + 1;
  png_uint_32 height = (ReadU32(Data, Size, off) % 32) + 1;

  int color_types[] = {
      PNG_COLOR_TYPE_GRAY,
      PNG_COLOR_TYPE_GRAY_ALPHA,
      PNG_COLOR_TYPE_RGB,
      PNG_COLOR_TYPE_RGB_ALPHA,
      PNG_COLOR_TYPE_PALETTE
  };
  int bit_depths[] = {1, 2, 4, 8, 16};

  int color_type = color_types[(off < Size ? Data[off++] : 0) % (sizeof(color_types) / sizeof(color_types[0]))];
  int bit_depth = bit_depths[(off < Size ? Data[off++] : 0) % (sizeof(bit_depths) / sizeof(bit_depths[0]))];

  if (color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_RGB_ALPHA ||
      color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
    if (bit_depth < 8) bit_depth = 8;
  }
  if (color_type == PNG_COLOR_TYPE_PALETTE && bit_depth == 16) {
    bit_depth = 8;
  }

  int interlace_type = (off < Size && (Data[off++] & 1)) ? PNG_INTERLACE_ADAM7 : PNG_INTERLACE_NONE;
  int compression_type = PNG_COMPRESSION_TYPE_BASE;
  int filter_type = PNG_FILTER_TYPE_BASE;

  png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, color_type,
               interlace_type, compression_type, filter_type);

  png_color palette[256];
  png_uint_16 hist[256];
  if (color_type == PNG_COLOR_TYPE_PALETTE) {
    int palette_size = (off < Size ? (Data[off++] % 256) : 0) + 1;
    for (int i = 0; i < palette_size; ++i) {
      palette[i].red = (off < Size) ? Data[off++] : static_cast<png_byte>(i);
      palette[i].green = (off < Size) ? Data[off++] : static_cast<png_byte>(255 - i);
      palette[i].blue = (off < Size) ? Data[off++] : static_cast<png_byte>(i ^ 0xAA);
      hist[i] = static_cast<png_uint_16>((off + i < Size) ? (Data[(off + i) % Size] << 8) | Data[(off + i + 1) % Size] : i);
    }
    png_set_PLTE(png_ptr, info_ptr, palette, palette_size);
    png_set_hIST(png_ptr, info_ptr, hist);
  } else {
    png_set_hIST(png_ptr, info_ptr, hist);
  }

  png_color_8 sig_bit;
  memset(&sig_bit, 0, sizeof(sig_bit));
  sig_bit.red = (off < Size) ? Data[off++] : 8;
  sig_bit.green = (off < Size) ? Data[off++] : 8;
  sig_bit.blue = (off < Size) ? Data[off++] : 8;
  sig_bit.gray = (off < Size) ? Data[off++] : 8;
  sig_bit.alpha = (off < Size) ? Data[off++] : 8;
  png_set_sBIT(png_ptr, info_ptr, &sig_bit);

  png_color_16 background;
  memset(&background, 0, sizeof(background));
  background.index = (off < Size) ? Data[off++] : 0;
  background.red = static_cast<png_uint_16>((off < Size) ? Data[off++] : 0);
  background.green = static_cast<png_uint_16>((off < Size) ? Data[off++] : 0);
  background.blue = static_cast<png_uint_16>((off < Size) ? Data[off++] : 0);
  background.gray = static_cast<png_uint_16>((off < Size) ? Data[off++] : 0);
  png_set_bKGD(png_ptr, info_ptr, &background);

  int channels = 1;
  switch (color_type) {
    case PNG_COLOR_TYPE_GRAY: channels = 1; break;
    case PNG_COLOR_TYPE_PALETTE: channels = 1; break;
    case PNG_COLOR_TYPE_RGB: channels = 3; break;
    case PNG_COLOR_TYPE_GRAY_ALPHA: channels = 2; break;
    case PNG_COLOR_TYPE_RGB_ALPHA: channels = 4; break;
    default: channels = 1; break;
  }

  size_t bytes_per_sample = (bit_depth <= 8) ? 1 : 2;
  size_t rowbytes = 0;
  if (color_type == PNG_COLOR_TYPE_PALETTE || color_type == PNG_COLOR_TYPE_GRAY) {
    rowbytes = ((static_cast<size_t>(width) * static_cast<size_t>(bit_depth) * static_cast<size_t>(channels)) + 7) / 8;
  } else {
    rowbytes = static_cast<size_t>(width) * static_cast<size_t>(channels) * bytes_per_sample;
  }
  if (rowbytes == 0) rowbytes = 1;

  std::vector<png_byte> image(rowbytes * height);
  if (!image.empty()) {
    for (size_t i = 0; i < image.size(); ++i) {
      image[i] = (off < Size) ? Data[off++] : static_cast<uint8_t>(i);
      if (off >= Size && Size != 0) off = 0;
    }
  }

  std::vector<png_bytep> rows(height);
  for (png_uint_32 y = 0; y < height; ++y) {
    rows[y] = image.data() + static_cast<size_t>(y) * rowbytes;
  }
  png_set_rows(png_ptr, info_ptr, rows.data());

  if (off < Size && (Data[off++] & 1)) {
    png_write_info_before_PLTE(png_ptr, info_ptr);
  }

  if (off < Size && (Data[off++] & 1)) {
    png_write_info(png_ptr, info_ptr);
  }

  int transforms = 0;
  if (off < Size) {
    uint8_t t = Data[off++];
    if (t & 0x01) transforms |= PNG_TRANSFORM_STRIP_16;
    if (t & 0x02) transforms |= PNG_TRANSFORM_PACKING;
    if (t & 0x04) transforms |= PNG_TRANSFORM_PACKSWAP;
    if (t & 0x08) transforms |= PNG_TRANSFORM_SHIFT;
    if (t & 0x10) transforms |= PNG_TRANSFORM_SWAP_ALPHA;
    if (t & 0x20) transforms |= PNG_TRANSFORM_BGR;
    if (t & 0x40) transforms |= PNG_TRANSFORM_SWAP_ENDIAN;
    if (t & 0x80) transforms |= PNG_TRANSFORM_INVERT_ALPHA;
  }

  png_write_png(png_ptr, info_ptr, transforms, nullptr);

  png_destroy_write_struct(&png_ptr, &info_ptr);
  fclose(fp);
  return 0;
}