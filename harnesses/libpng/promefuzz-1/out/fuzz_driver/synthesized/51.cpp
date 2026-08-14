// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_convert_to_rfc1123 at png.c:791:1 in png.h
// png_convert_to_rfc1123 at png.c:791:1 in png.h
// png_convert_to_rfc1123 at png.c:791:1 in png.h
// png_get_tIME at pngget.c:1075:1 in png.h
// png_set_tIME at pngset.c:968:1 in png.h
// png_get_tIME at pngget.c:1075:1 in png.h
// png_set_tIME at pngset.c:968:1 in png.h
// png_get_tIME at pngget.c:1075:1 in png.h
// png_set_tIME at pngset.c:968:1 in png.h
// png_get_tIME at pngget.c:1075:1 in png.h
// png_convert_to_rfc1123_buffer at png.c:737:1 in png.h
// png_convert_to_rfc1123 at png.c:791:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_convert_from_struct_tm at pngwrite.c:467:1 in png.h
// png_convert_from_time_t at pngwrite.c:480:1 in png.h
// png_convert_from_time_t at pngwrite.c:480:1 in png.h
// png_convert_to_rfc1123_buffer at png.c:737:1 in png.h
// png_convert_to_rfc1123_buffer at png.c:737:1 in png.h
// png_convert_to_rfc1123_buffer at png.c:737:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
#include <png.h>

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <setjmp.h>

static uint32_t ReadU32(const uint8_t* data) {
  return (static_cast<uint32_t>(data[0]) << 24) |
         (static_cast<uint32_t>(data[1]) << 16) |
         (static_cast<uint32_t>(data[2]) << 8) |
         static_cast<uint32_t>(data[3]);
}

static uint16_t ReadU16(const uint8_t* data) {
  return static_cast<uint16_t>((static_cast<uint16_t>(data[0]) << 8) | data[1]);
}

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput_51(const uint8_t *Data, size_t Size) {
  struct tm tmbuf;
  std::memset(&tmbuf, 0, sizeof(tmbuf));

  if (Size >= 4) tmbuf.tm_sec = static_cast<int32_t>(ReadU32(Data));
  if (Size >= 8) tmbuf.tm_min = static_cast<int32_t>(ReadU32(Data + 4));
  if (Size >= 12) tmbuf.tm_hour = static_cast<int32_t>(ReadU32(Data + 8));
  if (Size >= 16) tmbuf.tm_mday = static_cast<int32_t>(ReadU32(Data + 12));
  if (Size >= 20) tmbuf.tm_mon = static_cast<int32_t>(ReadU32(Data + 16));
  if (Size >= 24) tmbuf.tm_year = static_cast<int32_t>(ReadU32(Data + 20));
  if (Size >= 28) tmbuf.tm_wday = static_cast<int32_t>(ReadU32(Data + 24));
  if (Size >= 32) tmbuf.tm_yday = static_cast<int32_t>(ReadU32(Data + 28));
  if (Size >= 36) tmbuf.tm_isdst = static_cast<int32_t>(ReadU32(Data + 32));

  png_time pt_from_tm;
  std::memset(&pt_from_tm, 0, sizeof(pt_from_tm));
  png_convert_from_struct_tm(&pt_from_tm, &tmbuf);

  png_time pt_from_time_t;
  std::memset(&pt_from_time_t, 0, sizeof(pt_from_time_t));
  if (Size >= 44) {
    uint64_t v = (static_cast<uint64_t>(ReadU32(Data + 36)) << 32) |
                 static_cast<uint64_t>(ReadU32(Data + 40));
    time_t tt = static_cast<time_t>(v);

    struct tm* g = gmtime(&tt);
    if (g != nullptr) {
      png_convert_from_time_t(&pt_from_time_t, tt);
    } else {
      pt_from_time_t = pt_from_tm;
    }
  } else if (Size >= 40) {
    uint32_t v = ReadU32(Data + 36);
    time_t tt = static_cast<time_t>(v);

    struct tm* g = gmtime(&tt);
    if (g != nullptr) {
      png_convert_from_time_t(&pt_from_time_t, tt);
    } else {
      pt_from_time_t = pt_from_tm;
    }
  } else {
    pt_from_time_t = pt_from_tm;
  }

  png_time pt_direct;
  std::memset(&pt_direct, 0, sizeof(pt_direct));
  if (Size >= 46) pt_direct.year = ReadU16(Data + 44);
  if (Size >= 47) pt_direct.month = Data[46];
  if (Size >= 48) pt_direct.day = Data[47];
  if (Size >= 49) pt_direct.hour = Data[48];
  if (Size >= 50) pt_direct.minute = Data[49];
  if (Size >= 51) pt_direct.second = Data[50];

  char out1[29];
  char out2[29];
  char out3[29];
  std::memset(out1, 0, sizeof(out1));
  std::memset(out2, 0, sizeof(out2));
  std::memset(out3, 0, sizeof(out3));

  (void)png_convert_to_rfc1123_buffer(out1, &pt_from_tm);
  (void)png_convert_to_rfc1123_buffer(out2, &pt_from_time_t);
  (void)png_convert_to_rfc1123_buffer(out3, &pt_direct);

  png_structp png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn, PngWarningFn);
  if (png_ptr == nullptr) {
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr) {
    png_destroy_write_struct(&png_ptr, nullptr);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr)) != 0) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    return 0;
  }

  (void)png_convert_to_rfc1123(png_ptr, &pt_from_tm);
  (void)png_convert_to_rfc1123(png_ptr, &pt_from_time_t);
  (void)png_convert_to_rfc1123(png_ptr, &pt_direct);

  png_timep mod_time = nullptr;
  (void)png_get_tIME(png_ptr, info_ptr, &mod_time);

  png_set_tIME(png_ptr, info_ptr, &pt_from_tm);
  (void)png_get_tIME(png_ptr, info_ptr, &mod_time);

  png_set_tIME(png_ptr, info_ptr, &pt_from_time_t);
  (void)png_get_tIME(png_ptr, info_ptr, &mod_time);

  png_set_tIME(png_ptr, info_ptr, &pt_direct);
  (void)png_get_tIME(png_ptr, info_ptr, &mod_time);

  if (mod_time != nullptr) {
    char out4[29];
    std::memset(out4, 0, sizeof(out4));
    (void)png_convert_to_rfc1123_buffer(out4, mod_time);
    (void)png_convert_to_rfc1123(png_ptr, mod_time);
  }

  png_destroy_write_struct(&png_ptr, &info_ptr);
  return 0;
}