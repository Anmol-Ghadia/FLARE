// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_text at pngset.c:762:1 in png.h
// png_set_text at pngset.c:762:1 in png.h
// png_set_unknown_chunks at pngset.c:1200:1 in png.h
// png_get_text at pngget.c:1049:1 in png.h
// png_data_freer at png.c:452:1 in png.h
// png_set_invalid at pngset.c:1626:1 in png.h
// png_free_data at png.c:471:1 in png.h
// png_get_text at pngget.c:1049:1 in png.h
// png_set_text at pngset.c:762:1 in png.h
// png_get_text at pngget.c:1049:1 in png.h
// png_set_unknown_chunks at pngset.c:1200:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
#include <png.h>

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

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

static std::string ReadString(const uint8_t *data, size_t size, size_t &off,
                              size_t max_len) {
  size_t len = 0;
  if (off < size) len = data[off++];
  if (len > max_len) len = max_len;
  if (off + len > size) len = size - off;
  std::string out(reinterpret_cast<const char *>(data + off), len);
  off += len;
  return out;
}

static png_byte ReadByte(const uint8_t *data, size_t size, size_t &off) {
  if (off < size) return data[off++];
  return 0;
}

static void PngErrorFn(png_structp, png_const_charp) {}
static void PngWarnFn(png_structp, png_const_charp) {}

extern "C" int LLVMFuzzerTestOneInput_38(const uint8_t *Data, size_t Size) {
  FILE *fp = std::fopen("./dummy_file", "wb");
  if (fp) {
    if (Size) std::fwrite(Data, 1, Size, fp);
    std::fclose(fp);
  }

  png_structp png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn,
                              PngWarnFn);
  if (!png_ptr) return 0;

  png_inforp info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    png_destroy_write_struct(&png_ptr, nullptr);
    return 0;
  }

  size_t off = 0;

  int num_text = static_cast<int>(ReadByte(Data, Size, off) % 4);

  std::vector<std::string> text_keys;
  std::vector<std::string> text_vals;
#ifdef PNG_iTXt_SUPPORTED
  std::vector<std::string> text_langs;
  std::vector<std::string> text_lang_keys;
#endif

  text_keys.reserve(num_text > 0 ? static_cast<size_t>(num_text) : 0);
  text_vals.reserve(num_text > 0 ? static_cast<size_t>(num_text) : 0);
#ifdef PNG_iTXt_SUPPORTED
  text_langs.reserve(num_text > 0 ? static_cast<size_t>(num_text) : 0);
  text_lang_keys.reserve(num_text > 0 ? static_cast<size_t>(num_text) : 0);
