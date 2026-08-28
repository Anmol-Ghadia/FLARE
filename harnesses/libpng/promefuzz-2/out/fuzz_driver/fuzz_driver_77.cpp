// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_IHDR at pngset.c:254:1 in png.h
// png_set_PLTE at pngset.c:572:1 in png.h
// png_set_text at pngset.c:762:1 in png.h
// png_set_sPLT at pngset.c:1062:1 in png.h
// png_set_unknown_chunks at pngset.c:1200:1 in png.h
// png_set_unknown_chunk_location at pngset.c:1299:1 in png.h
// png_write_info_before_PLTE at pngwrite.c:84:1 in png.h
// png_write_info at pngwrite.c:192:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_init_io at png.c:703:1 in png.h
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
#include <cstdio>
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

static size_t BoundedLen(const uint8_t *Data, size_t Size, size_t &Off, size_t MaxLen) {
  uint32_t raw = ReadU32(Data, Size, Off);
  return static_cast<size_t>(raw % (MaxLen + 1));
}

static std::string ReadString(const uint8_t *Data, size_t Size, size_t &Off, size_t MaxLen) {
  size_t len = BoundedLen(Data, Size, Off, MaxLen);
  if (Off + len > Size) len = Size - Off;
  std::string s(reinterpret_cast<const char*>(Data + Off), len);
  Off += len;
  return s;
}

