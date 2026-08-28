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

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <ctime>

static uint32_t ReadU32(const uint8_t *data, size_t size, size_t &offset) {
  if (offset + 4 > size) return 0;
  uint32_t v = (static_cast<uint32_t>(data[offset]) << 24) |
               (static_cast<uint32_t>(data[offset + 1]) << 16) |
               (static_cast<uint32_t>(data[offset + 2]) << 8) |
               static_cast<uint32_t>(data[offset + 3]);
  offset += 4;
  return v;
}

static int ReadS32(const uint8_t *data, size_t size, size_t &offset) {
  return static_cast<int>(ReadU32(data, size, offset));
}

static void FillPngTimeFromData(png_time &pt, const uint8_t *data, size_t size,
                                size_t &offset) {
  pt.year = static_cast<png_uint_16>(ReadU32(data, size, offset) & 0xFFFFu);
  pt.month = static_cast<png_byte>((offset < size) ? data[offset++] : 0);
  pt.day = static_cast<png_byte>((offset < size) ? data[offset++] : 0);
  pt.hour = static_cast<png_byte>((offset < size) ? data[offset++] : 0);
  pt.minute = static_cast<png_byte>((offset < size) ? data[offset++] : 0);
  pt.second = static_cast<png_byte>((offset < size) ? data[offset++] : 0);
}

static void FillTmFromData(struct tm &t, const uint8_t *data, size_t size,
                           size_t &offset) {
  std::memset(&t, 0, sizeof(t));
  t.tm_sec = ReadS32(data, size, offset);
  t.tm_min = ReadS32(data, size, offset);
  t.tm_hour = ReadS32(data, size, offset);
  t.tm_mday = ReadS32(data, size, offset);
  t.tm_mon = ReadS32(data, size, offset);
  t.tm_year = ReadS32(data, size, offset);
  t.tm_wday = ReadS32(data, size, offset);
  t.tm_yday = ReadS32(data, size, offset);
  t.tm_isdst = ReadS32(data, size, offset);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE *fp = std::fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    if (Size > 0) std::fwrite(Data, 1, Size, fp);
    std::fclose(fp);
  }

  png_structp png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (png_ptr == nullptr) return 0;

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr) {
    png_destroy_write_struct(&png_ptr, nullptr);
    return 0;
  }

  size_t offset = 0;

  png_time direct_time;
  FillPngTimeFromData(direct_time, Data, Size, offset);

  struct tm tm_time;
  FillTmFromData(tm_time, Data, Size, offset);

  png_time from_tm_time;
  std::memset(&from_tm_time, 0, sizeof(from_tm_time));
  png_convert_from_struct_tm(&from_tm_time, &tm_time);

  time_t raw_time = static_cast<time_t>(ReadU32(Data, Size, offset));
  png_time from_time_t_time;
  std::memset(&from_time_t_time, 0, sizeof(from_time_t_time));
  png_convert_from_time_t(&from_time_t_time, raw_time);

  png_set_tIME(png_ptr, info_ptr, &direct_time);
  png_set_tIME(png_ptr, info_ptr, &from_tm_time);
  png_set_tIME(png_ptr, info_ptr, &from_time_t_time);

  png_timep mod_time = nullptr;
  (void)png_get_tIME(png_ptr, info_ptr, &mod_time);

  char out1[29];
  char out2[29];
  char out3[29];
  (void)png_convert_to_rfc1123_buffer(out1, &direct_time);
  (void)png_convert_to_rfc1123_buffer(out2, &from_tm_time);
  (void)png_convert_to_rfc1123_buffer(out3, &from_time_t_time);

  if (mod_time != nullptr) {
    char out4[29];
    (void)png_convert_to_rfc1123_buffer(out4, mod_time);
    (void)png_convert_to_rfc1123(png_ptr, mod_time);
  }

  (void)png_convert_to_rfc1123(png_ptr, &direct_time);
  (void)png_convert_to_rfc1123(png_ptr, &from_tm_time);
  (void)png_convert_to_rfc1123(png_ptr, &from_time_t_time);

  png_destroy_write_struct(&png_ptr, &info_ptr);
  return 0;
}