// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_PLTE at pngset.c:572:1 in png.h
// png_write_info_before_PLTE at pngwrite.c:84:1 in png.h
// png_write_info_before_PLTE at pngwrite.c:84:1 in png.h
// png_write_chunk at pngwutil.c:192:1 in png.h
// png_write_info at pngwrite.c:192:1 in png.h
// png_write_chunk at pngwutil.c:192:1 in png.h
// png_write_flush at pngwrite.c:915:1 in png.h
// png_write_image at pngwrite.c:596:1 in png.h
// png_write_rows at pngwrite.c:574:1 in png.h
// png_write_flush at pngwrite.c:915:1 in png.h
// png_write_rows at pngwrite.c:574:1 in png.h
// png_write_flush at pngwrite.c:915:1 in png.h
// png_write_rows at pngwrite.c:574:1 in png.h
// png_write_image at pngwrite.c:596:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_write_flush at pngwrite.c:915:1 in png.h
// png_write_end at pngwrite.c:358:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_write_fn at pngwio.c:122:1 in png.h
// png_set_IHDR at pngset.c:254:1 in png.h
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

static void user_write_fn(png_structp png_ptr, png_bytep data, png_size_t length) {
  FILE* fp = reinterpret_cast<FILE*>(png_get_io_ptr(png_ptr));
  if (fp != nullptr && data != nullptr && length > 0) {
    (void)fwrite(data, 1, length, fp);
  }
}

static void user_flush_fn(png_structp png_ptr) {
  FILE* fp = reinterpret_cast<FILE*>(png_get_io_ptr(png_ptr));
  if (fp != nullptr) {
    (void)fflush(fp);
  }
}

static png_uint_32 read_u32(const uint8_t* data, size_t size, size_t& off) {
  png_uint_32 v = 0;
  for (int i = 0; i < 4; ++i) {
    v <<= 8;
    if (off < size) v |= data[off++];
  }
  return v;
}

