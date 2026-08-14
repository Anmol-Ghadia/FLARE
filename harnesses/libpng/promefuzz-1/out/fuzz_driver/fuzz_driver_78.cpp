// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_IHDR at pngset.c:254:1 in png.h
// png_set_sBIT at pngset.c:638:1 in png.h
// png_set_PLTE at pngset.c:572:1 in png.h
// png_set_PLTE at pngset.c:572:1 in png.h
// png_set_text at pngset.c:762:1 in png.h
// png_set_filter at pngwrite.c:995:1 in png.h
// png_write_info_before_PLTE at pngwrite.c:84:1 in png.h
// png_write_info_before_PLTE at pngwrite.c:84:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_init_io at png.c:703:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_info_init_3 at png.c:423:1 in png.h
// png_destroy_info_struct at png.c:386:1 in png.h
#include <png.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
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

static png_byte ReadByte(const uint8_t *data, size_t size, size_t &off) {
  if (off < size) return data[off++];
  return 0;
}

static size_t ConsumeString(const uint8_t *data, size_t size, size_t &off,
                            char *out, size_t out_cap) {
  if (out_cap == 0) return 0;
  size_t i = 0;
  while (off < size && i + 1 < out_cap) {
    uint8_t c = data[off++];
    if (c == 0) break;
    out[i++] = static_cast<char>(c);
  }
  out[i] = '\0';
  return i;
}

static void PngErrorFn(png_structp, png_const_charp) {
}

static void PngWarningFn(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  size_t off = 0;

  FILE *fp = fopen("./dummy_file", "wb+");
  if (!fp) return 0;
  if (Size > 0) fwrite(Data, 1, Size, fp);
  fflush(fp);
  rewind(fp);

  png_structp png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn, PngWarningFn);
  if (!png_ptr) {
    fclose(fp);
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) {
    png_destroy_write_struct(&png_ptr, nullptr);
    fclose(fp);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
    return 0;
  }

  png_init_io(png_ptr, fp);

  png_infop compat_info = png_create_info_struct(png_ptr);
  if (compat_info != nullptr) {
    png_info_init_3(&compat_info, 0);
    png_destroy_info_struct(png_ptr, &compat_info);
  }

  png_uint_32 width = (ReadU32(Data, Size, off) % 32) + 1;
  png_uint_32 height = (ReadU32(Data, Size, off) % 32) + 1;

  const int color_types[] = {
      PNG_COLOR_TYPE_GRAY,
      PNG_COLOR_TYPE_RGB,
      PNG_COLOR_TYPE_PALETTE,
      PNG_COLOR_TYPE_GRAY_ALPHA,
      PNG_COLOR_TYPE_RGB_ALPHA
  };
  int color_type = color_types[ReadByte(Data, Size, off) % 5];

  int bit_depth_choices[4];
  size_t bd_count = 0;
  switch (color_type) {
    case PNG_COLOR_TYPE_PALETTE:
      bit_depth_choices[0] = 1;
      bit_depth_choices[1] = 2;
      bit_depth_choices[2] = 4;
      bit_depth_choices[3] = 8;
      bd_count = 4;
      break;
    case PNG_COLOR_TYPE_GRAY:
      bit_depth_choices[0] = 1;
      bit_depth_choices[1] = 2;
      bit_depth_choices[2] = 4;
      bit_depth_choices[3] = 8;
      bd_count = 4;
      break;
    default:
      bit_depth_choices[0] = 8;
      bit_depth_choices[1] = 16;
      bd_count = 2;
      break;
  }
  int bit_depth = bit_depth_choices[ReadByte(Data, Size, off) % bd_count];

  png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, color_type,
               PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE,
               PNG_FILTER_TYPE_BASE);

  png_color_8 sig_bit;
  memset(&sig_bit, 0, sizeof(sig_bit));
  sig_bit.red = ReadByte(Data, Size, off);
  sig_bit.green = ReadByte(Data, Size, off);
  sig_bit.blue = ReadByte(Data, Size, off);
  sig_bit.gray = ReadByte(Data, Size, off);
  sig_bit.alpha = ReadByte(Data, Size, off);
  png_set_sBIT(png_ptr, info_ptr, &sig_bit);

  int max_palette = (color_type == PNG_COLOR_TYPE_PALETTE) ? (1 << bit_depth) : 256;
  int requested_palette = ReadInt(Data, Size, off);
  if (requested_palette < 0) requested_palette = -requested_palette;
  int num_palette = max_palette > 0 ? (requested_palette % (max_palette + 1)) : 0;

  std::vector<png_color> palette_storage;
  if (num_palette > 0) {
    palette_storage.resize(static_cast<size_t>(num_palette));
    for (int i = 0; i < num_palette; ++i) {
      palette_storage[i].red = ReadByte(Data, Size, off);
      palette_storage[i].green = ReadByte(Data, Size, off);
      palette_storage[i].blue = ReadByte(Data, Size, off);
    }
    png_set_PLTE(png_ptr, info_ptr, palette_storage.data(), num_palette);
  } else {
    png_set_PLTE(png_ptr, info_ptr, nullptr, 0);
  }

  png_text text_entries[2];
  memset(text_entries, 0, sizeof(text_entries));
  char key1[32], val1[128], key2[32], val2[128];
  ConsumeString(Data, Size, off, key1, sizeof(key1));
  ConsumeString(Data, Size, off, val1, sizeof(val1));
  ConsumeString(Data, Size, off, key2, sizeof(key2));
  ConsumeString(Data, Size, off, val2, sizeof(val2));

  if (key1[0] == '\0') memcpy(key1, "K", 2);
  if (key2[0] == '\0') memcpy(key2, "L", 2);

  text_entries[0].compression = (ReadByte(Data, Size, off) % 4) - 1;
  text_entries[0].key = key1;
  text_entries[0].text = val1;
  text_entries[0].text_length = strlen(val1);

  text_entries[1].compression = (ReadByte(Data, Size, off) % 4) - 1;
  text_entries[1].key = key2;
  text_entries[1].text = val2;
  text_entries[1].text_length = strlen(val2);

  int num_text = ReadByte(Data, Size, off) % 3;
  if (num_text > 0) {
    png_set_text(png_ptr, info_ptr, text_entries, num_text);
  }

  int method = (ReadByte(Data, Size, off) % 3 == 0) ? 1 : PNG_FILTER_TYPE_BASE;
  int filters = ReadInt(Data, Size, off);
  png_set_filter(png_ptr, method, filters);

  png_write_info_before_PLTE(png_ptr, info_ptr);
  if (ReadByte(Data, Size, off) & 1) {
    png_write_info_before_PLTE(png_ptr, info_ptr);
  }

  png_destroy_write_struct(&png_ptr, &info_ptr);
  fclose(fp);
  return 0;
}