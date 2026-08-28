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
#include <algorithm>

static void PngReadCallback(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead) {
  png_voidp io_ptr = png_get_io_ptr(png_ptr);
  if (io_ptr == nullptr) {
    png_error(png_ptr, "no io_ptr");
    return;
  }

  struct ReaderState {
    const uint8_t* data;
    size_t size;
    size_t offset;
  };

  ReaderState* st = static_cast<ReaderState*>(io_ptr);
  if (st->offset > st->size || byteCountToRead > st->size - st->offset) {
    png_error(png_ptr, "read overflow");
    return;
  }

  if (byteCountToRead > 0) {
    memcpy(outBytes, st->data + st->offset, byteCountToRead);
    st->offset += byteCountToRead;
  }
}

static void PngWriteCallback(png_structp png_ptr, png_bytep data, png_size_t length) {
  png_voidp io_ptr = png_get_io_ptr(png_ptr);
  if (io_ptr == nullptr) {
    return;
  }

  std::vector<uint8_t>* out = static_cast<std::vector<uint8_t>*>(io_ptr);
  if (data != nullptr && length > 0) {
    out->insert(out->end(), data, data + length);
  }
}

static void PngFlushCallback(png_structp) {
}

static void PngErrorCallback(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningCallback(png_structp, png_const_charp) {
}

static uint32_t ReadU32(const uint8_t* data, size_t size, size_t off, uint32_t fallback) {
  if (off + 4 <= size) {
    return (static_cast<uint32_t>(data[off]) << 24) |
           (static_cast<uint32_t>(data[off + 1]) << 16) |
           (static_cast<uint32_t>(data[off + 2]) << 8) |
           static_cast<uint32_t>(data[off + 3]);
  }
  return fallback;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  if (Data == nullptr || Size == 0) {
    return 0;
  }

  {
    FILE* fp = fopen("./dummy_file", "wb");
    if (fp != nullptr) {
      fwrite(Data, 1, Size, fp);
      fclose(fp);
    }
  }

  struct ReaderState {
    const uint8_t* data;
    size_t size;
    size_t offset;
  } reader{Data, Size, 0};

  png_structp read_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorCallback, PngWarningCallback);
  if (read_ptr == nullptr) {
    return 0;
  }

  png_infop read_info = png_create_info_struct(read_ptr);
  png_infop end_info = png_create_info_struct(read_ptr);
  if (read_info == nullptr || end_info == nullptr) {
    png_destroy_read_struct(&read_ptr, &read_info, &end_info);
    return 0;
  }

  if (setjmp(png_jmpbuf(read_ptr))) {
    png_destroy_read_struct(&read_ptr, &read_info, &end_info);
  } else {
    png_set_read_fn(read_ptr, &reader, PngReadCallback);

    png_read_info(read_ptr, read_info);

    if (Size > 1 && (Data[0] & 1)) {
      png_set_expand(read_ptr);
    }
    if (Size > 2 && (Data[1] & 1)) {
      png_set_gray_to_rgb(read_ptr);
    }
    if (Size > 3 && (Data[2] & 1)) {
      png_set_strip_16(read_ptr);
    }
    if (Size > 4 && (Data[3] & 1)) {
      png_set_packing(read_ptr);
    }

    int passes = 1;
    if (Size > 5 && (Data[4] & 1)) {
      passes = png_set_interlace_handling(read_ptr);
    }

    png_get_current_pass_number(read_ptr);
    png_read_update_info(read_ptr, read_info);

    if (Size > 6 && (Data[5] & 1)) {
      png_read_update_info(read_ptr, read_info);
    }

    png_uint_32 width = png_get_image_width(read_ptr, read_info);
    png_uint_32 height = png_get_image_height(read_ptr, read_info);
    png_size_t rowbytes = png_get_rowbytes(read_ptr, read_info);

    if (rowbytes == 0 || width == 0 || height == 0) {
      png_read_end(read_ptr, end_info);
      png_destroy_read_struct(&read_ptr, &read_info, &end_info);
      read_ptr = nullptr;
    } else {
      const png_uint_32 max_dim = 4096;
      const png_size_t max_rowbytes = 1u << 20;

      if (height > max_dim || width > max_dim || rowbytes > max_rowbytes) {
        png_read_end(read_ptr, end_info);
        png_destroy_read_struct(&read_ptr, &read_info, &end_info);
        read_ptr = nullptr;
      } else {
        std::vector<std::vector<png_byte>> rows(height);
        std::vector<png_bytep> row_ptrs(height);
        for (png_uint_32 y = 0; y < height; ++y) {
          rows[y].resize(rowbytes);
          row_ptrs[y] = rows[y].data();
        }

        if (Size > 7 && (Data[6] & 1)) {
          png_read_image(read_ptr, row_ptrs.data());
          png_get_current_pass_number(read_ptr);
        } else {
          std::vector<png_byte> display_row(rowbytes);
          for (int pass = 0; pass < std::max(1, passes); ++pass) {
            (void)pass;
            for (png_uint_32 y = 0; y < height; ++y) {
              png_bytep dsp = (Size > 8 && (Data[7] & 1)) ? display_row.data() : nullptr;
              png_read_row(read_ptr, row_ptrs[y], dsp);
              png_get_current_pass_number(read_ptr);
            }
          }
        }

        png_read_end(read_ptr, end_info);

        png_structp write_ptr =
            png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorCallback, PngWarningCallback);
        if (write_ptr != nullptr) {
          png_infop write_info = png_create_info_struct(write_ptr);
          if (write_info != nullptr) {
            if (setjmp(png_jmpbuf(write_ptr)) == 0) {
              std::vector<uint8_t> outbuf;
              png_set_write_fn(write_ptr, &outbuf, PngWriteCallback, PngFlushCallback);

              int bit_depth = png_get_bit_depth(read_ptr, read_info);
              int color_type = png_get_color_type(read_ptr, read_info);
              int interlace_type = png_get_interlace_type(read_ptr, read_info);
              int compression_type = png_get_compression_type(read_ptr, read_info);
              int filter_method = png_get_filter_type(read_ptr, read_info);

              png_set_IHDR(write_ptr, write_info, width, height, bit_depth, color_type,
                           interlace_type, compression_type, filter_method);

              png_write_info(write_ptr, write_info);

              if (Size > 9 && (Data[8] & 1)) {
                png_set_interlace_handling(write_ptr);
              }

              png_write_image(write_ptr, row_ptrs.data());
              png_write_end(write_ptr, write_info);
            }
          }
          png_destroy_write_struct(&write_ptr, &write_info);
        }

        png_destroy_read_struct(&read_ptr, &read_info, &end_info);
      }
    }
  }

  return 0;
}