#endif

  std::vector<int> text_compressions;
  text_compressions.reserve(num_text > 0 ? static_cast<size_t>(num_text) : 0);

  for (int i = 0; i < num_text; ++i) {
    std::string key = ReadString(Data, Size, off, 32);
    if (key.empty()) key = "K";
    text_keys.push_back(key);
    text_vals.push_back(ReadString(Data, Size, off, 128));
#ifdef PNG_iTXt_SUPPORTED
    text_langs.push_back(ReadString(Data, Size, off, 32));
    text_lang_keys.push_back(ReadString(Data, Size, off, 32));
#endif
    png_byte comp = ReadByte(Data, Size, off);
#ifdef PNG_iTXt_SUPPORTED
    static const int kCompModes[] = {
        PNG_TEXT_COMPRESSION_NONE, PNG_TEXT_COMPRESSION_zTXt,
        PNG_ITXT_COMPRESSION_NONE, PNG_ITXT_COMPRESSION_zTXt};
    text_compressions.push_back(kCompModes[comp % 4]);
#else
    static const int kCompModes[] = {PNG_TEXT_COMPRESSION_NONE,
                                     PNG_TEXT_COMPRESSION_zTXt};
    text_compressions.push_back(kCompModes[comp % 2]);
#endif
  }

  std::vector<png_text> texts;
  texts.resize(num_text > 0 ? static_cast<size_t>(num_text) : 0);

  for (int i = 0; i < num_text; ++i) {
    png_text t;
    std::memset(&t, 0, sizeof(t));

    t.compression = text_compressions[static_cast<size_t>(i)];
    t.key = const_cast<png_charp>(text_keys[static_cast<size_t>(i)].c_str());
    t.text = const_cast<png_charp>(text_vals[static_cast<size_t>(i)].c_str());
    t.text_length = text_vals[static_cast<size_t>(i)].size();

#ifdef PNG_iTXt_SUPPORTED
    t.lang = text_langs[static_cast<size_t>(i)].empty()
                 ? nullptr
                 : const_cast<png_charp>(
                       text_langs[static_cast<size_t>(i)].c_str());
    t.lang_key = text_lang_keys[static_cast<size_t>(i)].empty()
                     ? nullptr
                     : const_cast<png_charp>(
                           text_lang_keys[static_cast<size_t>(i)].c_str());
    t.itxt_length = text_vals[static_cast<size_t>(i)].size();
#endif

    texts[static_cast<size_t>(i)] = t;
  }

  if (!texts.empty()) {
    png_set_text(png_ptr, info_ptr, texts.data(),
                 static_cast<int>(texts.size()));
  } else {
    png_set_text(png_ptr, info_ptr, nullptr, 0);
  }

  int num_unknowns = static_cast<int>(ReadByte(Data, Size, off) % 4);
  std::vector<std::vector<png_byte>> unknown_data_store;
  unknown_data_store.reserve(num_unknowns > 0 ? static_cast<size_t>(num_unknowns)
                                              : 0);
  std::vector<png_unknown_chunk> unknowns;
  unknowns.reserve(num_unknowns > 0 ? static_cast<size_t>(num_unknowns) : 0);

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

    unknown_data_store.emplace_back(Data + off, Data + off + chunk_size);
    off += chunk_size;

    ch.data = unknown_data_store.back().empty() ? nullptr
                                                : unknown_data_store.back().data();
    ch.size = unknown_data_store.back().size();

    png_byte loc_selector = ReadByte(Data, Size, off) % 3;
    switch (loc_selector) {
      case 0:
        ch.location = PNG_HAVE_IHDR;
        break;
      case 1:
        ch.location = PNG_HAVE_PLTE;
        break;
      default:
        ch.location = PNG_AFTER_IDAT;
        break;
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
      0u};

  int freer = (ReadByte(Data, Size, off) & 1) ? PNG_DESTROY_WILL_FREE_DATA
                                              : PNG_USER_WILL_FREE_DATA;
  png_uint_32 chosen_mask =
      masks[ReadByte(Data, Size, off) % (sizeof(masks) / sizeof(masks[0]))];
  png_data_freer(png_ptr, info_ptr, freer, chosen_mask);

  int invalid_mask = ReadInt(Data, Size, off);
  png_set_invalid(png_ptr, info_ptr, invalid_mask);

  png_uint_32 free_mask =
      masks[ReadByte(Data, Size, off) % (sizeof(masks) / sizeof(masks[0]))];
  int free_num;
  if (ReadByte(Data, Size, off) & 1) {
    free_num = -1;
  } else if (out_num_text > 0) {
    int idx = ReadInt(Data, Size, off);
    if (idx < 0) idx = -idx;
    free_num = idx % out_num_text;
  } else {
    free_num = 0;
  }
  png_free_data(png_ptr, info_ptr, free_mask, free_num);

  out_text = nullptr;
  out_num_text = -1;
  (void)png_get_text(png_ptr, info_ptr, &out_text, &out_num_text);

  if (!texts.empty() && (ReadByte(Data, Size, off) & 1)) {
    png_set_text(png_ptr, info_ptr, texts.data(),
                 static_cast<int>(texts.size()));
    (void)png_get_text(png_ptr, info_ptr, &out_text, &out_num_text);
  }

  if (!unknowns.empty() && (ReadByte(Data, Size, off) & 1)) {
    png_set_unknown_chunks(png_ptr, info_ptr, unknowns.data(),
                           static_cast<int>(unknowns.size()));
  }

  png_infop info_to_destroy = info_ptr;
  png_destroy_write_struct(&png_ptr, &info_to_destroy);
  return 0;
}