static int read_u8(const uint8_t* data, size_t size, size_t& off) {
  if (off < size) return data[off++];
  return 0;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  size_t off = 0;

  png_uint_32 width = (read_u32(Data, Size, off) % 64) + 1;
  png_uint_32 height = (read_u32(Data, Size, off) % 64) + 1;

  int bit_depth_choices[] = {1, 2, 4, 8, 16};
  int color_type_choices[] = {
      PNG_COLOR_TYPE_GRAY,
      PNG_COLOR_TYPE_PALETTE,
      PNG_COLOR_TYPE_RGB,
      PNG_COLOR_TYPE_RGB_ALPHA,
      PNG_COLOR_TYPE_GRAY_ALPHA
  };

  int bit_depth = bit_depth_choices[read_u8(Data, Size, off) % 5];
  int color_type = color_type_choices[read_u8(Data, Size, off) % 5];
  int interlace_type = (read_u8(Data, Size, off) & 1) ? PNG_INTERLACE_ADAM7 : PNG_INTERLACE_NONE;
  int compression_type = PNG_COMPRESSION_TYPE_BASE;
  int filter_type = PNG_FILTER_TYPE_BASE;

  if (color_type == PNG_COLOR_TYPE_PALETTE && bit_depth == 16)
    bit_depth = 8;
  if ((color_type == PNG_COLOR_TYPE_RGB ||
       color_type == PNG_COLOR_TYPE_RGB_ALPHA ||
       color_type == PNG_COLOR_TYPE_GRAY_ALPHA) &&
      bit_depth < 8)
    bit_depth = 8;

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

  png_set_write_fn(png_ptr, fp, user_write_fn, user_flush_fn);

  png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, color_type,
               interlace_type, compression_type, filter_type);

  std::vector<png_color> palette_storage;
  if (color_type == PNG_COLOR_TYPE_PALETTE) {
    size_t palette_entries = (read_u8(Data, Size, off) % 256) + 1;
    palette_storage.resize(palette_entries);
    for (size_t i = 0; i < palette_entries; ++i) {
      palette_storage[i].red = static_cast<png_byte>(read_u8(Data, Size, off));
      palette_storage[i].green = static_cast<png_byte>(read_u8(Data, Size, off));
      palette_storage[i].blue = static_cast<png_byte>(read_u8(Data, Size, off));
    }
    png_set_PLTE(png_ptr, info_ptr, palette_storage.data(),
                 static_cast<int>(palette_storage.size()));
  }

  int channels = 1;
  switch (color_type) {
    case PNG_COLOR_TYPE_GRAY: channels = 1; break;
    case PNG_COLOR_TYPE_PALETTE: channels = 1; break;
    case PNG_COLOR_TYPE_RGB: channels = 3; break;
    case PNG_COLOR_TYPE_RGB_ALPHA: channels = 4; break;
    case PNG_COLOR_TYPE_GRAY_ALPHA: channels = 2; break;
    default: channels = 1; break;
  }

  size_t bits_per_pixel = static_cast<size_t>(channels) * static_cast<size_t>(bit_depth);
  size_t rowbytes = (static_cast<size_t>(width) * bits_per_pixel + 7) / 8;
  if (rowbytes == 0) rowbytes = 1;

  std::vector<uint8_t> image_storage(rowbytes * height);
  for (size_t i = 0; i < image_storage.size(); ++i) {
    image_storage[i] = (off < Size) ? Data[off++] : 0;
  }

  std::vector<png_bytep> rows(height);
  for (png_uint_32 y = 0; y < height; ++y) {
    rows[y] = image_storage.data() + static_cast<size_t>(y) * rowbytes;
  }

  int mode = read_u8(Data, Size, off) % 4;
  bool call_before_plte_first = (read_u8(Data, Size, off) & 1) != 0;
  bool extra_chunk_before = (read_u8(Data, Size, off) & 1) != 0;
  bool extra_chunk_after_info = (read_u8(Data, Size, off) & 1) != 0;
  bool do_flushes = (read_u8(Data, Size, off) & 1) != 0;

  png_byte chunk_name[5] = {'f', 'U', 'Z', 'z', 0};
  png_uint_32 chunk_len = read_u32(Data, Size, off) % 64;
  std::vector<png_byte> chunk_data(chunk_len);
  for (png_uint_32 i = 0; i < chunk_len; ++i) {
    chunk_data[i] = static_cast<png_byte>((off < Size) ? Data[off++] : 0);
  }

  if (call_before_plte_first) {
    png_write_info_before_PLTE(png_ptr, info_ptr);
    png_write_info_before_PLTE(png_ptr, info_ptr);
  }

  if (extra_chunk_before) {
    png_write_chunk(png_ptr, chunk_name, chunk_data.empty() ? nullptr : chunk_data.data(),
                    chunk_data.size());
  }

  png_write_info(png_ptr, info_ptr);

  if (extra_chunk_after_info) {
    png_write_chunk(png_ptr, chunk_name, chunk_data.empty() ? nullptr : chunk_data.data(),
                    chunk_data.size());
  }

  if (do_flushes) {
    png_write_flush(png_ptr);
  }

  if (mode == 0) {
    png_write_image(png_ptr, rows.data());
  } else if (mode == 1) {
    png_uint_32 written = 0;
    while (written < height) {
      png_uint_32 n = (read_u8(Data, Size, off) % 8) + 1;
      if (n > height - written) n = height - written;
      png_write_rows(png_ptr, rows.data() + written, n);
      written += n;
      if (do_flushes && (read_u8(Data, Size, off) & 1)) {
        png_write_flush(png_ptr);
      }
    }
  } else if (mode == 2) {
    png_uint_32 split = height ? (read_u8(Data, Size, off) % (height + 1)) : 0;
    if (split > 0) {
      png_write_rows(png_ptr, rows.data(), split);
    }
    if (do_flushes) {
      png_write_flush(png_ptr);
    }
    if (split < height) {
      png_write_rows(png_ptr, rows.data() + split, height - split);
    }
  } else {
    png_write_image(png_ptr, rows.data());
    if (do_flushes) {
      png_write_flush(png_ptr);
    }
  }

  png_write_end(png_ptr, info_ptr);
  png_destroy_write_struct(&png_ptr, &info_ptr);
  fclose(fp);
  return 0;
}