// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_get_sRGB at pngget.c:718:1 in png.h
// png_get_sRGB at pngget.c:718:1 in png.h
// png_get_iCCP at pngget.c:736:1 in png.h
// png_set_iCCP at pngset.c:688:1 in png.h
// png_set_cHRM_fixed at pngset.c:40:1 in png.h
// png_set_cHRM_XYZ_fixed at pngset.c:69:1 in png.h
// png_set_cHRM at pngset.c:102:1 in png.h
// png_set_cHRM_XYZ at pngset.c:118:1 in png.h
// png_get_cHRM at pngget.c:512:1 in png.h
// png_get_iCCP at pngget.c:736:1 in png.h
// png_get_sRGB at pngget.c:718:1 in png.h
// png_read_end at pngread.c:766:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_init_io at png.c:703:1 in png.h
// png_set_sig_bytes at png.c:47:1 in png.h
// png_read_info at pngread.c:92:1 in png.h
// png_get_cHRM at pngget.c:512:1 in png.h
// png_get_cHRM at pngget.c:512:1 in png.h
// png_get_cHRM_XYZ at pngget.c:558:1 in png.h
// png_get_cHRM_fixed at pngget.c:641:1 in png.h
// png_get_cHRM_XYZ_fixed at pngget.c:604:1 in png.h
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

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

