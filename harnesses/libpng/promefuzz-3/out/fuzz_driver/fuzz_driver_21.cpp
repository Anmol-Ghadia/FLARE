// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_convert_from_struct_tm at pngwrite.c:467:1 in png.h
// png_set_tIME at pngset.c:968:1 in png.h
// png_get_tIME at pngget.c:1075:1 in png.h
// png_convert_to_rfc1123_buffer at png.c:737:1 in png.h
// png_convert_to_rfc1123 at png.c:791:1 in png.h
// png_convert_to_rfc1123_buffer at png.c:737:1 in png.h
// png_convert_to_rfc1123 at png.c:791:1 in png.h
// png_set_tIME at pngset.c:968:1 in png.h
// png_get_tIME at pngget.c:1075:1 in png.h
// png_convert_to_rfc1123_buffer at png.c:737:1 in png.h
// png_convert_to_rfc1123 at png.c:791:1 in png.h
// png_convert_to_rfc1123_buffer at png.c:737:1 in png.h
// png_convert_to_rfc1123 at png.c:791:1 in png.h
// png_set_tIME at pngset.c:968:1 in png.h
// png_get_tIME at pngget.c:1075:1 in png.h
// png_convert_to_rfc1123_buffer at png.c:737:1 in png.h
// png_convert_to_rfc1123 at png.c:791:1 in png.h
// png_convert_to_rfc1123_buffer at png.c:737:1 in png.h
// png_convert_to_rfc1123 at png.c:791:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_convert_from_struct_tm at pngwrite.c:467:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_tIME at pngset.c:968:1 in png.h
// png_get_tIME at pngget.c:1075:1 in png.h
// png_convert_to_rfc1123_buffer at png.c:737:1 in png.h
// png_convert_to_rfc1123 at png.c:791:1 in png.h
// png_convert_to_rfc1123_buffer at png.c:737:1 in png.h
// png_convert_to_rfc1123 at png.c:791:1 in png.h
#include <png.h>

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <fstream>

static uint32_t ReadU32(const uint8_t *data, size_t size, size_t &offset) {
  if (offset + 4 > size) return 0;
  uint32_t v = (static_cast<uint32_t>(data[offset]) << 24) |
               (static_cast<uint32_t>(data[offset + 1]) << 16) |
               (static_cast<uint32_t>(data[offset + 2]) << 8) |
               (static_cast<uint32_t>(data[offset + 3]));
  offset += 4;
  return v;
}

static int ReadS32(const uint8_t *data, size_t size, size_t &offset) {
  return static_cast<int>(ReadU32(data, size, offset));
}

static uint16_t ReadU16(const uint8_t *data, size_t size, size_t &offset) {
  if (offset + 2 > size) return 0;
  uint16_t v = static_cast<uint16_t>(
      (static_cast<uint16_t>(data[offset]) << 8) |
      static_cast<uint16_t>(data[offset + 1]));
  offset += 2;
  return v;
}

static uint8_t ReadU8(const uint8_t *data, size_t size, size_t &offset) {
  if (offset >= size) return 0;
  return data[offset++];
}

