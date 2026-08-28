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
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <vector>
#include <algorithm>

static void WriteLE32(std::vector<uint8_t>& out, uint32_t v) {
  out.push_back(static_cast<uint8_t>(v & 0xff));
  out.push_back(static_cast<uint8_t>((v >> 8) & 0xff));
  out.push_back(static_cast<uint8_t>((v >> 16) & 0xff));
  out.push_back(static_cast<uint8_t>((v >> 24) & 0xff));
}

static void WriteBE32(std::vector<uint8_t>& out, uint32_t v) {
  out.push_back(static_cast<uint8_t>((v >> 24) & 0xff));
  out.push_back(static_cast<uint8_t>((v >> 16) & 0xff));
  out.push_back(static_cast<uint8_t>((v >> 8) & 0xff));
  out.push_back(static_cast<uint8_t>(v & 0xff));
}

static uint32_t ReadLE32(const uint8_t* data, size_t size, size_t off, uint32_t defval) {
  if (off + 4 > size) return defval;
  return static_cast<uint32_t>(data[off]) |
         (static_cast<uint32_t>(data[off + 1]) << 8) |
         (static_cast<uint32_t>(data[off + 2]) << 16) |
         (static_cast<uint32_t>(data[off + 3]) << 24);
}

static uint32_t CRC32PNG(const uint8_t* data, size_t len) {
  uint32_t crc = 0xffffffffU;
  for (size_t i = 0; i < len; ++i) {
    crc ^= data[i];
    for (int k = 0; k < 8; ++k) {
      uint32_t mask = static_cast<uint32_t>(-(static_cast<int32_t>(crc & 1U)));
      crc = (crc >> 1) ^ (0xedb88320U & mask);
    }
  }
  return ~crc;
}

static void AppendChunk(std::vector<uint8_t>& png,
                        const char type[4],
                        const uint8_t* data,
                        uint32_t len) {
  WriteBE32(png, len);
  size_t type_pos = png.size();
  png.push_back(static_cast<uint8_t>(type[0]));
  png.push_back(static_cast<uint8_t>(type[1]));
  png.push_back(static_cast<uint8_t>(type[2]));
  png.push_back(static_cast<uint8_t>(type[3]));
  if (data != nullptr && len > 0) {
    png.insert(png.end(), data, data + len);
  }
  uint32_t crc = CRC32PNG(&png[type_pos], 4 + len);
  WriteBE32(png, crc);
}

static std::vector<uint8_t> BuildPNGFromFuzz(const uint8_t* Data, size_t Size) {
  std::vector<uint8_t> out;
  static const uint8_t sig[8] = {137, 80, 78, 71, 13, 10, 26, 10};
  out.insert(out.end(), sig, sig + 8);

  uint32_t seed0 = ReadLE32(Data, Size, 0, 0x12345678U);
  uint32_t seed1 = ReadLE32(Data, Size, 4, 0x9abcdef0U);
  uint32_t seed2 = ReadLE32(Data, Size, 8, 0x0badc0deU);
  uint32_t seed3 = ReadLE32(Data, Size, 12, 0x13579bdfU);

  uint8_t ihdr[13] = {};
  uint32_t width = (seed0 % 8U) + 1U;
  uint32_t height = (seed1 % 8U) + 1U;
  ihdr[0] = static_cast<uint8_t>((width >> 24) & 0xff);
  ihdr[1] = static_cast<uint8_t>((width >> 16) & 0xff);
  ihdr[2] = static_cast<uint8_t>((width >> 8) & 0xff);
  ihdr[3] = static_cast<uint8_t>(width & 0xff);
  ihdr[4] = static_cast<uint8_t>((height >> 24) & 0xff);
  ihdr[5] = static_cast<uint8_t>((height >> 16) & 0xff);
  ihdr[6] = static_cast<uint8_t>((height >> 8) & 0xff);
  ihdr[7] = static_cast<uint8_t>(height & 0xff);
  ihdr[8] = 8;
  ihdr[9] = 2;
  ihdr[10] = 0;
  ihdr[11] = 0;
  ihdr[12] = 0;
  AppendChunk(out, "IHDR", ihdr, 13);

  if ((seed0 & 1U) != 0) {
    uint8_t gama[4];
    uint32_t g = (seed2 % 1000000U) + 1U;
    gama[0] = static_cast<uint8_t>((g >> 24) & 0xff);
    gama[1] = static_cast<uint8_t>((g >> 16) & 0xff);
    gama[2] = static_cast<uint8_t>((g >> 8) & 0xff);
    gama[3] = static_cast<uint8_t>(g & 0xff);
    AppendChunk(out, "gAMA", gama, 4);
  }

  if ((seed0 & 2U) != 0) {
    uint8_t srgb[1];
    srgb[0] = static_cast<uint8_t>(seed1 % 4U);
    AppendChunk(out, "sRGB", srgb, 1);
  }

  if ((seed0 & 4U) != 0) {
    uint8_t sbit[3];
    sbit[0] = static_cast<uint8_t>((seed1 % 8U) + 1U);
    sbit[1] = static_cast<uint8_t>((seed2 % 8U) + 1U);
    sbit[2] = static_cast<uint8_t>((seed3 % 8U) + 1U);
    AppendChunk(out, "sBIT", sbit, 3);
  }

  if ((seed0 & 8U) != 0) {
    uint8_t chrm[32];
    for (int i = 0; i < 8; ++i) {
      uint32_t v = (ReadLE32(Data, Size, 16 + i * 4, 10000U + static_cast<uint32_t>(i) * 1000U) % 80000U) + 1U;
      chrm[i * 4 + 0] = static_cast<uint8_t>((v >> 24) & 0xff);
      chrm[i * 4 + 1] = static_cast<uint8_t>((v >> 16) & 0xff);
      chrm[i * 4 + 2] = static_cast<uint8_t>((v >> 8) & 0xff);
      chrm[i * 4 + 3] = static_cast<uint8_t>(v & 0xff);
    }
    AppendChunk(out, "cHRM", chrm, 32);
  }

  if ((seed0 & 16U) != 0) {
    uint8_t idat[1] = {0};
    AppendChunk(out, "IDAT", idat, 1);
  }

  AppendChunk(out, "IEND", nullptr, 0);
  return out;
}