static std::string MakeCStringSafe(const std::string &in, const char *fallback) {
  std::string out = in;
  for (size_t i = 0; i < out.size(); ++i) {
    if (out[i] == '\0') out[i] = '_';
  }
  if (out.empty()) out = fallback;
  return out;
}

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarnFn(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  if (Data == nullptr || Size == 0) return 0;

  FILE *fp = std::fopen("./dummy_file", "wb");
  if (fp) {
    std::fwrite(Data, 1, Size, fp);
    std::fclose(fp);
  }

  png_structp png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn, PngWarnFn);
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

  FILE *out = std::fopen("./dummy_file", "wb");
  if (!out) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    return 0;
  }

  png_init_io(png_ptr, out);

  size_t Off = 0;

  int color_types[] = {
      PNG_COLOR_TYPE_GRAY,
      PNG_COLOR_TYPE_RGB,
      PNG_COLOR_TYPE_PALETTE,
      PNG_COLOR_TYPE_GRAY_ALPHA,
      PNG_COLOR_TYPE_RGB_ALPHA
  };
  int bit_depths[] = {1, 2, 4, 8, 16};

  int color_type = color_types[ReadU8(Data, Size, Off) % 5];
  int bit_depth = bit_depths[ReadU8(Data, Size, Off) % 5];

  if (color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_RGB_ALPHA ||
      color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
    if (bit_depth < 8) bit_depth = 8;
  }
  if (color_type == PNG_COLOR_TYPE_PALETTE && bit_depth == 16) bit_depth = 8;

  png_uint_32 width = (ReadU32(Data, Size, Off) % 32) + 1;
  png_uint_32 height = (ReadU32(Data, Size, Off) % 32) + 1;
  int interlace = (ReadU8(Data, Size, Off) & 1) ? PNG_INTERLACE_ADAM7 : PNG_INTERLACE_NONE;

  png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, color_type, interlace,
               PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

  std::vector<png_color> palette_storage;
  if (color_type == PNG_COLOR_TYPE_PALETTE) {
    size_t pal_size = (ReadU8(Data, Size, Off) % 16) + 1;
    palette_storage.resize(pal_size);
    for (size_t i = 0; i < pal_size; ++i) {
      palette_storage[i].red = ReadU8(Data, Size, Off);
      palette_storage[i].green = ReadU8(Data, Size, Off);
      palette_storage[i].blue = ReadU8(Data, Size, Off);
    }
    png_set_PLTE(png_ptr, info_ptr, palette_storage.data(), static_cast<int>(palette_storage.size()));
  }

  std::vector<std::string> text_keys;
  std::vector<std::string> text_vals;
  std::vector<png_text> texts;
  int num_text = static_cast<int>(ReadU8(Data, Size, Off) % 4);
  texts.resize(num_text);
  text_keys.reserve(num_text);
  text_vals.reserve(num_text);

  for (int i = 0; i < num_text; ++i) {
    std::string key = MakeCStringSafe(ReadString(Data, Size, Off, 32), "K");
    std::string val = MakeCStringSafe(ReadString(Data, Size, Off, 128), "");
    text_keys.push_back(key);
    text_vals.push_back(val);

    std::memset(&texts[i], 0, sizeof(texts[i]));
    int comp_modes[] = {
        PNG_TEXT_COMPRESSION_NONE,
        PNG_TEXT_COMPRESSION_zTXt,
        PNG_ITXT_COMPRESSION_NONE,
        PNG_ITXT_COMPRESSION_zTXt
    };
    texts[i].compression = comp_modes[ReadU8(Data, Size, Off) % 4];
    texts[i].key = const_cast<png_charp>(text_keys.back().c_str());
    texts[i].text = const_cast<png_charp>(text_vals.back().c_str());
    texts[i].text_length = text_vals.back().size();
#ifdef PNG_iTXt_SUPPORTED
    texts[i].itxt_length = text_vals.back().size();
    texts[i].lang = nullptr;
    texts[i].lang_key = nullptr;
#endif
  }

  if (!texts.empty()) {
    png_set_text(png_ptr, info_ptr, texts.data(), static_cast<int>(texts.size()));
  }

  std::vector<std::string> splt_names;
  std::vector<std::vector<png_sPLT_entry>> splt_entries_storage;
  std::vector<png_sPLT_t> splts;
  int num_splt = static_cast<int>(ReadU8(Data, Size, Off) % 3);
  splts.resize(num_splt);
  splt_names.reserve(num_splt);
  splt_entries_storage.resize(num_splt);

  for (int i = 0; i < num_splt; ++i) {
    std::string name = MakeCStringSafe(ReadString(Data, Size, Off, 32), "SPLT");
    splt_names.push_back(name);

    size_t nentries = (ReadU8(Data, Size, Off) % 8) + 1;
    splt_entries_storage[i].resize(nentries);
    for (size_t j = 0; j < nentries; ++j) {
      png_sPLT_entry &e = splt_entries_storage[i][j];
      e.red = static_cast<png_uint_16>(ReadU8(Data, Size, Off) * 257u);
      e.green = static_cast<png_uint_16>(ReadU8(Data, Size, Off) * 257u);
      e.blue = static_cast<png_uint_16>(ReadU8(Data, Size, Off) * 257u);
      e.alpha = static_cast<png_uint_16>(ReadU8(Data, Size, Off) * 257u);
      e.frequency = static_cast<png_uint_16>(ReadU32(Data, Size, Off) & 0xFFFFu);
    }

    std::memset(&splts[i], 0, sizeof(splts[i]));
    splts[i].name = const_cast<png_charp>(splt_names.back().c_str());
    splts[i].depth = (ReadU8(Data, Size, Off) & 1) ? 16 : 8;
    splts[i].entries = splt_entries_storage[i].data();
    splts[i].nentries = static_cast<png_int_32>(splt_entries_storage[i].size());
  }

  if (!splts.empty()) {
    png_set_sPLT(png_ptr, info_ptr, splts.data(), static_cast<int>(splts.size()));
  }

  std::vector<std::vector<png_byte>> unknown_data_storage;
  std::vector<png_unknown_chunk> unknowns;
  int num_unknowns = static_cast<int>(ReadU8(Data, Size, Off) % 4);
  unknowns.resize(num_unknowns);
  unknown_data_storage.resize(num_unknowns);

  for (int i = 0; i < num_unknowns; ++i) {
    std::memset(&unknowns[i], 0, sizeof(unknowns[i]));
    unknowns[i].name[0] = 'a' + (ReadU8(Data, Size, Off) % 26);
    unknowns[i].name[1] = 'A' + (ReadU8(Data, Size, Off) % 26);
    unknowns[i].name[2] = '0' + (ReadU8(Data, Size, Off) % 10);
    unknowns[i].name[3] = 'a' + (ReadU8(Data, Size, Off) % 26);
    unknowns[i].name[4] = 0;

    size_t chunk_size = ReadU8(Data, Size, Off) % 32;
    unknown_data_storage[i].resize(chunk_size);
    for (size_t j = 0; j < chunk_size; ++j) {
      unknown_data_storage[i][j] = ReadU8(Data, Size, Off);
    }
    unknowns[i].data = chunk_size ? unknown_data_storage[i].data() : nullptr;
    unknowns[i].size = chunk_size;

    int loc_choice = ReadU8(Data, Size, Off) % 4;
    switch (loc_choice) {
      case 0: unknowns[i].location = PNG_HAVE_IHDR; break;
      case 1: unknowns[i].location = PNG_HAVE_PLTE; break;
      case 2: unknowns[i].location = PNG_AFTER_IDAT; break;
      default: unknowns[i].location = static_cast<png_byte>(ReadU8(Data, Size, Off)); break;
    }
  }

  if (!unknowns.empty()) {
    png_set_unknown_chunks(png_ptr, info_ptr, unknowns.data(), static_cast<int>(unknowns.size()));
    for (int i = 0; i < num_unknowns; ++i) {
      int loc_choice = ReadU8(Data, Size, Off) % 5;
      int loc = 0;
      switch (loc_choice) {
        case 0: loc = PNG_HAVE_IHDR; break;
        case 1: loc = PNG_HAVE_PLTE; break;
        case 2: loc = PNG_AFTER_IDAT; break;
        case 3: loc = PNG_HAVE_IHDR | PNG_HAVE_PLTE; break;
        default: loc = ReadU8(Data, Size, Off); break;
      }
      png_set_unknown_chunk_location(png_ptr, info_ptr, i, loc);
    }
  }

  if (ReadU8(Data, Size, Off) & 1) {
    png_write_info_before_PLTE(png_ptr, info_ptr);
  }

  png_write_info(png_ptr, info_ptr);

  png_destroy_write_struct(&png_ptr, &info_ptr);
  std::fclose(out);
  return 0;
}