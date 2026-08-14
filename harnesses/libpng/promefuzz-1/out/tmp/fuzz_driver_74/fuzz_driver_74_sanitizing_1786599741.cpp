#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <png.h>

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

static png_uint_32 ReadU32(const uint8_t *data, size_t size, size_t &off) {
  if (off + 4 > size) return 0;
  png_uint_32 v = static_cast<png_uint_32>(data[off]) |
                  (static_cast<png_uint_32>(data[off + 1]) << 8) |
                  (static_cast<png_uint_32>(data[off + 2]) << 16) |
                  (static_cast<png_uint_32>(data[off + 3]) << 24);
  off += 4;
  return v;
}

static int ReadS32(const uint8_t *data, size_t size, size_t &off) {
  return static_cast<int>(ReadU32(data, size, off));
}

static int ChannelsForColorType(int color_type) {
  switch (color_type) {
    case PNG_COLOR_TYPE_GRAY:
      return 1;
    case PNG_COLOR_TYPE_PALETTE:
      return 1;
    case PNG_COLOR_TYPE_RGB:
      return 3;
    case PNG_COLOR_TYPE_GRAY_ALPHA:
      return 2;
    case PNG_COLOR_TYPE_RGB_ALPHA:
      return 4;
    default:
      return 1;
  }
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  size_t off = 0;

  int benign_allowed = ReadS32(Data, Size, off);
  int flush_rows = ReadS32(Data, Size, off);
  int transforms = ReadS32(Data, Size, off);
  png_fixed_point screen_gamma =
      static_cast<png_fixed_point>(ReadS32(Data, Size, off));
  png_fixed_point file_gamma =
      static_cast<png_fixed_point>(ReadS32(Data, Size, off));
  png_uint_32 width = (ReadU32(Data, Size, off) % 32) + 1;
  png_uint_32 height = (ReadU32(Data, Size, off) % 32) + 1;

  int bit_depth_choices[] = {1, 2, 4, 8, 16};
  int color_type_choices[] = {
      PNG_COLOR_TYPE_GRAY,
      PNG_COLOR_TYPE_GRAY_ALPHA,
      PNG_COLOR_TYPE_RGB,
      PNG_COLOR_TYPE_RGB_ALPHA,
      PNG_COLOR_TYPE_PALETTE
  };

  int bit_depth = bit_depth_choices[ReadU32(Data, Size, off) % 5];
  int color_type = color_type_choices[ReadU32(Data, Size, off) % 5];
  int interlace_type = (ReadU32(Data, Size, off) & 1) ? PNG_INTERLACE_ADAM7
                                                       : PNG_INTERLACE_NONE;
  int compression_type = PNG_COMPRESSION_TYPE_BASE;
  int filter_type = PNG_FILTER_TYPE_BASE;

  FILE *fp = std::fopen("./dummy_file", "wb");
  if (fp == nullptr) return 0;

  png_structp png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn,
                              PngWarningFn);
  if (png_ptr == nullptr) {
    std::fclose(fp);
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr) {
    png_destroy_write_struct(&png_ptr, nullptr);
    std::fclose(fp);
    return 0;
  }

  png_bytep image = nullptr;
  png_bytepp rows = nullptr;

  if (setjmp(png_jmpbuf(png_ptr))) {
    std::free(rows);
    std::free(image);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    std::fclose(fp);
    return 0;
  }

  png_init_io(png_ptr, fp);

  png_set_benign_errors(png_ptr, benign_allowed);
  png_set_flush(png_ptr, flush_rows);
  png_set_bgr(png_ptr);
  png_set_packing(png_ptr);
  png_set_gamma_fixed(png_ptr, screen_gamma, file_gamma);

  png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, color_type,
               interlace_type, compression_type, filter_type);

  if (color_type == PNG_COLOR_TYPE_PALETTE) {
    png_color palette[256];
    int palette_entries = 1 << bit_depth;
    if (palette_entries < 1) palette_entries = 1;
    if (palette_entries > 256) palette_entries = 256;
    for (int i = 0; i < palette_entries; ++i) {
      palette[i].red = static_cast<png_byte>(i);
      palette[i].green = static_cast<png_byte>(255 - i);
      palette[i].blue = static_cast<png_byte>(i ^ 0xAA);
    }
    png_set_PLTE(png_ptr, info_ptr, palette, palette_entries);
  }

  png_write_info(png_ptr, info_ptr);

  int channels = ChannelsForColorType(color_type);
  png_size_t bytes_per_sample = (bit_depth == 16) ? 2u : 1u;
  png_size_t rowbytes = static_cast<png_size_t>(width) *
                        static_cast<png_size_t>(channels) *
                        bytes_per_sample;
  if (rowbytes == 0) rowbytes = 1;

  if (height != 0 && rowbytes > static_cast<png_size_t>(-1) / height) {
    png_error(png_ptr, "size overflow");
  }

  image = static_cast<png_bytep>(std::malloc(rowbytes * height));
  rows = static_cast<png_bytepp>(std::malloc(sizeof(png_bytep) * height));
  if (image == nullptr || rows == nullptr) {
    png_error(png_ptr, "allocation failed");
  }

  size_t remaining = (off < Size) ? (Size - off) : 0;
  for (png_uint_32 y = 0; y < height; ++y) {
    rows[y] = image + static_cast<size_t>(y) * rowbytes;
    for (png_size_t x = 0; x < rowbytes; ++x) {
      if (remaining != 0) {
        rows[y][x] = Data[off + ((static_cast<size_t>(y) * rowbytes + x) % remaining)];
      } else {
        rows[y][x] = static_cast<png_byte>((x + y) & 0xFF);
      }
    }
  }

  png_set_rows(png_ptr, info_ptr, rows);
  png_write_png(png_ptr, info_ptr, transforms, nullptr);

  png_set_benign_errors(png_ptr, !benign_allowed);
  png_set_flush(png_ptr, -flush_rows);
  png_set_bgr(png_ptr);
  png_set_packing(png_ptr);
  png_set_gamma_fixed(png_ptr, file_gamma, screen_gamma);

  std::free(rows);
  std::free(image);
  png_destroy_write_struct(&png_ptr, &info_ptr);
  std::fclose(fp);
  return 0;
}