static void FuzzInfoQueries(png_structp png_ptr, png_infop info_ptr, const uint8_t* Data, size_t Size) {
  double gamma = 0.0;
  png_fixed_point gamma_fixed = 0;
  png_color_8p sig_bit_ptr = nullptr;
  png_color_8 sig_bit_local = {};
  int srgb_intent = 0;
  png_fixed_point wx = 0, wy = 0, rx = 0, ry = 0, gx = 0, gy = 0, bx = 0, by = 0;

  (void)png_get_gAMA(png_ptr, info_ptr, &gamma);
  (void)png_get_gAMA_fixed(png_ptr, info_ptr, &gamma_fixed);
  (void)png_get_sBIT(png_ptr, info_ptr, &sig_bit_ptr);
  (void)png_get_sBIT(png_ptr, info_ptr, &sig_bit_ptr);
  (void)png_get_sRGB(png_ptr, info_ptr, &srgb_intent);
  (void)png_get_sRGB(png_ptr, info_ptr, nullptr);
  (void)png_get_cHRM_fixed(png_ptr, info_ptr, &wx, &wy, &rx, &ry, &gx, &gy, &bx, &by);
  (void)png_get_cHRM_fixed(png_ptr, info_ptr, nullptr, &wy, nullptr, &ry, nullptr, &gy, nullptr, &by);

  const png_uint_32 flags[] = {
      PNG_INFO_gAMA,
      PNG_INFO_sBIT,
      PNG_INFO_sRGB,
      PNG_INFO_cHRM,
      static_cast<png_uint_32>(ReadLE32(Data, Size, 0, 0))
  };
  for (png_uint_32 f : flags) {
    (void)png_get_valid(png_ptr, info_ptr, f);
  }

#ifdef PNG_gAMA_SUPPORTED
  if (Size > 0) {
    double set_gamma = static_cast<double>((ReadLE32(Data, Size, 20, 45455U) % 1000000U) + 1U) / 100000.0;
    png_set_gAMA(png_ptr, info_ptr, set_gamma);
    png_fixed_point set_gamma_fixed = static_cast<png_fixed_point>(ReadLE32(Data, Size, 24, 45455U) % 1000000U);
    png_set_gAMA_fixed(png_ptr, info_ptr, set_gamma_fixed);
    (void)png_get_gAMA(png_ptr, info_ptr, &gamma);
    (void)png_get_gAMA_fixed(png_ptr, info_ptr, &gamma_fixed);
  }
#endif

#ifdef PNG_cHRM_SUPPORTED
  if (Size > 4) {
    png_fixed_point vals[8];
    for (int i = 0; i < 8; ++i) {
      vals[i] = static_cast<png_fixed_point>(ReadLE32(Data, Size, 32 + i * 4, 10000U + i) % 100000U);
    }
    png_set_cHRM_fixed(png_ptr, info_ptr,
                       vals[0], vals[1], vals[2], vals[3],
                       vals[4], vals[5], vals[6], vals[7]);
    (void)png_get_cHRM_fixed(png_ptr, info_ptr, &wx, &wy, &rx, &ry, &gx, &gy, &bx, &by);
  }
#endif

  if (sig_bit_ptr == nullptr) {
    (void)sig_bit_local;
  }
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  std::vector<uint8_t> png_bytes = BuildPNGFromFuzz(Data, Size);

  FILE* fp = fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    if (!png_bytes.empty()) {
      fwrite(png_bytes.data(), 1, png_bytes.size(), fp);
    }
    fclose(fp);
  }

  fp = fopen("./dummy_file", "rb");
  if (fp == nullptr) {
    return 0;
  }

  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (png_ptr == nullptr) {
    fclose(fp);
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr) {
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    fclose(fp);
    return 0;
  }

  png_infop end_info = png_create_info_struct(png_ptr);
  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, end_info ? &end_info : nullptr);
    fclose(fp);
    return 0;
  }

  png_init_io(png_ptr, fp);
  png_read_info(png_ptr, info_ptr);

  FuzzInfoQueries(png_ptr, info_ptr, Data, Size);

  if ((Size & 1U) != 0) {
    png_read_end(png_ptr, end_info);
    if (end_info != nullptr) {
      FuzzInfoQueries(png_ptr, end_info, Data, Size);
    }
  }

  png_destroy_read_struct(&png_ptr, &info_ptr, end_info ? &end_info : nullptr);
  fclose(fp);
  return 0;
}