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
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <setjmp.h>

static void PngWriteCallback(png_structp png_ptr, png_bytep data, png_size_t length) {
  FILE* fp = static_cast<FILE*>(png_get_io_ptr(png_ptr));
  if (fp != nullptr && data != nullptr && length > 0) {
    (void)fwrite(data, 1, length, fp);
  }
}

static void PngFlushCallback(png_structp png_ptr) {
  FILE* fp = static_cast<FILE*>(png_get_io_ptr(png_ptr));
  if (fp != nullptr) {
    (void)fflush(fp);
  }
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE* fp = fopen("./dummy_file", "wb");
  if (fp == nullptr) {
    return 0;
  }

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

  uint32_t seed = 0;
  for (size_t i = 0; i < Size && i < 4; ++i) {
    seed = (seed << 8) | Data[i];
  }

  png_uint_32 width = (Size > 0) ? (static_cast<png_uint_32>(Data[0]) % 32 + 1) : 1;
  png_uint_32 height = (Size > 1) ? (static_cast<png_uint_32>(Data[1]) % 32 + 1) : 1;

  int color_types[] = {
      PNG_COLOR_TYPE_GRAY,
      PNG_COLOR_TYPE_RGB,
      PNG_COLOR_TYPE_PALETTE,
      PNG_COLOR_TYPE_GRAY_ALPHA,
      PNG_COLOR_TYPE_RGB_ALPHA
  };
  int bit_depths[] = {1, 2, 4, 8, 16};

  int color_type = color_types[(Size > 2 ? Data[2] : 0) % 5];
  int bit_depth = bit_depths[(Size > 3 ? Data[3] : 3) % 5];

  if ((color_type == PNG_COLOR_TYPE_RGB ||
       color_type == PNG_COLOR_TYPE_GRAY_ALPHA ||
       color_type == PNG_COLOR_TYPE_RGB_ALPHA) &&
      bit_depth < 8) {
    bit_depth = 8;
  }

  if (color_type == PNG_COLOR_TYPE_PALETTE && bit_depth == 16) {
    bit_depth = 8;
  }

  int interlace_type = (Size > 4 && (Data[4] & 1)) ? PNG_INTERLACE_ADAM7 : PNG_INTERLACE_NONE;
  int compression_type = PNG_COMPRESSION_TYPE_BASE;
  int filter_type = PNG_FILTER_TYPE_BASE;

  png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, color_type,
               interlace_type, compression_type, filter_type);

  png_color palette[256];
  if (color_type == PNG_COLOR_TYPE_PALETTE) {
    int palette_entries = (Size > 5) ? (Data[5] % 256) : 0;
    if (palette_entries == 0) palette_entries = 1;
    for (int i = 0; i < palette_entries; ++i) {
      uint8_t v = (Size > 6 + static_cast<size_t>(i)) ? Data[6 + i] : static_cast<uint8_t>(i);
      palette[i].red = v;
      palette[i].green = static_cast<png_byte>(v ^ 0x55);
      palette[i].blue = static_cast<png_byte>(v ^ 0xAA);
    }
    png_set_PLTE(png_ptr, info_ptr, palette, palette_entries);
  }

  if (Size > 6) {
    png_set_sig_bytes(png_ptr, Data[6] % 8);
  } else {
    png_set_sig_bytes(png_ptr, 0);
  }

  png_write_sig(png_ptr);
  png_write_flush(png_ptr);

  if (Size > 16) {
    png_byte chunk_name[4];
    chunk_name[0] = (Size > 7 && Data[7] >= 'A' && Data[7] <= 'Z') ? Data[7] : 'f';
    chunk_name[1] = (Size > 8 && Data[8] >= 'A' && Data[8] <= 'z') ? Data[8] : 'U';
    chunk_name[2] = (Size > 9 && Data[9] >= 'A' && Data[9] <= 'z') ? Data[9] : 'Z';
    chunk_name[3] = (Size > 10 && Data[10] >= 'A' && Data[10] <= 'z') ? Data[10] : 'Z';

    size_t chunk_len = (Size > 11) ? (Data[11] % (Size - 12 + 1)) : 0;
    const png_bytep chunk_data = (chunk_len > 0 && Size > 12)
                                     ? reinterpret_cast<const png_bytep>(Data + 12)
                                     : nullptr;
    png_write_chunk(png_ptr, chunk_name, chunk_data, chunk_len);
    png_write_flush(png_ptr);
  }

  png_write_info(png_ptr, info_ptr);

  int channels = 1;
  switch (color_type) {
    case PNG_COLOR_TYPE_GRAY: channels = 1; break;
    case PNG_COLOR_TYPE_PALETTE: channels = 1; break;
    case PNG_COLOR_TYPE_RGB: channels = 3; break;
    case PNG_COLOR_TYPE_GRAY_ALPHA: channels = 2; break;
    case PNG_COLOR_TYPE_RGB_ALPHA: channels = 4; break;
    default: channels = 1; break;
  }

  size_t bytes_per_sample = (bit_depth == 16) ? 2u : 1u;
  size_t rowbytes;
  if (bit_depth < 8 && (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_PALETTE)) {
    rowbytes = ((static_cast<size_t>(width) * static_cast<size_t>(bit_depth) * static_cast<size_t>(channels)) + 7u) / 8u;
  } else {
    rowbytes = static_cast<size_t>(width) * static_cast<size_t>(channels) * bytes_per_sample;
  }
  if (rowbytes == 0) rowbytes = 1;

  std::vector<uint8_t> image_storage(rowbytes * height);
  for (size_t i = 0; i < image_storage.size(); ++i) {
    image_storage[i] = (Size > 0) ? Data[(i + seed) % Size] : 0;
  }

  std::vector<png_bytep> rows(height);
  for (png_uint_32 y = 0; y < height; ++y) {
    rows[y] = reinterpret_cast<png_bytep>(&image_storage[static_cast<size_t>(y) * rowbytes]);
  }

  if (Size > 12 && (Data[12] & 1)) {
    png_write_image(png_ptr, rows.data());
    png_write_flush(png_ptr);
  } else {
    for (png_uint_32 y = 0; y < height; ++y) {
      png_write_row(png_ptr, rows[y]);
      if (Size > 13 && ((Data[13] + y) % 3 == 0)) {
        png_write_flush(png_ptr);
      }
    }
  }

  if (Size > 14) {
    png_byte chunk_name2[4] = {'t', 'E', 'S', 'T'};
    size_t off = 15;
    size_t chunk_len2 = (off < Size) ? (Data[14] % (Size - off + 1)) : 0;
    const png_bytep chunk_data2 =
        (chunk_len2 > 0 && off < Size) ? reinterpret_cast<const png_bytep>(Data + off) : nullptr;
    png_write_chunk(png_ptr, chunk_name2, chunk_data2, chunk_len2);
    png_write_flush(png_ptr);
  }

  png_write_flush(png_ptr);
  png_destroy_write_struct(&png_ptr, &info_ptr);
  fclose(fp);
  return 0;
}