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
#include <cstring>
#include <setjmp.h>

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

static png_uint_32 ReadU32(const uint8_t *data, size_t size, size_t &off) {
  png_uint_32 v = 0;
  for (int i = 0; i < 4; ++i) {
    v <<= 8;
    if (off < size) v |= data[off++];
  }
  return v;
}

static int ReadInt(const uint8_t *data, size_t size, size_t &off) {
  return static_cast<int>(ReadU32(data, size, off));
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE *fp = std::fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    if (Size > 0) std::fwrite(Data, 1, Size, fp);
    std::fclose(fp);
  }

  size_t off = 0;

  png_structp png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn, PngWarningFn);
  if (png_ptr == nullptr) return 0;

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr) {
    png_destroy_write_struct(&png_ptr, nullptr);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    return 0;
  }

  FILE *out = std::fopen("./dummy_file", "wb");
  if (out == nullptr) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    return 0;
  }
  png_init_io(png_ptr, out);

  png_uint_32 width = (ReadU32(Data, Size, off) % 64) + 1;
  png_uint_32 height = (ReadU32(Data, Size, off) % 64) + 1;

  int bit_depth_choices[] = {1, 2, 4, 8, 16};
  int color_type_choices[] = {
      PNG_COLOR_TYPE_GRAY,
      PNG_COLOR_TYPE_PALETTE,
      PNG_COLOR_TYPE_RGB,
      PNG_COLOR_TYPE_RGB_ALPHA,
      PNG_COLOR_TYPE_GRAY_ALPHA
  };

  int bit_depth = bit_depth_choices[(Size > off ? Data[off++] : 0) % 5];
  int color_type = color_type_choices[(Size > off ? Data[off++] : 0) % 5];
  int interlace_type = (Size > off && (Data[off++] & 1)) ? PNG_INTERLACE_ADAM7 : PNG_INTERLACE_NONE;
  int compression_type = PNG_COMPRESSION_TYPE_BASE;
  int filter_method = PNG_FILTER_TYPE_BASE;

  if (color_type == PNG_COLOR_TYPE_PALETTE && bit_depth == 16)
    bit_depth = 8;
  if ((color_type == PNG_COLOR_TYPE_RGB ||
       color_type == PNG_COLOR_TYPE_RGB_ALPHA ||
       color_type == PNG_COLOR_TYPE_GRAY_ALPHA) &&
      bit_depth < 8)
    bit_depth = 8;

  png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, color_type,
               interlace_type, compression_type, filter_method);

  png_uint_32 mng_features = ReadU32(Data, Size, off);
  (void)png_permit_mng_features(png_ptr, mng_features);
  (void)png_permit_mng_features(png_ptr, ~mng_features);

  int keep_values[] = {
      PNG_HANDLE_CHUNK_AS_DEFAULT,
      PNG_HANDLE_CHUNK_NEVER,
      PNG_HANDLE_CHUNK_IF_SAFE,
      PNG_HANDLE_CHUNK_ALWAYS
  };
  int keep = keep_values[(Size > off ? Data[off++] : 0) % 4];

  int max_entries = static_cast<int>((Size - off) / 5);
  int requested_entries = max_entries > 0 ? (ReadInt(Data, Size, off) & 0x7fffffff) % (max_entries + 1) : 0;
  std::vector<png_byte> chunk_list;
  if (requested_entries > 0) {
    chunk_list.resize(static_cast<size_t>(requested_entries) * 5);
    for (int i = 0; i < requested_entries; ++i) {
      for (int j = 0; j < 4; ++j) {
        png_byte c = (off < Size) ? Data[off++] : 0;
        if (c < 32 || c > 126) c = static_cast<png_byte>('A' + (c % 26));
        chunk_list[static_cast<size_t>(i) * 5 + j] = c;
      }
      chunk_list[static_cast<size_t>(i) * 5 + 4] = 0;
    }
    png_set_keep_unknown_chunks(png_ptr, keep, chunk_list.data(), requested_entries);
  } else {
    png_set_keep_unknown_chunks(png_ptr, keep, nullptr, 0);
  }

  std::vector<png_color> palette_storage;
  int palette_size = 0;
  if (color_type == PNG_COLOR_TYPE_PALETTE) {
    palette_size = static_cast<int>((Size > off ? Data[off++] : 0) % 256);
    if (palette_size == 0) palette_size = 1;
    palette_storage.resize(static_cast<size_t>(palette_size));
    for (int i = 0; i < palette_size; ++i) {
      palette_storage[i].red = (off < Size) ? Data[off++] : 0;
      palette_storage[i].green = (off < Size) ? Data[off++] : 0;
      palette_storage[i].blue = (off < Size) ? Data[off++] : 0;
    }
    png_set_PLTE(png_ptr, info_ptr, palette_storage.data(), palette_size);
  }

  png_colorp out_palette = nullptr;
  int out_num_palette = -1;
  (void)png_get_PLTE(png_ptr, info_ptr, &out_palette, &out_num_palette);

  png_color_16 trans_color;
  std::memset(&trans_color, 0, sizeof(trans_color));
  trans_color.index = (off < Size) ? Data[off++] : 0;
  trans_color.red = static_cast<png_uint_16>(ReadU32(Data, Size, off) & 0xFFFFu);
  trans_color.green = static_cast<png_uint_16>(ReadU32(Data, Size, off) & 0xFFFFu);
  trans_color.blue = static_cast<png_uint_16>(ReadU32(Data, Size, off) & 0xFFFFu);
  trans_color.gray = static_cast<png_uint_16>(ReadU32(Data, Size, off) & 0xFFFFu);

  std::vector<png_byte> trans_alpha;
  int num_trans = 0;
  if (color_type == PNG_COLOR_TYPE_PALETTE && palette_size > 0) {
    num_trans = static_cast<int>((Size > off ? Data[off++] : 0) % (palette_size + 1));
    trans_alpha.resize(static_cast<size_t>(num_trans));
    for (int i = 0; i < num_trans; ++i)
      trans_alpha[i] = (off < Size) ? Data[off++] : 0;
    png_set_tRNS(png_ptr, info_ptr,
                 trans_alpha.empty() ? nullptr : trans_alpha.data(),
                 num_trans, &trans_color);
  } else {
    int use_color = (Size > off ? Data[off++] : 0) & 1;
    png_set_tRNS(png_ptr, info_ptr, nullptr, 0, use_color ? &trans_color : nullptr);
  }

  if ((Size > off ? Data[off++] : 0) & 1)
    png_write_info_before_PLTE(png_ptr, info_ptr);
  if ((Size > off ? Data[off++] : 0) & 1)
    png_write_info_before_PLTE(png_ptr, info_ptr);

  png_get_PLTE(png_ptr, info_ptr, &out_palette, &out_num_palette);

  png_write_info(png_ptr, info_ptr);
  if ((Size > off ? Data[off++] : 0) & 1)
    png_write_info(png_ptr, info_ptr);

  std::fclose(out);
  png_destroy_write_struct(&png_ptr, &info_ptr);
  return 0;
}