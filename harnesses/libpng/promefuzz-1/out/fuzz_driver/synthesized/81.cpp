// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_save_int_32 at png.c:726:1 in png.h
// png_save_uint_16 at pngwutil.c:37:1 in png.h
// png_save_uint_32 at pngwutil.c:24:1 in png.h
// png_save_int_32 at png.c:726:1 in png.h
// png_save_uint_16 at pngwutil.c:37:1 in png.h
// png_save_uint_32 at pngwutil.c:24:1 in png.h
// png_save_int_32 at png.c:726:1 in png.h
// png_save_uint_16 at pngwutil.c:37:1 in png.h
// png_save_uint_32 at pngwutil.c:24:1 in png.h
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
#include <vector>
#include <png.h>

static png_uint_32 ReadU32Safe(const uint8_t* data, size_t size, size_t offset) {
  png_byte buf[4] = {0, 0, 0, 0};
  if (offset < size) {
    size_t avail = size - offset;
    if (avail > 4) avail = 4;
    memcpy(buf, data + offset, avail);
  }
  return png_get_uint_32(buf);
}

static png_int_32 ReadI32Safe(const uint8_t* data, size_t size, size_t offset) {
  png_byte buf[4] = {0, 0, 0, 0};
  if (offset < size) {
    size_t avail = size - offset;
    if (avail > 4) avail = 4;
    memcpy(buf, data + offset, avail);
  }
  return png_get_int_32(buf);
}

static png_uint_16 ReadU16Safe(const uint8_t* data, size_t size, size_t offset) {
  png_byte buf[2] = {0, 0};
  if (offset < size) {
    size_t avail = size - offset;
    if (avail > 2) avail = 2;
    memcpy(buf, data + offset, avail);
  }
  return png_get_uint_16(buf);
}

extern "C" int LLVMFuzzerTestOneInput_81(const uint8_t *Data, size_t Size) {
  png_byte buf4[4] = {0, 0, 0, 0};
  png_byte buf4b[4] = {0, 0, 0, 0};
  png_byte buf2[2] = {0, 0};

  if (Size >= 4) {
    memcpy(buf4, Data, 4);
  } else if (Size > 0) {
    memcpy(buf4, Data, Size);
  }

  png_uint_32 u32 = png_get_uint_32(buf4);
  png_int_32 i32 = png_get_int_32(buf4);

  png_save_uint_32(buf4b, u32);
  png_save_int_32(buf4b, i32);

  png_uint_32 round_u32 = png_get_uint_32(buf4b);
  png_int_32 round_i32 = png_get_int_32(buf4b);

  if (Size >= 2) {
    memcpy(buf2, Data, 2);
  } else if (Size == 1) {
    buf2[0] = Data[0];
  }

  png_uint_16 u16 = png_get_uint_16(buf2);
  png_save_uint_16(buf2, static_cast<unsigned int>(u16));
  png_uint_16 round_u16 = png_get_uint_16(buf2);

  size_t offset = 0;
  while (offset < Size) {
    png_uint_32 v32 = ReadU32Safe(Data, Size, offset);
    png_int_32 s32 = ReadI32Safe(Data, Size, offset);
    png_uint_16 v16 = ReadU16Safe(Data, Size, offset);

    png_save_uint_32(buf4b, v32);
    png_save_int_32(buf4, s32);
    png_save_uint_16(buf2, static_cast<unsigned int>(v16));

    volatile png_uint_32 sink_u32 = png_get_uint_32(buf4b) ^ png_get_uint_32(buf4);
    volatile png_int_32 sink_i32 = png_get_int_32(buf4);
    volatile png_uint_16 sink_u16 = png_get_uint_16(buf2);
    (void)sink_u32;
    (void)sink_i32;
    (void)sink_u16;

    offset += 1;
  }

  if (Size >= 8) {
    png_uint_32 a = ReadU32Safe(Data, Size, 0);
    png_uint_32 b = ReadU32Safe(Data, Size, 4);
    png_save_uint_32(buf4, a ^ b);
    png_save_int_32(buf4b, static_cast<png_int_32>(a + b));
    png_save_uint_16(buf2, static_cast<unsigned int>((a ^ b) & 0xFFFFu));

    volatile png_uint_32 sink_mix1 = png_get_uint_32(buf4);
    volatile png_int_32 sink_mix2 = png_get_int_32(buf4b);
    volatile png_uint_16 sink_mix3 = png_get_uint_16(buf2);
    (void)sink_mix1;
    (void)sink_mix2;
    (void)sink_mix3;
  }

  volatile png_uint_32 final_u32 = u32 ^ round_u32;
  volatile png_int_32 final_i32 = i32 ^ round_i32;
  volatile png_uint_16 final_u16 = static_cast<png_uint_16>(u16 ^ round_u16);
  (void)final_u32;
  (void)final_i32;
  (void)final_u16;

  return 0;
}