static bool SafeConvertFromTimeT(png_timep ptime, time_t ttime) {
  if (ptime == nullptr) return false;
  struct tm *tmp = gmtime(&ttime);
  if (tmp == nullptr) return false;
  png_convert_from_struct_tm(ptime, tmp);
  return true;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  std::ofstream dummy("./dummy_file", std::ios::binary);
  if (dummy) {
    dummy.write(reinterpret_cast<const char *>(Data),
                static_cast<std::streamsize>(Size));
  }

  png_structp png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (png_ptr == nullptr) return 0;

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr) {
    png_destroy_write_struct(&png_ptr, nullptr);
    return 0;
  }

  size_t off = 0;

  png_time fuzz_time;
  std::memset(&fuzz_time, 0, sizeof(fuzz_time));
  fuzz_time.year = ReadU16(Data, Size, off);
  fuzz_time.month = ReadU8(Data, Size, off);
  fuzz_time.day = ReadU8(Data, Size, off);
  fuzz_time.hour = ReadU8(Data, Size, off);
  fuzz_time.minute = ReadU8(Data, Size, off);
  fuzz_time.second = ReadU8(Data, Size, off);

  png_set_tIME(png_ptr, info_ptr, &fuzz_time);

  png_timep mod_time = nullptr;
  (void)png_get_tIME(png_ptr, info_ptr, &mod_time);

  char out1[29];
  std::memset(out1, 0, sizeof(out1));
  if (mod_time != nullptr) {
    (void)png_convert_to_rfc1123_buffer(out1, mod_time);
    (void)png_convert_to_rfc1123(png_ptr, mod_time);
  } else {
    (void)png_convert_to_rfc1123_buffer(out1, &fuzz_time);
    (void)png_convert_to_rfc1123(png_ptr, &fuzz_time);
  }

  struct tm t;
  std::memset(&t, 0, sizeof(t));
  t.tm_sec = ReadS32(Data, Size, off);
  t.tm_min = ReadS32(Data, Size, off);
  t.tm_hour = ReadS32(Data, Size, off);
  t.tm_mday = ReadS32(Data, Size, off);
  t.tm_mon = ReadS32(Data, Size, off);
  t.tm_year = ReadS32(Data, Size, off);
  t.tm_wday = ReadS32(Data, Size, off);
  t.tm_yday = ReadS32(Data, Size, off);
  t.tm_isdst = ReadS32(Data, Size, off);

  png_time from_tm;
  std::memset(&from_tm, 0, sizeof(from_tm));
  png_convert_from_struct_tm(&from_tm, &t);
  png_set_tIME(png_ptr, info_ptr, &from_tm);

  png_timep mod_time2 = nullptr;
  (void)png_get_tIME(png_ptr, info_ptr, &mod_time2);

  char out2[29];
  std::memset(out2, 0, sizeof(out2));
  (void)png_convert_to_rfc1123_buffer(out2, &from_tm);
  (void)png_convert_to_rfc1123(png_ptr, &from_tm);
  if (mod_time2 != nullptr) {
    (void)png_convert_to_rfc1123_buffer(out2, mod_time2);
    (void)png_convert_to_rfc1123(png_ptr, mod_time2);
  }

  time_t tt = static_cast<time_t>(ReadU32(Data, Size, off));
  if (sizeof(time_t) > 4 && off + 4 <= Size) {
    uint64_t hi = ReadU32(Data, Size, off);
    uint64_t lo = static_cast<uint64_t>(tt) & 0xffffffffULL;
    uint64_t combined = (hi << 32) | lo;
    tt = static_cast<time_t>(combined);
  }

  png_time from_time_t_val;
  std::memset(&from_time_t_val, 0, sizeof(from_time_t_val));
  if (SafeConvertFromTimeT(&from_time_t_val, tt)) {
    png_set_tIME(png_ptr, info_ptr, &from_time_t_val);

    png_timep mod_time3 = nullptr;
    (void)png_get_tIME(png_ptr, info_ptr, &mod_time3);

    char out3[29];
    std::memset(out3, 0, sizeof(out3));
    (void)png_convert_to_rfc1123_buffer(out3, &from_time_t_val);
    (void)png_convert_to_rfc1123(png_ptr, &from_time_t_val);
    if (mod_time3 != nullptr) {
      (void)png_convert_to_rfc1123_buffer(out3, mod_time3);
      (void)png_convert_to_rfc1123(png_ptr, mod_time3);
    }
  }

  png_time boundary_times[4];
  std::memset(boundary_times, 0, sizeof(boundary_times));

  boundary_times[0].year = 1970;
  boundary_times[0].month = 1;
  boundary_times[0].day = 1;
  boundary_times[0].hour = 0;
  boundary_times[0].minute = 0;
  boundary_times[0].second = 0;

  boundary_times[1].year = 9999;
  boundary_times[1].month = 12;
  boundary_times[1].day = 31;
  boundary_times[1].hour = 23;
  boundary_times[1].minute = 59;
  boundary_times[1].second = 60;

  boundary_times[2].year = 0;
  boundary_times[2].month = 0;
  boundary_times[2].day = 0;
  boundary_times[2].hour = 0;
  boundary_times[2].minute = 0;
  boundary_times[2].second = 0;

  boundary_times[3].year = 65535;
  boundary_times[3].month = 255;
  boundary_times[3].day = 255;
  boundary_times[3].hour = 255;
  boundary_times[3].minute = 255;
  boundary_times[3].second = 255;

  for (int i = 0; i < 4; ++i) {
    png_set_tIME(png_ptr, info_ptr, &boundary_times[i]);
    png_timep mt = nullptr;
    (void)png_get_tIME(png_ptr, info_ptr, &mt);

    char out[29];
    std::memset(out, 0, sizeof(out));
    (void)png_convert_to_rfc1123_buffer(out, &boundary_times[i]);
    (void)png_convert_to_rfc1123(png_ptr, &boundary_times[i]);
    if (mt != nullptr) {
      (void)png_convert_to_rfc1123_buffer(out, mt);
      (void)png_convert_to_rfc1123(png_ptr, mt);
    }
  }

  png_destroy_write_struct(&png_ptr, &info_ptr);
  return 0;
}