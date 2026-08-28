// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_IHDR at pngset.c:254:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_set_PLTE at pngset.c:572:1 in png.h
// png_set_oFFs at pngset.c:295:1 in png.h
// png_set_pCAL at pngset.c:312:1 in png.h
// png_write_info at pngwrite.c:192:1 in png.h
// png_write_chunk at pngwutil.c:192:1 in png.h
// png_write_end at pngwrite.c:358:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_save_int_32 at png.c:726:1 in png.h
// png_save_int_32 at png.c:726:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_write_fn at pngwio.c:122:1 in png.h
#include <png.h>

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

static void WriteDataFn(png_structp png_ptr, png_bytep data, png_size_t length) {
  png_voidp io_ptr = png_get_io_ptr(png_ptr);
  if (io_ptr == nullptr || data == nullptr || length == 0) return;
  std::vector<unsigned char>* out =
      reinterpret_cast<std::vector<unsigned char>*>(io_ptr);
  out->insert(out->end(), data, data + length);
}

static void FlushFn(png_structp) {
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* Data, size_t Size) {
  if (Data == nullptr) return 0;

  {
    uint8_t buf4[4] = {0, 0, 0, 0};
    if (Size >= 4)
      memcpy(buf4, Data, 4);
    else if (Size > 0)
      memcpy(buf4, Data, Size);

    png_int_32 v1 = png_get_int_32(buf4);
    uint8_t out4[4] = {0, 0, 0, 0};
    png_save_int_32(out4, v1);
    (void)png_get_int_32(out4);

    if (Size >= 8) {
      png_int_32 v2 = png_get_int_32(Data + Size - 4);
      png_save_int_32(out4, v2);
      (void)png_get_int_32(out4);
    }
  }

  FILE* fp = std::fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    if (Size > 0) std::fwrite(Data, 1, Size, fp);
    std::fclose(fp);
  }

  png_structp png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn,
                              PngWarningFn);
  if (png_ptr == nullptr) return 0;

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr) {
    png_destroy_write_struct(&png_ptr, nullptr);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    return 0;
  }

  std::vector<unsigned char> output;
  png_set_write_fn(png_ptr, &output, WriteDataFn, FlushFn);

  int color_type = PNG_COLOR_TYPE_RGB;
  int bit_depth = 8;
  if (Size > 0) {
    switch (Data[0] % 5) {
      case 0: color_type = PNG_COLOR_TYPE_GRAY; break;
      case 1: color_type = PNG_COLOR_TYPE_RGB; break;
      case 2: color_type = PNG_COLOR_TYPE_RGBA; break;
      case 3: color_type = PNG_COLOR_TYPE_GA; break;
      case 4: color_type = PNG_COLOR_TYPE_PALETTE; break;
    }
  }

  if (Size > 1) {
    switch (Data[1] % 4) {
      case 0: bit_depth = 1; break;
      case 1: bit_depth = 2; break;
      case 2: bit_depth = 4; break;
      case 3: bit_depth = 8; break;
    }

    if (color_type == PNG_COLOR_TYPE_RGB ||
        color_type == PNG_COLOR_TYPE_RGBA ||
        color_type == PNG_COLOR_TYPE_GA) {
      bit_depth = (Data[1] & 1) ? 8 : 16;
    }
  }

  png_uint_32 width = 1, height = 1;
  if (Size >= 10) {
    uint8_t wbuf[4], hbuf[4];
    memcpy(wbuf, Data + 2, 4);
    memcpy(hbuf, Data + 6, 4);
    png_int_32 sw = png_get_int_32(wbuf);
    png_int_32 sh = png_get_int_32(hbuf);
    if (sw == 0) sw = 1;
    if (sh == 0) sh = 1;
    width = static_cast<png_uint_32>(sw < 0 ? -static_cast<int64_t>(sw) : sw);
    height = static_cast<png_uint_32>(sh < 0 ? -static_cast<int64_t>(sh) : sh);
    width = (width % 64) + 1;
    height = (height % 64) + 1;
  }

  png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, color_type,
               PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE,
               PNG_FILTER_TYPE_BASE);

  png_color palette[256];
  if (color_type == PNG_COLOR_TYPE_PALETTE) {
    for (int i = 0; i < 256; ++i) {
      palette[i].red = static_cast<png_byte>(i);
      palette[i].green = static_cast<png_byte>(255 - i);
      palette[i].blue = static_cast<png_byte>((i * 3) & 0xff);
    }
    png_set_PLTE(png_ptr, info_ptr, palette, 256);
  }

  png_int_32 x0 = 0, x1 = 1;
  if (Size >= 18) {
    x0 = png_get_int_32(Data + 10);
    x1 = png_get_int_32(Data + 14);
  }

  png_int_32 offx = 0, offy = 0;
  if (Size >= 26) {
    offx = png_get_int_32(Data + 18);
    offy = png_get_int_32(Data + 22);
  }
  int unit_type = (Size > 26) ? (Data[26] % 3) : 0;
  png_set_oFFs(png_ptr, info_ptr, offx, offy, unit_type);

  if (Size > 27) {
    int pcal_type = Data[27] % 4;
    png_set_pCAL(png_ptr, info_ptr, "fuzz", x0, x1, pcal_type, 0, "unit",
                 nullptr);
  }

  png_write_info(png_ptr, info_ptr);

  png_byte chunk_name[4] = {'f', 'U', 'Z', 'Z'};
  if (Size >= 4) memcpy(chunk_name, Data, 4);

  png_const_bytep chunk_data =
      (Size > 32) ? reinterpret_cast<png_const_bytep>(Data + 32) : nullptr;
  size_t chunk_len = (Size > 32) ? (Size - 32) : 0;
  if (chunk_len > 128) chunk_len = 128;
  png_write_chunk(png_ptr, chunk_name, chunk_data, chunk_len);

  png_write_end(png_ptr, info_ptr);
  png_destroy_write_struct(&png_ptr, &info_ptr);
  return 0;
}