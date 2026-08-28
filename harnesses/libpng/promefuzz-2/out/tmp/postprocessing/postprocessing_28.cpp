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
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <setjmp.h>
#include <vector>
#include <algorithm>

static void WriteCallback(png_structp png_ptr, png_bytep data, png_size_t length) {
  if (png_ptr == NULL || data == NULL || length == 0) return;
  std::vector<unsigned char>* out =
      reinterpret_cast<std::vector<unsigned char>*>(png_get_io_ptr(png_ptr));
  if (out == NULL) return;
  out->insert(out->end(), data, data + length);
}

static void FlushCallback(png_structp) {
}

static void ReadCallback(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead) {
  if (png_ptr == NULL || outBytes == NULL) return;

  struct ReaderState {
    const uint8_t* data;
    size_t size;
    size_t offset;
  };

  ReaderState* st = reinterpret_cast<ReaderState*>(png_get_io_ptr(png_ptr));
  if (st == NULL) {
    png_error(png_ptr, "no reader state");
    return;
  }

  if (byteCountToRead > st->size - st->offset) {
    png_error(png_ptr, "read overflow");
    return;
  }

  for (png_size_t i = 0; i < byteCountToRead; ++i)
    outBytes[i] = st->data[st->offset + i];
  st->offset += byteCountToRead;
}

static png_uint_32 ReadU32(const uint8_t* data, size_t size, size_t& off) {
  png_uint_32 v = 0;
  for (int i = 0; i < 4; ++i) {
    v <<= 8;
    if (off < size) v |= data[off++];
  }
  return v;
}

static int PickColorType(uint8_t v) {
  switch (v % 5) {
    case 0: return PNG_COLOR_TYPE_GRAY;
    case 1: return PNG_COLOR_TYPE_GRAY_ALPHA;
    case 2: return PNG_COLOR_TYPE_RGB;
    case 3: return PNG_COLOR_TYPE_RGB_ALPHA;
    default: return PNG_COLOR_TYPE_PALETTE;
  }
}

