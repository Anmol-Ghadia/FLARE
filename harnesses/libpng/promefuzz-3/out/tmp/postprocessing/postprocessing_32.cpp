#include <png.h>

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <setjmp.h>
#include <vector>

struct FuzzState {
  std::vector<png_byte> output;
};

static void FuzzWriteData(png_structp png_ptr, png_bytep data, png_size_t length) {
  FuzzState* state = reinterpret_cast<FuzzState*>(png_get_io_ptr(png_ptr));
  if (state == nullptr || data == nullptr || length == 0) return;
  state->output.insert(state->output.end(), data, data + length);
}

static void FuzzFlush(png_structp) {
}

static uint32_t ReadU32(const uint8_t* data, size_t size, size_t& off) {
  if (off + 4 > size) {
    off = size;
    return 0;
  }
  uint32_t v = (static_cast<uint32_t>(data[off]) << 24) |
               (static_cast<uint32_t>(data[off + 1]) << 16) |
               (static_cast<uint32_t>(data[off + 2]) << 8) |
               static_cast<uint32_t>(data[off + 3]);
  off += 4;
  return v;
}

static png_byte ReadU8(const uint8_t* data, size_t size, size_t& off) {
  if (off >= size) {
    off = size;
    return 0;
  }
  return data[off++];
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE* fp = std::fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    if (Size > 0) std::fwrite(Data, 1, Size, fp);
    std::fclose(fp);
  }

  size_t off = 0;
  FuzzState state;

  png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (png_ptr == nullptr) return 0;

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr) {
    png_destroy_write_struct(&png_ptr, nullptr);
    return 0;
  }

  bool rows_allocated = false;
  std::vector<std::vector<png_byte> > rows;
  std::vector<png_bytep> row_ptrs;

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    return 0;
  }

  png_set_write_fn(png_ptr, &state, FuzzWriteData, FuzzFlush);

  png_write_sig(png_ptr);

  png_byte chunk_name[4];
  for (int i = 0; i < 4; ++i) {
    png_byte c = ReadU8(Data, Size, off);
    if (c < 32 || c > 126) c = static_cast<png_byte>('A' + (c % 26));
    chunk_name[i] = c;
  }

  png_uint_32 chunk_len = ReadU32(Data, Size, off) % 64;
  png_write_chunk_start(png_ptr, chunk_name, chunk_len);

  size_t available = (off <= Size) ? (Size - off) : 0;
  size_t first_part = available ? (ReadU8(Data, Size, off) % (available + 1)) : 0;
  if (first_part > chunk_len) first_part = chunk_len;

  if (first_part > 0 && off + first_part <= Size) {
    png_write_chunk_data(png_ptr,
                         reinterpret_cast<png_const_bytep>(Data + off),
                         first_part);
    off += first_part;
  } else {
    png_write_chunk_data(png_ptr, nullptr, 0);
    first_part = 0;
  }

  size_t remaining_chunk = chunk_len - first_part;
  size_t available2 = (off <= Size) ? (Size - off) : 0;
  size_t second_part = remaining_chunk;
  if (second_part > available2) second_part = available2;

  if (second_part > 0 && off + second_part <= Size) {
    png_write_chunk_data(png_ptr,
                         reinterpret_cast<png_const_bytep>(Data + off),
                         second_part);
    off += second_part;
  } else {
    png_write_chunk_data(png_ptr, nullptr, 0);
    second_part = 0;
  }

  if (remaining_chunk > second_part) {
    std::vector<png_byte> pad(remaining_chunk - second_part, 0);
    if (!pad.empty()) png_write_chunk_data(png_ptr, pad.data(), pad.size());
  }

  png_write_chunk_end(png_ptr);

  int save_flag = static_cast<int>(ReadU8(Data, Size, off) & 1);
  (void)png_process_data_pause(png_ptr, save_flag);
  (void)png_process_data_pause(png_ptr, save_flag ^ 1);

  png_uint_32 width = (ReadU32(Data, Size, off) % 8) + 1;
  png_uint_32 height = (ReadU32(Data, Size, off) % 8) + 1;
  int color_type_choices[] = {
      PNG_COLOR_TYPE_GRAY,
      PNG_COLOR_TYPE_RGB,
      PNG_COLOR_TYPE_PALETTE,
      PNG_COLOR_TYPE_GRAY_ALPHA,
      PNG_COLOR_TYPE_RGB_ALPHA
  };
  int bit_depth_choices[] = {1, 2, 4, 8, 16};

  int color_type = color_type_choices[ReadU8(Data, Size, off) % 5];
  int bit_depth = bit_depth_choices[ReadU8(Data, Size, off) % 5];

  if ((color_type == PNG_COLOR_TYPE_RGB ||
       color_type == PNG_COLOR_TYPE_GRAY_ALPHA ||
       color_type == PNG_COLOR_TYPE_RGB_ALPHA) &&
      bit_depth < 8) {
    bit_depth = 8;
  }

  if (color_type == PNG_COLOR_TYPE_PALETTE && bit_depth == 16) {
    bit_depth = 8;
  }

  int interlace_type = (ReadU8(Data, Size, off) & 1) ? PNG_INTERLACE_ADAM7 : PNG_INTERLACE_NONE;
  png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, color_type,
               interlace_type, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

  png_color palette[256];
  if (color_type == PNG_COLOR_TYPE_PALETTE) {
    int palette_entries = static_cast<int>((ReadU8(Data, Size, off) % 256) + 1);
    for (int i = 0; i < palette_entries; ++i) {
      palette[i].red = ReadU8(Data, Size, off);
      palette[i].green = ReadU8(Data, Size, off);
      palette[i].blue = ReadU8(Data, Size, off);
    }
    png_set_PLTE(png_ptr, info_ptr, palette, palette_entries);
  }

  png_write_info(png_ptr, info_ptr);

  png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);
  if (rowbytes == 0 || rowbytes > (1u << 20)) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    return 0;
  }

  rows.reserve(height);
  row_ptrs.reserve(height);
  for (png_uint_32 y = 0; y < height; ++y) {
    rows.emplace_back(rowbytes);
    for (png_size_t x = 0; x < rowbytes; ++x) {
      rows.back()[x] = (off < Size) ? Data[off++] : static_cast<uint8_t>(x + y);
    }
  }
  for (png_uint_32 y = 0; y < height; ++y) {
    row_ptrs.push_back(rows[y].data());
  }
  rows_allocated = true;

  if (ReadU8(Data, Size, off) & 1) {
    png_write_image(png_ptr, row_ptrs.data());
  } else {
    for (png_uint_32 y = 0; y < height; ++y) {
      png_write_row(png_ptr, row_ptrs[y]);
    }
  }

  png_write_end(png_ptr, info_ptr);

  if (rows_allocated) {
    row_ptrs.clear();
    rows.clear();
  }

  png_destroy_write_struct(&png_ptr, &info_ptr);
  return 0;
}