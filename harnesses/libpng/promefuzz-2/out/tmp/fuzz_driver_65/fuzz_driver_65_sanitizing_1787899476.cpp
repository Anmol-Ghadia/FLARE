#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <setjmp.h>
#include <png.h>

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

static void WriteDataFn(png_structp png_ptr, png_bytep data, png_size_t length) {
  FILE* fp = static_cast<FILE*>(png_get_io_ptr(png_ptr));
  if (fp != nullptr && data != nullptr && length > 0) {
    (void)fwrite(data, 1, length, fp);
  }
}

static void FlushDataFn(png_structp png_ptr) {
  FILE* fp = static_cast<FILE*>(png_get_io_ptr(png_ptr));
  if (fp != nullptr) {
    (void)fflush(fp);
  }
}

static int GetChannels(int color_type) {
  switch (color_type) {
    case PNG_COLOR_TYPE_GRAY: return 1;
    case PNG_COLOR_TYPE_PALETTE: return 1;
    case PNG_COLOR_TYPE_RGB: return 3;
    case PNG_COLOR_TYPE_GRAY_ALPHA: return 2;
    case PNG_COLOR_TYPE_RGB_ALPHA: return 4;
    default: return 1;
  }
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE* fp = fopen("./dummy_file", "wb+");
  if (fp == nullptr) {
    return 0;
  }

  if (Size > 0) {
    (void)fwrite(Data, 1, Size, fp);
    (void)fflush(fp);
    (void)fseek(fp, 0, SEEK_SET);
  }

  png_structp read_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn, PngWarningFn);
  png_infop read_info = nullptr;

  if (read_ptr != nullptr) {
    read_info = png_create_info_struct(read_ptr);
    if (read_info != nullptr) {
      if (setjmp(png_jmpbuf(read_ptr)) == 0) {
        png_set_gray_to_rgb(read_ptr);
        png_set_packing(read_ptr);
        png_set_packswap(read_ptr);
        if (Size >= 8) {
          png_set_sig_bytes(read_ptr, static_cast<int>(Data[0] % 8));
        }
      }
    }
  }

  png_structp write_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn, PngWarningFn);
  png_infop write_info = nullptr;

  if (write_ptr != nullptr) {
    write_info = png_create_info_struct(write_ptr);
    if (write_info != nullptr) {
      if (setjmp(png_jmpbuf(write_ptr)) == 0) {
        png_set_write_fn(write_ptr, fp, WriteDataFn, FlushDataFn);

        png_set_packing(write_ptr);
        png_set_packswap(write_ptr);
        png_write_sig(write_ptr);

        png_uint_32 width = (Size > 0) ? static_cast<png_uint_32>((Data[0] % 8) + 1) : 1;
        png_uint_32 height = (Size > 1) ? static_cast<png_uint_32>((Data[1] % 8) + 1) : 1;

        int color_type = PNG_COLOR_TYPE_GRAY;
        if (Size > 2) {
          switch (Data[2] % 5) {
            case 0: color_type = PNG_COLOR_TYPE_GRAY; break;
            case 1: color_type = PNG_COLOR_TYPE_RGB; break;
            case 2: color_type = PNG_COLOR_TYPE_PALETTE; break;
            case 3: color_type = PNG_COLOR_TYPE_GRAY_ALPHA; break;
            default: color_type = PNG_COLOR_TYPE_RGB_ALPHA; break;
          }
        }

        int bit_depth_choices[] = {1, 2, 4, 8, 16};
        int bit_depth = bit_depth_choices[(Size > 3) ? (Data[3] % 5) : 3];

        if (color_type == PNG_COLOR_TYPE_RGB ||
            color_type == PNG_COLOR_TYPE_RGB_ALPHA ||
            color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
          if (bit_depth < 8) bit_depth = 8;
        }
        if (color_type == PNG_COLOR_TYPE_PALETTE && bit_depth == 16) {
          bit_depth = 8;
        }

        png_set_IHDR(write_ptr, write_info, width, height, bit_depth, color_type,
                     PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE,
                     PNG_FILTER_TYPE_BASE);

        png_color palette[256];
        if (color_type == PNG_COLOR_TYPE_PALETTE) {
          int palette_entries = 1 << std::min(bit_depth, 8);
          for (int i = 0; i < palette_entries; ++i) {
            uint8_t v = (Size > static_cast<size_t>(4 + i)) ? Data[4 + i] : static_cast<uint8_t>(i);
            palette[i].red = v;
            palette[i].green = static_cast<png_byte>(255 - v);
            palette[i].blue = static_cast<png_byte>(v ^ 0xAA);
          }
          png_set_PLTE(write_ptr, write_info, palette, palette_entries);
        }

        int channels = GetChannels(color_type);

        png_size_t rowbytes = png_get_rowbytes(write_ptr, write_info);
        if (rowbytes == 0) {
          rowbytes = 1;
        }

        std::vector<png_byte> image(static_cast<size_t>(rowbytes) * height);
        for (size_t i = 0; i < image.size(); ++i) {
          image[i] = (Size > 0) ? Data[i % Size] : 0;
        }

        if (bit_depth < 8 && channels == 1) {
          png_byte mask = static_cast<png_byte>((1u << bit_depth) - 1u);
          for (size_t i = 0; i < image.size(); ++i) {
            image[i] &= mask;
          }
        }

        std::vector<png_bytep> rows(height);
        for (png_uint_32 y = 0; y < height; ++y) {
          rows[y] = image.data() + static_cast<size_t>(y) * rowbytes;
        }

        png_set_rows(write_ptr, write_info, rows.data());

        if (Size > 4 && (Data[4] & 1)) {
          int transforms = 0;
          if (Size > 5 && (Data[5] & 1)) transforms |= PNG_TRANSFORM_PACKING;
          if (Size > 5 && (Data[5] & 2)) transforms |= PNG_TRANSFORM_PACKSWAP;
          if (Size > 5 && (Data[5] & 4)) transforms |= PNG_TRANSFORM_SWAP_ENDIAN;
          if (Size > 5 && (Data[5] & 8)) transforms |= PNG_TRANSFORM_INVERT_ALPHA;

          if (bit_depth < 8 && channels == 1 &&
              (transforms & PNG_TRANSFORM_PACKING) != 0) {
            std::vector<png_byte> expanded(static_cast<size_t>(width) * channels * height);
            for (png_uint_32 y = 0; y < height; ++y) {
              const png_bytep src = rows[y];
              png_bytep dst = expanded.data() + static_cast<size_t>(y) * width * channels;
              for (png_uint_32 x = 0; x < width; ++x) {
                unsigned int bit_index = x * static_cast<unsigned int>(bit_depth);
                unsigned int byte_index = bit_index / 8;
                unsigned int shift = 8 - bit_depth - (bit_index % 8);
                png_byte sample = static_cast<png_byte>((src[byte_index] >> shift) &
                                                        ((1u << bit_depth) - 1u));
                dst[x] = sample;
              }
            }
            std::vector<png_bytep> expanded_rows(height);
            for (png_uint_32 y = 0; y < height; ++y) {
              expanded_rows[y] = expanded.data() + static_cast<size_t>(y) * width * channels;
            }
            png_set_rows(write_info, expanded_rows.data());
            png_write_png(write_ptr, write_info, transforms, nullptr);
          } else {
            png_write_png(write_ptr, write_info, transforms, nullptr);
          }
        } else {
          png_write_info(write_ptr, write_info);
          for (png_uint_32 y = 0; y < height; ++y) {
            png_write_row(write_ptr, rows[y]);
          }
          png_write_end(write_ptr, write_info);
        }
      }
    }
  }

  if (write_ptr != nullptr || write_info != nullptr) {
    png_destroy_write_struct(write_ptr != nullptr ? &write_ptr : nullptr,
                             write_info != nullptr ? &write_info : nullptr);
  }

  if (read_ptr != nullptr || read_info != nullptr) {
    png_destroy_read_struct(read_ptr != nullptr ? &read_ptr : nullptr,
                            read_info != nullptr ? &read_info : nullptr,
                            nullptr);
  }

  fclose(fp);
  return 0;
}