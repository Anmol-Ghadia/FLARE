#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <png.h>

static void PngWriteCallback(png_structp png_ptr, png_bytep data, png_size_t length) {
  FILE* fp = reinterpret_cast<FILE*>(png_get_io_ptr(png_ptr));
  if (fp != nullptr && data != nullptr && length > 0) {
    fwrite(data, 1, length, fp);
  }
}

static void PngFlushCallback(png_structp png_ptr) {
  FILE* fp = reinterpret_cast<FILE*>(png_get_io_ptr(png_ptr));
  if (fp != nullptr) fflush(fp);
}

static png_uint_32 ReadU32(const uint8_t* data, size_t size, size_t& off) {
  png_uint_32 v = 0;
  for (int i = 0; i < 4; ++i) {
    v <<= 8;
    if (off < size) v |= data[off++];
  }
  return v;
}

static uint8_t NextByte(const uint8_t* data, size_t size, size_t& off, uint8_t fallback) {
  if (off < size) return data[off++];
  return fallback;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  if (Data == nullptr) return 0;

  FILE* fp = fopen("./dummy_file", "wb");
  if (fp == nullptr) return 0;

  png_structp png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
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

  const int color_types[] = {
      PNG_COLOR_TYPE_GRAY,
      PNG_COLOR_TYPE_GRAY_ALPHA,
      PNG_COLOR_TYPE_RGB,
      PNG_COLOR_TYPE_RGB_ALPHA,
      PNG_COLOR_TYPE_PALETTE
  };
  const int bit_depths[] = {1, 2, 4, 8, 16};

  int color_type =
      color_types[NextByte(Data, Size, off, 0) % (sizeof(color_types) / sizeof(color_types[0]))];
  int bit_depth =
      bit_depths[NextByte(Data, Size, off, 0) % (sizeof(bit_depths) / sizeof(bit_depths[0]))];

  if (color_type == PNG_COLOR_TYPE_RGB ||
      color_type == PNG_COLOR_TYPE_RGB_ALPHA ||
      color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
    if (bit_depth < 8) bit_depth = 8;
  }
  if (color_type == PNG_COLOR_TYPE_PALETTE && bit_depth == 16) {
    bit_depth = 8;
  }

  int interlace_type =
      (NextByte(Data, Size, off, 0) & 1) ? PNG_INTERLACE_ADAM7 : PNG_INTERLACE_NONE;

  png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, color_type,
               interlace_type, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

  png_color palette[256];
  png_uint_16 hist[256];
  if (color_type == PNG_COLOR_TYPE_PALETTE) {
    int palette_size = (NextByte(Data, Size, off, 0) % 256) + 1;
    for (int i = 0; i < palette_size; ++i) {
      palette[i].red = NextByte(Data, Size, off, static_cast<uint8_t>(i));
      palette[i].green = NextByte(Data, Size, off, static_cast<uint8_t>(255 - i));
      palette[i].blue = NextByte(Data, Size, off, static_cast<uint8_t>(i ^ 0xAA));
      uint16_t hi = NextByte(Data, Size, off, static_cast<uint8_t>(i));
      uint16_t lo = NextByte(Data, Size, off, static_cast<uint8_t>(i ^ 0x55));
      hist[i] = static_cast<png_uint_16>((hi << 8) | lo);
    }
    png_set_PLTE(png_ptr, info_ptr, palette, palette_size);
    png_set_hIST(png_ptr, info_ptr, hist);
  } else {
    png_set_hIST(png_ptr, info_ptr, hist);
  }

  png_color_8 sig_bit;
  std::memset(&sig_bit, 0, sizeof(sig_bit));
  sig_bit.red = NextByte(Data, Size, off, 8);
  sig_bit.green = NextByte(Data, Size, off, 8);
  sig_bit.blue = NextByte(Data, Size, off, 8);
  sig_bit.gray = NextByte(Data, Size, off, 8);
  sig_bit.alpha = NextByte(Data, Size, off, 8);
  png_set_sBIT(png_ptr, info_ptr, &sig_bit);

  png_color_16 background;
  std::memset(&background, 0, sizeof(background));
  background.index = NextByte(Data, Size, off, 0);
  background.red = static_cast<png_uint_16>(NextByte(Data, Size, off, 0));
  background.green = static_cast<png_uint_16>(NextByte(Data, Size, off, 0));
  background.blue = static_cast<png_uint_16>(NextByte(Data, Size, off, 0));
  background.gray = static_cast<png_uint_16>(NextByte(Data, Size, off, 0));
  png_set_bKGD(png_ptr, info_ptr, &background);

  png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);
  if (rowbytes == 0) rowbytes = 1;

  std::vector<png_byte> image(static_cast<size_t>(rowbytes) * height);
  for (size_t i = 0; i < image.size(); ++i) {
    image[i] = NextByte(Data, Size, off, static_cast<uint8_t>(i));
  }

  std::vector<png_bytep> rows(height);
  for (png_uint_32 y = 0; y < height; ++y) {
    rows[y] = image.data() + static_cast<size_t>(y) * rowbytes;
  }
  png_set_rows(png_ptr, info_ptr, rows.data());

  if (NextByte(Data, Size, off, 0) & 1) {
    png_write_info_before_PLTE(png_ptr, info_ptr);
  }

  if (NextByte(Data, Size, off, 0) & 1) {
    png_write_info(png_ptr, info_ptr);
  }

  int transforms = 0;
  uint8_t t = NextByte(Data, Size, off, 0);
  if (t & 0x01) transforms |= PNG_TRANSFORM_STRIP_16;
  if (t & 0x02) transforms |= PNG_TRANSFORM_PACKING;
  if (t & 0x04) transforms |= PNG_TRANSFORM_PACKSWAP;
  if (t & 0x08) transforms |= PNG_TRANSFORM_SHIFT;
  if (t & 0x10) transforms |= PNG_TRANSFORM_SWAP_ALPHA;
  if (t & 0x20) transforms |= PNG_TRANSFORM_BGR;
  if (t & 0x40) transforms |= PNG_TRANSFORM_SWAP_ENDIAN;
  if (t & 0x80) transforms |= PNG_TRANSFORM_INVERT_ALPHA;

  png_write_png(png_ptr, info_ptr, transforms, nullptr);

  png_destroy_write_struct(&png_ptr, &info_ptr);
  fclose(fp);
  return 0;
}