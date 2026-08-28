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
#include <cstring>
#include <vector>
#include <string>

static uint32_t ReadU32(const uint8_t *data, size_t size, size_t &off) {
  uint32_t v = 0;
  for (int i = 0; i < 4; ++i) {
    v <<= 8;
    if (off < size) v |= data[off++];
  }
  return v;
}

static int ReadInt(const uint8_t *data, size_t size, size_t &off) {
  return static_cast<int>(ReadU32(data, size, off));
}

static std::string ReadString(const uint8_t *data, size_t size, size_t &off, size_t max_len) {
  size_t len = 0;
  if (off < size) len = data[off++];
  if (len > max_len) len = max_len;
  if (off + len > size) len = size - off;
  std::string out(reinterpret_cast<const char*>(data + off), len);
  off += len;
  return out;
}

static png_byte ReadByte(const uint8_t *data, size_t size, size_t &off) {
  if (off < size) return data[off++];
  return 0;
}

static void PngErrorFn(png_structp, png_const_charp) {
  std::abort();
}

static void PngWarnFn(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE *fp = std::fopen("./dummy_file", "wb");
  if (fp) {
    if (Size) std::fwrite(Data, 1, Size, fp);
    std::fclose(fp);
  }

  png_structp png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn, PngWarnFn);
  if (!png_ptr) return 0;

  png_inforp info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    png_destroy_write_struct(&png_ptr, nullptr);
    return 0;
  }

  size_t off = 0;

  std::vector<std::string> text_keys;
  std::vector<std::string> text_vals;
#ifdef PNG_iTXt_SUPPORTED
  std::vector<std::string> text_langs;
  std::vector<std::string> text_lang_keys;
#endif
  std::vector<png_text> texts;

  int num_text = static_cast<int>(ReadByte(Data, Size, off) % 4);
  for (int i = 0; i < num_text; ++i) {
    text_keys.push_back(ReadString(Data, Size, off, 32));
    if (text_keys.back().empty()) text_keys.back() = "K";

    text_vals.push_back(ReadString(Data, Size, off, 128));

    png_text t;
    std::memset(&t, 0, sizeof(t));

    int comp_selector = ReadByte(Data, Size, off) % 4;
    switch (comp_selector) {
      case 0: t.compression = PNG_TEXT_COMPRESSION_NONE; break;
      case 1: t.compression = PNG_TEXT_COMPRESSION_zTXt; break;
#ifdef PNG_iTXt_SUPPORTED
      case 2: t.compression = PNG_ITXT_COMPRESSION_NONE; break;
      case 3: t.compression = PNG_ITXT_COMPRESSION_zTXt; break;
#else
      default: t.compression = PNG_TEXT_COMPRESSION_NONE; break;
#endif
    }

    t.key = const_cast<png_charp>(text_keys.back().c_str());
    t.text = const_cast<png_charp>(text_vals.back().c_str());
    t.text_length = text_vals.back().size();

#ifdef PNG_iTXt_SUPPORTED
    text_langs.push_back(ReadString(Data, Size, off, 32));
    text_lang_keys.push_back(ReadString(Data, Size, off, 32));
    t.lang = text_langs.back().empty() ? nullptr
                                       : const_cast<png_charp>(text_langs.back().c_str());
    t.lang_key = text_lang_keys.back().empty()
                     ? nullptr
                     : const_cast<png_charp>(text_lang_keys.back().c_str());
    t.itxt_length = text_vals.back().size();
#endif

    texts.push_back(t);
  }

  if (!texts.empty()) {
    png_set_text(png_ptr, info_ptr, texts.data(), static_cast<int>(texts.size()));
  } else {
    png_set_text(png_ptr, info_ptr, nullptr, 0);
  }

  std::vector<std::vector<png_byte>> unknown_data_store;
  std::vector<png_unknown_chunk> unknowns;

  int num_unknowns = static_cast<int>(ReadByte(Data, Size, off) % 4);
  for (int i = 0; i < num_unknowns; ++i) {
    png_unknown_chunk ch;
    std::memset(&ch, 0, sizeof(ch));

    for (int j = 0; j < 4; ++j) {
      png_byte c = ReadByte(Data, Size, off);
      if (c == 0) c = static_cast<png_byte>('A' + (j % 26));
      ch.name[j] = c;
    }
    ch.name[4] = 0;

    size_t chunk_size = ReadByte(Data, Size, off);
    if (chunk_size > Size - off) chunk_size = Size - off;

    unknown_data_store.emplace_back();
    unknown_data_store.back().assign(Data + off, Data + off + chunk_size);
    off += chunk_size;

    ch.data = unknown_data_store.back().empty() ? nullptr : unknown_data_store.back().data();
    ch.size = unknown_data_store.back().size();

    png_byte loc_selector = ReadByte(Data, Size, off) % 3;
    switch (loc_selector) {
      case 0: ch.location = PNG_HAVE_IHDR; break;
      case 1: ch.location = PNG_AFTER_IDAT; break;
      default: ch.location = PNG_AFTER_PLTE; break;
    }

    unknowns.push_back(ch);
  }

  if (!unknowns.empty()) {
    png_set_unknown_chunks(png_ptr, info_ptr, unknowns.data(),
                           static_cast<int>(unknowns.size()));
  }

  png_textp out_text = nullptr;
  int out_num_text = -1;
  (void)png_get_text(png_ptr, info_ptr, &out_text, &out_num_text);

  png_uint_32 masks[] = {
#ifdef PNG_FREE_TEXT
      PNG_FREE_TEXT,
#endif
#ifdef PNG_FREE_UNKN
      PNG_FREE_UNKN,
#endif
#ifdef PNG_FREE_ALL
      PNG_FREE_ALL,
#endif
      0u
  };

  int freer_mode = static_cast<int>(ReadByte(Data, Size, off) % 3);
  int freer = PNG_DESTROY_WILL_FREE_DATA;
  if (freer_mode == 1) {
    freer = PNG_USER_WILL_FREE_DATA;
  } else if (freer_mode == 2) {
    freer = 12345;
  }

  png_uint_32 chosen_mask = masks[ReadByte(Data, Size, off) % (sizeof(masks) / sizeof(masks[0]))];
  png_data_freer(png_ptr, info_ptr, freer, chosen_mask);

  int invalid_mask = ReadInt(Data, Size, off);
  png_set_invalid(png_ptr, info_ptr, invalid_mask);

  png_uint_32 free_mask = masks[ReadByte(Data, Size, off) % (sizeof(masks) / sizeof(masks[0]))];
  int free_num;
  if (ReadByte(Data, Size, off) & 1) {
    free_num = -1;
  } else if (out_num_text > 0) {
    free_num = ReadInt(Data, Size, off) % out_num_text;
    if (free_num < 0) free_num = -free_num;
  } else {
    free_num = 0;
  }
  png_free_data(png_ptr, info_ptr, free_mask, free_num);

  out_text = nullptr;
  out_num_text = -1;
  (void)png_get_text(png_ptr, info_ptr, &out_text, &out_num_text);

  if (!texts.empty() && (ReadByte(Data, Size, off) & 1)) {
    png_set_text(png_ptr, info_ptr, texts.data(), static_cast<int>(texts.size()));
    (void)png_get_text(png_ptr, info_ptr, &out_text, &out_num_text);
  }

  if (!unknowns.empty() && (ReadByte(Data, Size, off) & 1)) {
    png_set_unknown_chunks(png_ptr, info_ptr, unknowns.data(),
                           static_cast<int>(unknowns.size()));
  }

  png_destroy_write_struct(&png_ptr, &info_ptr);
  return 0;
}