// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_convert_from_time_t at pngwrite.c:480:1 in png.h
// png_convert_from_struct_tm at pngwrite.c:467:1 in png.h
// png_convert_from_time_t at pngwrite.c:480:1 in png.h
// png_convert_from_struct_tm at pngwrite.c:467:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_tIME at pngset.c:968:1 in png.h
// png_set_tIME at pngset.c:968:1 in png.h
// png_set_tIME at pngset.c:968:1 in png.h
// png_set_tIME at pngset.c:968:1 in png.h
// png_set_tIME at pngset.c:968:1 in png.h
// png_set_tIME at pngset.c:968:1 in png.h
// png_get_tIME at pngget.c:1075:1 in png.h
// png_convert_to_rfc1123_buffer at png.c:737:1 in png.h
// png_convert_to_rfc1123 at png.c:791:1 in png.h
// png_get_tIME at pngget.c:1075:1 in png.h
// png_get_tIME at pngget.c:1075:1 in png.h
// png_get_tIME at pngget.c:1075:1 in png.h
// png_convert_to_rfc1123 at png.c:791:1 in png.h
// png_convert_to_rfc1123 at png.c:791:1 in png.h
// png_convert_to_rfc1123 at png.c:791:1 in png.h
// png_convert_to_rfc1123 at png.c:791:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_convert_to_rfc1123_buffer at png.c:737:1 in png.h
// png_convert_to_rfc1123_buffer at png.c:737:1 in png.h
// png_convert_to_rfc1123_buffer at png.c:737:1 in png.h
// png_convert_from_struct_tm at pngwrite.c:467:1 in png.h
#include <png.h>

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <ctime>

static uint16_t ReadU16(const uint8_t *data, size_t size, size_t offset) {
  if (offset + 1 >= size) return 0;
  return static_cast<uint16_t>(data[offset]) |
         (static_cast<uint16_t>(data[offset + 1]) << 8);
}

static uint32_t ReadU32(const uint8_t *data, size_t size, size_t offset) {
  if (offset + 3 >= size) return 0;
  return static_cast<uint32_t>(data[offset]) |
         (static_cast<uint32_t>(data[offset + 1]) << 8) |
         (static_cast<uint32_t>(data[offset + 2]) << 16) |
         (static_cast<uint32_t>(data[offset + 3]) << 24);
}

static void FillPngTimeFromData(png_time *pt, const uint8_t *data, size_t size,
                                size_t offset) {
  if (pt == nullptr) return;
  std::memset(pt, 0, sizeof(*pt));
  pt->year = ReadU16(data, size, offset);
  pt->month = (offset + 2 < size) ? data[offset + 2] : 0;
  pt->day = (offset + 3 < size) ? data[offset + 3] : 0;
  pt->hour = (offset + 4 < size) ? data[offset + 4] : 0;
  pt->minute = (offset + 5 < size) ? data[offset + 5] : 0;
  pt->second = (offset + 6 < size) ? data[offset + 6] : 0;
}

static void FillTmFromData(struct tm *tmv, const uint8_t *data, size_t size,
                           size_t offset) {
  if (tmv == nullptr) return;
  std::memset(tmv, 0, sizeof(*tmv));
  tmv->tm_sec = (offset + 0 < size) ? static_cast<int8_t>(data[offset + 0]) : 0;
  tmv->tm_min = (offset + 1 < size) ? static_cast<int8_t>(data[offset + 1]) : 0;
  tmv->tm_hour = (offset + 2 < size) ? static_cast<int8_t>(data[offset + 2]) : 0;
  tmv->tm_mday = (offset + 3 < size) ? static_cast<int8_t>(data[offset + 3]) : 1;
  tmv->tm_mon = (offset + 4 < size) ? static_cast<int8_t>(data[offset + 4]) : 0;
  tmv->tm_year = static_cast<int>(ReadU16(data, size, offset + 5)) - 1900;
  tmv->tm_wday = (offset + 7 < size) ? static_cast<int8_t>(data[offset + 7]) : 0;
  tmv->tm_yday = static_cast<int>(ReadU16(data, size, offset + 8));
  tmv->tm_isdst = (offset + 10 < size) ? static_cast<int8_t>(data[offset + 10]) : 0;
}

