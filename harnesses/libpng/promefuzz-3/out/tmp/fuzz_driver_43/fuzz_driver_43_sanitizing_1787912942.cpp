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

static void PngReadCallback(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead) {
  png_voidp io_ptr = png_get_io_ptr(png_ptr);
  if (io_ptr == nullptr) {
    png_error(png_ptr, "no io_ptr");
    return;
  }

  struct Reader {
    const uint8_t* data;
    size_t size;
    size_t offset;
  };

  Reader* r = static_cast<Reader*>(io_ptr);
  if (r->offset > r->size || byteCountToRead > r->size - r->offset) {
    png_error(png_ptr, "read overflow");
    return;
  }

  memcpy(outBytes, r->data + r->offset, byteCountToRead);
  r->offset += byteCountToRead;
}

static png_structp CreateReadStruct() {
  return png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  if (Data == nullptr || Size == 0) {
    return 0;
  }

  {
    FILE* fp = fopen("./dummy_file", "wb");
    if (fp != nullptr) {
      fwrite(Data, 1, Size, fp);
      fclose(fp);
    }
  }

  png_structp png_ptr = CreateReadStruct();
  if (png_ptr == nullptr) {
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr) {
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    return 0;
  }

  png_infop end_info = png_create_info_struct(png_ptr);
  if (end_info == nullptr) {
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    return 0;
  }

  struct Reader {
    const uint8_t* data;
    size_t size;
    size_t offset;
  } reader = { Data, Size, 0 };

  png_set_read_fn(png_ptr, &reader, PngReadCallback);

  png_read_info(png_ptr, info_ptr);

  png_bytep trans_alpha = nullptr;
  int num_trans = 0;
  png_color_16p trans_color = nullptr;
  (void)png_get_tRNS(png_ptr, info_ptr, &trans_alpha, &num_trans, &trans_color);

  png_charp iccp_name = nullptr;
  int compression_type = 0;
  png_bytep profile = nullptr;
  png_uint_32 proflen = 0;
  (void)png_get_iCCP(png_ptr, info_ptr, &iccp_name, &compression_type, &profile, &proflen);

  png_uint_32 num_exif = 0;
  png_bytep exif = nullptr;
  (void)png_get_eXIf_1(png_ptr, info_ptr, &num_exif, &exif);

  png_sPLT_tp splt_entries = nullptr;
  int existing_splt = png_get_sPLT(png_ptr, info_ptr, &splt_entries);
  (void)existing_splt;

  std::vector<png_sPLT_entry> palette_entries;
  size_t available = Size > reader.offset ? (Size - reader.offset) : 0;
  size_t count = available / 6;
  if (count == 0) count = 1;
  if (count > 64) count = 64;
  palette_entries.resize(count);

  for (size_t i = 0; i < count; ++i) {
    size_t base = reader.offset + i * 6;
    uint8_t b0 = (base + 0 < Size) ? Data[base + 0] : 0;
    uint8_t b1 = (base + 1 < Size) ? Data[base + 1] : 0;
    uint8_t b2 = (base + 2 < Size) ? Data[base + 2] : 0;
    uint8_t b3 = (base + 3 < Size) ? Data[base + 3] : 0;
    uint8_t b4 = (base + 4 < Size) ? Data[base + 4] : 0;
    uint8_t b5 = (base + 5 < Size) ? Data[base + 5] : 0;

    palette_entries[i].red = b0;
    palette_entries[i].green = b1;
    palette_entries[i].blue = b2;
    palette_entries[i].alpha = b3;
    palette_entries[i].frequency = static_cast<png_uint_16>((static_cast<unsigned>(b4) << 8) | b5);
  }

  char name1[] = "fuzz";
  char name2[] = "alt";

  png_sPLT_t splt_list[2];
  memset(splt_list, 0, sizeof(splt_list));

  splt_list[0].name = name1;
  splt_list[0].depth = (Size > 0 && (Data[0] & 1)) ? 16 : 8;
  splt_list[0].entries = palette_entries.data();
  splt_list[0].nentries = static_cast<png_int_32>(palette_entries.size());

  splt_list[1].name = name2;
  splt_list[1].depth = (Size > 1 && (Data[1] & 1)) ? 16 : 8;
  splt_list[1].entries = palette_entries.data();
  splt_list[1].nentries = static_cast<png_int_32>(palette_entries.size() > 1 ? palette_entries.size() / 2 : 1);

  png_set_sPLT(png_ptr, info_ptr, &splt_list[0], 1);
  png_set_sPLT(png_ptr, info_ptr, splt_list, 2);

  png_sPLT_tp out_entries = nullptr;
  (void)png_get_sPLT(png_ptr, info_ptr, &out_entries);

  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
  return 0;
}