static int ChannelsForColorType(int ct) {
  switch (ct) {
    case PNG_COLOR_TYPE_GRAY: return 1;
    case PNG_COLOR_TYPE_PALETTE: return 1;
    case PNG_COLOR_TYPE_RGB: return 3;
    case PNG_COLOR_TYPE_GRAY_ALPHA: return 2;
    case PNG_COLOR_TYPE_RGB_ALPHA: return 4;
    default: return 4;
  }
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  if (Data == NULL || Size == 0) return 0;

  {
    FILE* fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
      fwrite(Data, 1, Size, fp);
      fclose(fp);
    }
  }

  size_t off = 0;
  png_uint_32 width = (ReadU32(Data, Size, off) % 64) + 1;
  png_uint_32 height = (ReadU32(Data, Size, off) % 64) + 1;
  int color_type = PickColorType(off < Size ? Data[off++] : 0);
  int bit_depth = ((off < Size ? Data[off++] : 0) & 1) ? 16 : 8;
  int interlace_type = ((off < Size ? Data[off++] : 0) & 1) ? PNG_INTERLACE_ADAM7 : PNG_INTERLACE_NONE;
  int compression_type = PNG_COMPRESSION_TYPE_BASE;
  int filter_type = PNG_FILTER_TYPE_BASE;
  png_uint_32 filler = ReadU32(Data, Size, off);
  int filler_flags = ((off < Size ? Data[off++] : 0) & 1) ? PNG_FILLER_AFTER : PNG_FILLER_BEFORE;

  if (color_type == PNG_COLOR_TYPE_PALETTE && bit_depth == 16)
    bit_depth = 8;

  int channels = ChannelsForColorType(color_type);
  size_t bytes_per_sample = (bit_depth == 16) ? 2u : 1u;
  size_t rowbytes = static_cast<size_t>(width) * static_cast<size_t>(channels) * bytes_per_sample;
  if (rowbytes == 0 || rowbytes > (1u << 20)) return 0;

  std::vector<unsigned char> image_storage(rowbytes * height);
  if (image_storage.empty()) return 0;

  for (size_t i = 0; i < image_storage.size(); ++i)
    image_storage[i] = Data[(off + i) % Size];

  std::vector<png_bytep> rows(height);
  for (png_uint_32 y = 0; y < height; ++y)
    rows[y] = image_storage.data() + y * rowbytes;

  std::vector<unsigned char> encoded;
  png_structp write_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  png_infop write_info = write_ptr ? png_create_info_struct(write_ptr) : NULL;

  if (write_ptr != NULL && write_info != NULL) {
    if (setjmp(png_jmpbuf(write_ptr)) == 0) {
      png_set_write_fn(write_ptr, &encoded, WriteCallback, FlushCallback);

      if ((off < Size ? Data[off++] : 0) & 1) {
        png_set_filler(write_ptr, filler, filler_flags);
      }

      if (color_type == PNG_COLOR_TYPE_PALETTE) {
        png_color palette[256];
        for (int i = 0; i < 256; ++i) {
          palette[i].red = static_cast<png_byte>(i);
          palette[i].green = static_cast<png_byte>(255 - i);
          palette[i].blue = static_cast<png_byte>(i ^ 0xAA);
        }
        png_set_PLTE(write_ptr, write_info, palette, 256);
      }

      png_set_IHDR(write_ptr, write_info, width, height, bit_depth, color_type,
                   interlace_type, compression_type, filter_type);

      png_write_info(write_ptr, write_info);

      uint8_t mode = off < Size ? Data[off++] : 0;
      switch (mode % 3) {
        case 0:
          for (png_uint_32 y = 0; y < height; ++y) {
            png_write_row(write_ptr, rows[y]);
            if ((y % 4) == 0) png_write_flush(write_ptr);
          }
          break;
        case 1:
          png_write_rows(write_ptr, rows.data(), height);
          png_write_flush(write_ptr);
          break;
        case 2:
          png_write_image(write_ptr, rows.data());
          png_write_flush(write_ptr);
          break;
      }

      png_write_end(write_ptr, write_info);
    }
  }

  if (write_ptr != NULL || write_info != NULL)
    png_destroy_write_struct(&write_ptr, &write_info);

  if (!encoded.empty()) {
    struct ReaderState {
      const uint8_t* data;
      size_t size;
      size_t offset;
    } reader = { encoded.data(), encoded.size(), 0 };

    png_structp read_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop read_info = read_ptr ? png_create_info_struct(read_ptr) : NULL;

    if (read_ptr != NULL && read_info != NULL) {
      if (setjmp(png_jmpbuf(read_ptr)) == 0) {
        png_set_read_fn(read_ptr, &reader, ReadCallback);
        png_read_info(read_ptr, read_info);

        png_uint_32 rwidth = 0, rheight = 0;
        int rbit_depth = 0, rcolor_type = 0, rinterlace = 0, rcompression = 0, rfilter = 0;
        png_get_IHDR(read_ptr, read_info, &rwidth, &rheight, &rbit_depth, &rcolor_type,
                     &rinterlace, &rcompression, &rfilter);

        if ((off < Size ? Data[off++] : 0) & 1) {
          png_set_filler(read_ptr, filler, filler_flags);
        }

        png_read_update_info(read_ptr, read_info);
        png_size_t read_rowbytes = png_get_rowbytes(read_ptr, read_info);
        if (read_rowbytes > 0 && rheight > 0 && read_rowbytes <= (1u << 20) && rheight <= 256) {
          std::vector<unsigned char> read_storage(read_rowbytes * rheight);
          std::vector<png_bytep> read_rows(rheight);
          for (png_uint_32 y = 0; y < rheight; ++y)
            read_rows[y] = read_storage.data() + y * read_rowbytes;

          png_read_image(read_ptr, read_rows.data());
          png_read_end(read_ptr, read_info);
        }
      }
    }

    if (read_ptr != NULL || read_info != NULL)
      png_destroy_read_struct(&read_ptr, &read_info, NULL);
  }

  return 0;
}