static png_uint_32 ReadU32(const uint8_t* data, size_t size, size_t offset) {
  if (offset + 4 > size) return 0;
  return (static_cast<png_uint_32>(data[offset]) << 24) |
         (static_cast<png_uint_32>(data[offset + 1]) << 16) |
         (static_cast<png_uint_32>(data[offset + 2]) << 8) |
         (static_cast<png_uint_32>(data[offset + 3]));
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE* fp = std::fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    if (Size > 0) std::fwrite(Data, 1, Size, fp);
    std::fclose(fp);
  }

  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr,
                                               PngErrorFn, PngWarningFn);
  if (png_ptr == nullptr) return 0;

  png_infop info_ptr = png_create_info_struct(png_ptr);
  png_infop end_info = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr || end_info == nullptr) {
    png_destroy_read_struct(&png_ptr, info_ptr ? &info_ptr : nullptr,
                            end_info ? &end_info : nullptr);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    return 0;
  }

  FILE* rf = std::fopen("./dummy_file", "rb");
  if (rf == nullptr) {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    return 0;
  }

  png_init_io(png_ptr, rf);

  if (Size >= 8) {
    size_t sig_bytes = Data[0] % 9;
    if (sig_bytes > Size) sig_bytes = Size;
    if (sig_bytes > 0) {
      unsigned char sigbuf[8];
      std::memcpy(sigbuf, Data, sig_bytes);
      png_set_sig_bytes(png_ptr, static_cast<int>(sig_bytes));
      std::fseek(rf, static_cast<long>(sig_bytes), SEEK_SET);
    }
  }

  png_read_info(png_ptr, info_ptr);

  double white_x = 0.0, white_y = 0.0, red_x = 0.0, red_y = 0.0;
  double green_x = 0.0, green_y = 0.0, blue_x = 0.0, blue_y = 0.0;
  double red_X = 0.0, red_Y = 0.0, red_Z = 0.0;
  double green_X = 0.0, green_Y = 0.0, green_Z = 0.0;
  double blue_X = 0.0, blue_Y = 0.0, blue_Z = 0.0;
  png_fixed_point f_white_x = 0, f_white_y = 0, f_red_x = 0, f_red_y = 0;
  png_fixed_point f_green_x = 0, f_green_y = 0, f_blue_x = 0, f_blue_y = 0;
  png_fixed_point f_red_X = 0, f_red_Y = 0, f_red_Z = 0;
  png_fixed_point f_green_X = 0, f_green_Y = 0, f_green_Z = 0;
  png_fixed_point f_blue_X = 0, f_blue_Y = 0, f_blue_Z = 0;

  png_get_cHRM(png_ptr, info_ptr, &white_x, &white_y, &red_x, &red_y,
               &green_x, &green_y, &blue_x, &blue_y);
  png_get_cHRM(png_ptr, info_ptr, nullptr, &white_y, nullptr, &red_y,
               &green_x, nullptr, &blue_x, nullptr);
  png_get_cHRM_XYZ(png_ptr, info_ptr, &red_X, &red_Y, &red_Z,
                   &green_X, &green_Y, &green_Z,
                   &blue_X, &blue_Y, &blue_Z);
  png_get_cHRM_fixed(png_ptr, info_ptr, &f_white_x, &f_white_y, &f_red_x, &f_red_y,
                     &f_green_x, &f_green_y, &f_blue_x, &f_blue_y);
  png_get_cHRM_XYZ_fixed(png_ptr, info_ptr, &f_red_X, &f_red_Y, &f_red_Z,
                         &f_green_X, &f_green_Y, &f_green_Z,
                         &f_blue_X, &f_blue_Y, &f_blue_Z);

  int srgb_intent = 0;
  png_get_sRGB(png_ptr, info_ptr, &srgb_intent);
  png_get_sRGB(png_ptr, info_ptr, nullptr);

  png_charp iccp_name = nullptr;
  int compression_type = -1;
  png_bytep profile = nullptr;
  png_uint_32 proflen = 0;
  png_get_iCCP(png_ptr, info_ptr, &iccp_name, &compression_type, &profile, &proflen);

  if (Size > 16) {
    size_t name_len = (Data[1] % 32) + 1;
    char name_buf[64];
    for (size_t i = 0; i < name_len && i < sizeof(name_buf) - 1; ++i) {
      unsigned char c = Data[2 + (i % (Size - 2))];
      name_buf[i] = (c >= 32 && c <= 126) ? static_cast<char>(c) : 'A';
    }
    name_buf[name_len < sizeof(name_buf) ? name_len : sizeof(name_buf) - 1] = '\0';

    size_t profile_off = 2 + name_len;
    png_uint_32 fuzz_proflen = 0;
    if (profile_off < Size) {
      fuzz_proflen = static_cast<png_uint_32>(Size - profile_off);
    }

    int comp = (Data[0] & 1) ? PNG_COMPRESSION_TYPE_BASE : (Data[0] % 256);
    png_set_iCCP(png_ptr, info_ptr, name_buf, comp,
                 profile_off < Size ? reinterpret_cast<png_const_bytep>(Data + profile_off)
                                    : reinterpret_cast<png_const_bytep>(Data),
                 fuzz_proflen);

    if (Size >= 36) {
      png_fixed_point vals[8];
      for (int i = 0; i < 8; ++i) {
        vals[i] = static_cast<png_fixed_point>(ReadU32(Data, Size, (i * 4) % (Size - 3)));
      }
      png_set_cHRM_fixed(png_ptr, info_ptr,
                         vals[0], vals[1], vals[2], vals[3],
                         vals[4], vals[5], vals[6], vals[7]);
    }

    if (Size >= 44) {
      png_fixed_point vals_xyz[9];
      for (int i = 0; i < 9; ++i) {
        vals_xyz[i] = static_cast<png_fixed_point>(ReadU32(Data, Size, (i * 4) % (Size - 3)));
      }
      png_set_cHRM_XYZ_fixed(png_ptr, info_ptr,
                             vals_xyz[0], vals_xyz[1], vals_xyz[2],
                             vals_xyz[3], vals_xyz[4], vals_xyz[5],
                             vals_xyz[6], vals_xyz[7], vals_xyz[8]);
    }

    double d[9];
    for (int i = 0; i < 9; ++i) {
      uint8_t b = (i < static_cast<int>(Size)) ? Data[i] : 0;
      d[i] = static_cast<double>(b) / 255.0;
    }
    png_set_cHRM(png_ptr, info_ptr, d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7]);
    png_set_cHRM_XYZ(png_ptr, info_ptr, d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7], d[8]);

    png_get_cHRM(png_ptr, info_ptr, &white_x, &white_y, &red_x, &red_y,
                 &green_x, &green_y, &blue_x, &blue_y);
    png_get_iCCP(png_ptr, info_ptr, &iccp_name, &compression_type, &profile, &proflen);
    png_get_sRGB(png_ptr, info_ptr, &srgb_intent);
  }

  png_read_end(png_ptr, end_info);

  std::fclose(rf);
  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
  return 0;
}