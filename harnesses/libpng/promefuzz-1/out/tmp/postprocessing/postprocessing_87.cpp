#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <cstddef>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <png.h>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  png_byte buf4[4] = {0, 0, 0, 0};
  png_byte buf2[2] = {0, 0};
  png_byte out4[4] = {0, 0, 0, 0};
  png_byte out2[2] = {0, 0};

  if (Size >= 4) {
    memcpy(buf4, Data, 4);
  } else if (Size > 0) {
    memcpy(buf4, Data, Size);
  }

  if (Size >= 2) {
    memcpy(buf2, Data, 2);
  } else if (Size == 1) {
    buf2[0] = Data[0];
  }

  png_uint_32 u32 = png_get_uint_32(buf4);
  png_int_32 i32 = png_get_int_32(buf4);
  png_uint_16 u16 = png_get_uint_16(buf2);

  png_save_uint_32(out4, u32);
  png_save_int_32(out4, i32);
  png_save_uint_16(out2, u16);

  png_uint_32 mixed_u32 = u32;
  png_int_32 mixed_i32 = i32;
  unsigned int mixed_u16 = u16;

  if (Size >= 8) {
    mixed_u32 ^= static_cast<png_uint_32>(Data[4]) << 24;
    mixed_u32 ^= static_cast<png_uint_32>(Data[5]) << 16;
    mixed_u32 ^= static_cast<png_uint_32>(Data[6]) << 8;
    mixed_u32 ^= static_cast<png_uint_32>(Data[7]);

    mixed_i32 ^= static_cast<png_int_32>(mixed_u32);

    mixed_u16 ^= static_cast<unsigned int>(
        (static_cast<unsigned int>(Data[6]) << 8) | static_cast<unsigned int>(Data[7]));
  } else {
    mixed_u32 ^= 0xFFFFFFFFu;
    mixed_i32 = -mixed_i32;
    mixed_u16 ^= 0xFFFFu;
  }

  png_save_uint_32(out4, mixed_u32);
  png_save_int_32(out4, mixed_i32);
  png_save_uint_16(out2, mixed_u16);

  png_uint_32 roundtrip_u32 = png_get_uint_32(out4);
  png_int_32 roundtrip_i32 = png_get_int_32(out4);
  png_uint_16 roundtrip_u16 = png_get_uint_16(out2);

  png_save_uint_32(out4, roundtrip_u32 ^ static_cast<png_uint_32>(roundtrip_i32));
  png_save_int_32(out4, static_cast<png_int_32>(roundtrip_u32));
  png_save_uint_16(out2, static_cast<unsigned int>(roundtrip_u16 ^ (roundtrip_u32 & 0xFFFFu)));

  return 0;
}