static time_t ReadTimeT(const uint8_t *data, size_t size, size_t offset) {
  time_t t = 0;
  const size_t n = sizeof(time_t) < 8 ? sizeof(time_t) : 8;
  for (size_t i = 0; i < n && offset + i < size; ++i) {
    t |= static_cast<time_t>(
        static_cast<unsigned long long>(data[offset + i]) << (8 * i));
  }
  return t;
}

static int SafeGmtime(const time_t *t, struct tm *out_tm) {
  if (t == nullptr || out_tm == nullptr) return 0;
#if defined(_WIN32)
  return gmtime_s(out_tm, t) == 0;
#elif defined(__STDC_LIB_EXT1__)
  return gmtime_s(t, out_tm) != nullptr;
#else
  struct tm *res = gmtime(t);
  if (res == nullptr) return 0;
  *out_tm = *res;
  return 1;
#endif
}

static void PngErrorFn(png_structp, png_const_charp) {
}

static void PngWarningFn(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput_8(const uint8_t *Data, size_t Size) {
  png_time pt1;
  png_time pt2;
  png_time pt3;
  FillPngTimeFromData(&pt1, Data, Size, 0);
  FillPngTimeFromData(&pt2, Data, Size, 7);
  FillPngTimeFromData(&pt3, Data, Size, 14);

  struct tm tmv;
  FillTmFromData(&tmv, Data, Size, 21);

  char out1[29];
  char out2[29];
  std::memset(out1, 0, sizeof(out1));
  std::memset(out2, 0, sizeof(out2));

  (void)png_convert_to_rfc1123_buffer(out1, &pt1);
  (void)png_convert_to_rfc1123_buffer(out2, &pt2);
  (void)png_convert_to_rfc1123_buffer(nullptr, &pt1);

  png_convert_from_struct_tm(&pt2, &tmv);

  time_t tt1 = ReadTimeT(Data, Size, 32);
  time_t tt2 = static_cast<time_t>(ReadU32(Data, Size, 40));

  struct tm safe_tm;
  if (SafeGmtime(&tt1, &safe_tm)) {
    png_convert_from_time_t(&pt3, tt1);
  } else {
    png_convert_from_struct_tm(&pt3, &tmv);
  }

  if (SafeGmtime(&tt2, &safe_tm)) {
    png_convert_from_time_t(&pt1, tt2);
  } else {
    png_convert_from_struct_tm(&pt1, &tmv);
  }

  png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr,
                                                PngErrorFn, PngWarningFn);
  if (png_ptr == nullptr) {
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr) {
    png_destroy_write_struct(&png_ptr, nullptr);
    return 0;
  }

  if (Size > 0) {
    switch (Data[0] % 4) {
      case 0:
        png_set_tIME(png_ptr, info_ptr, &pt1);
        break;
      case 1:
        png_set_tIME(png_ptr, info_ptr, &pt2);
        break;
      case 2:
        png_set_tIME(png_ptr, info_ptr, &pt3);
        break;
      default:
        png_set_tIME(nullptr, info_ptr, &pt1);
        png_set_tIME(png_ptr, nullptr, &pt1);
        break;
    }
  } else {
    png_set_tIME(png_ptr, info_ptr, &pt1);
  }

  png_timep got_time = nullptr;
  (void)png_get_tIME(png_ptr, info_ptr, &got_time);

  if (got_time != nullptr) {
    (void)png_convert_to_rfc1123_buffer(out1, got_time);
    (void)png_convert_to_rfc1123(png_ptr, got_time);
  }

  (void)png_get_tIME(nullptr, info_ptr, &got_time);
  (void)png_get_tIME(png_ptr, nullptr, &got_time);
  (void)png_get_tIME(png_ptr, info_ptr, nullptr);

  (void)png_convert_to_rfc1123(png_ptr, &pt1);
  (void)png_convert_to_rfc1123(png_ptr, &pt2);
  (void)png_convert_to_rfc1123(png_ptr, &pt3);
  (void)png_convert_to_rfc1123(nullptr, &pt1);

  png_destroy_write_struct(&png_ptr, &info_ptr);
  return 0;
}