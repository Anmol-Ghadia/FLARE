#include <png.h>

#include <csetjmp>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <vector>

static void PngWriteCallback(png_structp png_ptr, png_bytep data, png_size_t length) {
  if (png_ptr == nullptr || data == nullptr || length == 0) return;
  png_voidp io_ptr = png_get_io_ptr(png_ptr);
  if (io_ptr == nullptr) return;
  std::vector<unsigned char>* out =
      reinterpret_cast<std::vector<unsigned char>*>(io_ptr);
  out->insert(out->end(), data, data + length);
}

static void PngFlushCallback(png_structp) {}

static void PngErrorCallback(png_structp png_ptr, png_const_charp) {
  if (png_ptr != nullptr) longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningCallback(png_structp, png_const_charp) {}

static uint32_t ReadU32(const uint8_t* data, size_t size, size_t& off) {
  uint32_t v = 0;
  for (int i = 0; i < 4; ++i) {
    v <<= 8;
    if (off < size) v |= data[off++];
  }
  return v;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* Data, size_t Size) {
  if (Data == nullptr) return 0;

  FILE* fp = std::fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    if (Size > 0) std::fwrite(Data, 1, Size, fp);
    std::fclose(fp);
  }

  size_t off = 0;
  uint32_t raw_w = ReadU32(Data, Size, off);
  uint32_t raw_h = ReadU32(Data, Size, off);

  png_uint_32 width = (raw_w % 32) + 1;
  png_uint_32 height = (raw_h % 32) + 1;

  const int bit_depths[] = {1, 2, 4, 8, 16};
  const int color_types[] = {
      PNG_COLOR_TYPE_GRAY,
      PNG_COLOR_TYPE_RGB,
      PNG_COLOR_TYPE_PALETTE,
      PNG_COLOR_TYPE_GRAY_ALPHA,
      PNG_COLOR_TYPE_RGB_ALPHA};
  const int interlace_methods[] = {PNG_INTERLACE_NONE, PNG_INTERLACE_ADAM7};
  const int filler_flags[] = {PNG_FILLER_BEFORE, PNG_FILLER_AFTER};

  int bit_depth = bit_depths[(off < Size ? Data[off++] : 0) % 5];
  int color_type = color_types[(off < Size ? Data[off++] : 0) % 5];
  int interlace = interlace_methods[(off < Size ? Data[off++] : 0) % 2];
  int compression = PNG_COMPRESSION_TYPE_BASE;
  int filter = PNG_FILTER_TYPE_BASE;
  png_uint_32 filler = ReadU32(Data, Size, off);
  int filler_flag = filler_flags[(off < Size ? Data[off++] : 0) % 2];
  int use_add_alpha = (off < Size ? Data[off++] : 0) & 1;
  int write_mode = (off < Size ? Data[off++] : 0) % 3;
  int transforms = static_cast<int>(ReadU32(Data, Size, off));

  png_structp png_ptr = png_create_write_struct(
      PNG_LIBPNG_VER_STRING, nullptr, PngErrorCallback, PngWarningCallback);
  if (png_ptr == nullptr) return 0;

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr) {
    png_destroy_write_struct(&png_ptr, nullptr);
    return 0;
  }

  std::vector<unsigned char> output;
  png_bytep image_buffer = nullptr;
  png_bytep* row_ptrs = nullptr;

  if (setjmp(png_jmpbuf(png_ptr))) {
    if (row_ptrs != nullptr) {
      delete[] row_ptrs;
      row_ptrs = nullptr;
    }
    if (image_buffer != nullptr) {
      delete[] image_buffer;
      image_buffer = nullptr;
    }
    png_destroy_write_struct(&png_ptr, &info_ptr);
    return 0;
  }

  png_set_write_fn(png_ptr, &output, PngWriteCallback, PngFlushCallback);

  png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, color_type,
               interlace, compression, filter);

  png_size_t channels = 0;
  switch (color_type) {
    case PNG_COLOR_TYPE_GRAY:
      channels = 1;
      break;
    case PNG_COLOR_TYPE_PALETTE:
      channels = 1;
      break;
    case PNG_COLOR_TYPE_RGB:
      channels = 3;
      break;
    case PNG_COLOR_TYPE_GRAY_ALPHA:
      channels = 2;
      break;
    case PNG_COLOR_TYPE_RGB_ALPHA:
      channels = 4;
      break;
    default:
      channels = 1;
      break;
  }

  png_size_t pixel_depth = channels * static_cast<png_size_t>(bit_depth);
  png_size_t rowbytes = (static_cast<png_size_t>(width) * pixel_depth + 7) / 8;
  if (rowbytes == 0) rowbytes = 1;

  if (height > 0 && rowbytes > 0) {
    image_buffer = new png_byte[rowbytes * height];
    row_ptrs = new png_bytep[height];

    for (png_uint_32 y = 0; y < height; ++y) {
      row_ptrs[y] = image_buffer + static_cast<size_t>(y) * rowbytes;
      for (png_size_t x = 0; x < rowbytes; ++x) {
        row_ptrs[y][x] = (off < Size) ? Data[off++] : static_cast<uint8_t>(x + y);
      }
    }
  }

  if (use_add_alpha) {
    png_set_add_alpha(png_ptr, filler, filler_flag);
  } else {
    png_set_filler(png_ptr, filler, filler_flag);
  }

  if (color_type == PNG_COLOR_TYPE_PALETTE) {
    png_color palette[256];
    for (int i = 0; i < 256; ++i) {
      palette[i].red = static_cast<png_byte>(i);
      palette[i].green = static_cast<png_byte>(255 - i);
      palette[i].blue = static_cast<png_byte>(i ^ 0xAA);
    }
    png_set_PLTE(png_ptr, info_ptr, palette, 256);
  }

  if (write_mode == 0) {
    if (row_ptrs != nullptr) {
      png_set_rows(png_ptr, info_ptr, row_ptrs);
    }
    png_write_png(png_ptr, info_ptr, transforms, nullptr);
  } else if (write_mode == 1) {
    png_write_info(png_ptr, info_ptr);
    if (row_ptrs != nullptr) {
      png_write_image(png_ptr, row_ptrs);
    }
    png_write_end(png_ptr, info_ptr);
  } else {
    png_write_info(png_ptr, info_ptr);
    if (row_ptrs != nullptr) {
      for (png_uint_32 y = 0; y < height; ++y) {
        png_write_row(png_ptr, row_ptrs[y]);
      }
    }
    png_write_end(png_ptr, info_ptr);
  }

  delete[] row_ptrs;
  delete[] image_buffer;
  png_destroy_write_struct(&png_ptr, &info_ptr);
  return 0;
}