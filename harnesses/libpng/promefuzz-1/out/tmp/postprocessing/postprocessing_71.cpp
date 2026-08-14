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
#include <string>
#include <fstream>
#include <cstring>
#include <setjmp.h>

static uint32_t ReadU32(const uint8_t *Data, size_t Size, size_t &Off) {
  uint32_t v = 0;
  for (int i = 0; i < 4; ++i) {
    v <<= 8;
    if (Off < Size) v |= Data[Off++];
  }
  return v;
}

static uint8_t ReadU8(const uint8_t *Data, size_t Size, size_t &Off) {
  if (Off < Size) return Data[Off++];
  return 0;
}

static size_t Bounded(size_t v, size_t limit) {
  return limit ? (v % limit) : 0;
}

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarnFn(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  std::ofstream ofs("./dummy_file", std::ios::binary);
  if (ofs) ofs.write(reinterpret_cast<const char*>(Data), static_cast<std::streamsize>(Size));
  ofs.close();

  png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr,
                                                PngErrorFn, PngWarnFn);
  if (!png_ptr) return 0;

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    png_destroy_write_struct(&png_ptr, nullptr);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    return 0;
  }

  FILE *fp = fopen("./dummy_file", "wb");
  if (!fp) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    return 0;
  }

  png_init_io(png_ptr, fp);

  size_t Off = 0;

  const png_uint_32 width = 1 + Bounded(ReadU32(Data, Size, Off), 32);
  const png_uint_32 height = 1 + Bounded(ReadU32(Data, Size, Off), 32);

  const int color_types[] = {
      PNG_COLOR_TYPE_PALETTE,
      PNG_COLOR_TYPE_RGB,
      PNG_COLOR_TYPE_RGBA,
      PNG_COLOR_TYPE_GRAY,
      PNG_COLOR_TYPE_GRAY_ALPHA
  };
  int color_type = color_types[Bounded(ReadU8(Data, Size, Off), sizeof(color_types)/sizeof(color_types[0]))];

  int bit_depth;
  if (color_type == PNG_COLOR_TYPE_PALETTE) {
    const int palette_depths[] = {1, 2, 4, 8};
    bit_depth = palette_depths[Bounded(ReadU8(Data, Size, Off), 4)];
  } else if (color_type == PNG_COLOR_TYPE_GRAY) {
    const int gray_depths[] = {1, 2, 4, 8, 16};
    bit_depth = gray_depths[Bounded(ReadU8(Data, Size, Off), 5)];
  } else {
    const int other_depths[] = {8, 16};
    bit_depth = other_depths[Bounded(ReadU8(Data, Size, Off), 2)];
  }

  png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, color_type,
               PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

  int max_palette = (color_type == PNG_COLOR_TYPE_PALETTE) ? (1 << bit_depth) : PNG_MAX_PALETTE_LENGTH;
  if (max_palette <= 0) max_palette = 1;
  int num_palette = 1 + static_cast<int>(Bounded(ReadU8(Data, Size, Off), static_cast<size_t>(max_palette)));

  std::vector<png_color> palette(static_cast<size_t>(num_palette));
  for (int i = 0; i < num_palette; ++i) {
    palette[i].red = ReadU8(Data, Size, Off);
    palette[i].green = ReadU8(Data, Size, Off);
    palette[i].blue = ReadU8(Data, Size, Off);
  }

  png_set_PLTE(png_ptr, info_ptr, palette.data(), num_palette);

  std::vector<png_uint_16> hist(static_cast<size_t>(num_palette));
  for (int i = 0; i < num_palette; ++i) {
    hist[i] = static_cast<png_uint_16>((ReadU8(Data, Size, Off) << 8) | ReadU8(Data, Size, Off));
  }
  png_set_hIST(png_ptr, info_ptr, hist.data());

  int splt_count = static_cast<int>(Bounded(ReadU8(Data, Size, Off), 4)) + 1;
  std::vector<png_sPLT_t> splt_vec(static_cast<size_t>(splt_count));
  std::vector<std::string> splt_names(static_cast<size_t>(splt_count));
  std::vector<std::vector<png_sPLT_entry>> splt_entries(static_cast<size_t>(splt_count));

  for (int i = 0; i < splt_count; ++i) {
    size_t name_len = 1 + Bounded(ReadU8(Data, Size, Off), 15);
    splt_names[i].resize(name_len);
    for (size_t j = 0; j < name_len; ++j) {
      char c = static_cast<char>('A' + (ReadU8(Data, Size, Off) % 26));
      splt_names[i][j] = c;
    }

    int depth_choice = ReadU8(Data, Size, Off) & 1;
    png_byte depth = depth_choice ? 16 : 8;

    int entry_count = 1 + static_cast<int>(Bounded(ReadU8(Data, Size, Off), 8));
    splt_entries[i].resize(static_cast<size_t>(entry_count));
    for (int j = 0; j < entry_count; ++j) {
      splt_entries[i][j].red = static_cast<png_uint_16>((ReadU8(Data, Size, Off) << 8) | ReadU8(Data, Size, Off));
      splt_entries[i][j].green = static_cast<png_uint_16>((ReadU8(Data, Size, Off) << 8) | ReadU8(Data, Size, Off));
      splt_entries[i][j].blue = static_cast<png_uint_16>((ReadU8(Data, Size, Off) << 8) | ReadU8(Data, Size, Off));
      splt_entries[i][j].alpha = static_cast<png_uint_16>((ReadU8(Data, Size, Off) << 8) | ReadU8(Data, Size, Off));
      splt_entries[i][j].frequency = static_cast<png_uint_16>((ReadU8(Data, Size, Off) << 8) | ReadU8(Data, Size, Off));
    }

    std::memset(&splt_vec[i], 0, sizeof(splt_vec[i]));
    splt_vec[i].name = const_cast<png_charp>(splt_names[i].c_str());
    splt_vec[i].depth = depth;
    splt_vec[i].entries = splt_entries[i].data();
    splt_vec[i].nentries = entry_count;
  }

  png_set_sPLT(png_ptr, info_ptr, splt_vec.data(), splt_count);

  png_sPLT_t *retrieved = nullptr;
  (void)png_get_sPLT(png_ptr, info_ptr, &retrieved);

  int unknown_count = static_cast<int>(Bounded(ReadU8(Data, Size, Off), 4)) + 1;
  std::vector<png_unknown_chunk> unknowns(static_cast<size_t>(unknown_count));
  std::vector<std::vector<png_byte>> unknown_datas(static_cast<size_t>(unknown_count));

  const png_byte locations[] = {
      PNG_HAVE_IHDR,
      PNG_HAVE_PLTE,
      PNG_AFTER_IDAT
  };

  for (int i = 0; i < unknown_count; ++i) {
    std::memset(&unknowns[i], 0, sizeof(unknowns[i]));
    for (int j = 0; j < 4; ++j) {
      unknowns[i].name[j] = static_cast<png_byte>('A' + (ReadU8(Data, Size, Off) % 26));
    }
    unknowns[i].name[4] = 0;

    size_t chunk_size = Bounded(ReadU8(Data, Size, Off), 32);
    unknown_datas[i].resize(chunk_size);
    for (size_t j = 0; j < chunk_size; ++j) {
      unknown_datas[i][j] = ReadU8(Data, Size, Off);
    }

    unknowns[i].data = unknown_datas[i].empty() ? nullptr : unknown_datas[i].data();
    unknowns[i].size = unknown_datas[i].size();
    unknowns[i].location = locations[Bounded(ReadU8(Data, Size, Off), 3)];
  }

  png_set_unknown_chunks(png_ptr, info_ptr, unknowns.data(), unknown_count);

  int channels;
  switch (color_type) {
    case PNG_COLOR_TYPE_GRAY: channels = 1; break;
    case PNG_COLOR_TYPE_GRAY_ALPHA: channels = 2; break;
    case PNG_COLOR_TYPE_RGB: channels = 3; break;
    case PNG_COLOR_TYPE_RGBA: channels = 4; break;
    case PNG_COLOR_TYPE_PALETTE: channels = 1; break;
    default: channels = 4; break;
  }

  size_t rowbytes = 0;
  if (bit_depth < 8 && color_type == PNG_COLOR_TYPE_PALETTE) {
    rowbytes = (static_cast<size_t>(width) * static_cast<size_t>(bit_depth) + 7) / 8;
  } else if (bit_depth < 8 && color_type == PNG_COLOR_TYPE_GRAY) {
    rowbytes = (static_cast<size_t>(width) * static_cast<size_t>(bit_depth) + 7) / 8;
  } else {
    rowbytes = static_cast<size_t>(width) * static_cast<size_t>(channels) * static_cast<size_t>(bit_depth / 8);
  }
  if (rowbytes == 0) rowbytes = 1;

  std::vector<png_bytep> rows(static_cast<size_t>(height));
  std::vector<std::vector<png_byte>> image(static_cast<size_t>(height));
  for (size_t y = 0; y < height; ++y) {
    image[y].resize(rowbytes);
    for (size_t x = 0; x < rowbytes; ++x) {
      image[y][x] = ReadU8(Data, Size, Off);
    }
    rows[y] = image[y].data();
  }

  png_set_rows(png_ptr, info_ptr, rows.data());

  int transforms = 0;
  if (ReadU8(Data, Size, Off) & 1) transforms |= PNG_TRANSFORM_PACKING;
  if (ReadU8(Data, Size, Off) & 1) transforms |= PNG_TRANSFORM_SHIFT;
  if (ReadU8(Data, Size, Off) & 1) transforms |= PNG_TRANSFORM_SWAP_ENDIAN;
  if (ReadU8(Data, Size, Off) & 1) transforms |= PNG_TRANSFORM_INVERT_ALPHA;
  if (ReadU8(Data, Size, Off) & 1) transforms |= PNG_TRANSFORM_BGR;
  if (ReadU8(Data, Size, Off) & 1) transforms |= PNG_TRANSFORM_SWAP_ALPHA;
  if (ReadU8(Data, Size, Off) & 1) transforms |= PNG_TRANSFORM_INVERT_MONO;

  png_write_png(png_ptr, info_ptr, transforms, nullptr);

  fclose(fp);
  png_destroy_write_struct(&png_ptr, &info_ptr);
  return 0;
}