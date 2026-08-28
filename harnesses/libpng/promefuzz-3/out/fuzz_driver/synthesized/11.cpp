// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_get_cHRM at pngget.c:512:1 in png.h
// png_get_cHRM_fixed at pngget.c:641:1 in png.h
// png_get_cHRM_XYZ_fixed at pngget.c:604:1 in png.h
// png_set_cHRM_fixed at pngset.c:40:1 in png.h
// png_set_cHRM_XYZ_fixed at pngset.c:69:1 in png.h
// png_set_cHRM at pngset.c:102:1 in png.h
// png_set_cHRM_fixed at pngset.c:40:1 in png.h
// png_get_cHRM_fixed at pngget.c:641:1 in png.h
// png_set_cHRM_XYZ_fixed at pngset.c:69:1 in png.h
// png_set_cHRM_XYZ_fixed at pngset.c:69:1 in png.h
// png_get_cHRM_XYZ_fixed at pngget.c:604:1 in png.h
// png_set_cHRM at pngset.c:102:1 in png.h
// png_set_cHRM at pngset.c:102:1 in png.h
// png_get_cHRM at pngget.c:512:1 in png.h
// png_set_cHRM_fixed at pngset.c:40:1 in png.h
// png_get_cHRM at pngget.c:512:1 in png.h
// png_get_cHRM_fixed at pngget.c:641:1 in png.h
// png_get_cHRM_XYZ_fixed at pngget.c:604:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
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
#include <string.h>
#include <setjmp.h>
#include <vector>
#include <limits>

static uint32_t ReadU32(const uint8_t *data, size_t size, size_t &off) {
    if (off + 4 > size) return 0;
    uint32_t v = (static_cast<uint32_t>(data[off]) << 24) |
                 (static_cast<uint32_t>(data[off + 1]) << 16) |
                 (static_cast<uint32_t>(data[off + 2]) << 8) |
                 (static_cast<uint32_t>(data[off + 3]));
    off += 4;
    return v;
}

static int32_t ReadS32(const uint8_t *data, size_t size, size_t &off) {
    return static_cast<int32_t>(ReadU32(data, size, off));
}

static double ReadDoubleLike(const uint8_t *data, size_t size, size_t &off) {
    int32_t v = ReadS32(data, size, off);
    return static_cast<double>(v) / 100000.0;
}

static void WriteDummyFile(const uint8_t *data, size_t size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) return;
    if (size) fwrite(data, 1, size, fp);
    fclose(fp);
}

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
    longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput_11(const uint8_t *Data, size_t Size) {
    WriteDummyFile(Data, Size);

    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr,
                                                  PngErrorFn, PngWarningFn);
    if (!png_ptr) return 0;

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_write_struct(&png_ptr, nullptr);
        return 0;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return 0;
    }

    size_t off = 0;

    png_fixed_point fw = static_cast<png_fixed_point>(ReadS32(Data, Size, off));
    png_fixed_point fwy = static_cast<png_fixed_point>(ReadS32(Data, Size, off));
    png_fixed_point frx = static_cast<png_fixed_point>(ReadS32(Data, Size, off));
    png_fixed_point fry = static_cast<png_fixed_point>(ReadS32(Data, Size, off));
    png_fixed_point fgx = static_cast<png_fixed_point>(ReadS32(Data, Size, off));
    png_fixed_point fgy = static_cast<png_fixed_point>(ReadS32(Data, Size, off));
    png_fixed_point fbx = static_cast<png_fixed_point>(ReadS32(Data, Size, off));
    png_fixed_point fby = static_cast<png_fixed_point>(ReadS32(Data, Size, off));

    png_fixed_point rX = static_cast<png_fixed_point>(ReadS32(Data, Size, off));
    png_fixed_point rY = static_cast<png_fixed_point>(ReadS32(Data, Size, off));
    png_fixed_point rZ = static_cast<png_fixed_point>(ReadS32(Data, Size, off));
    png_fixed_point gX = static_cast<png_fixed_point>(ReadS32(Data, Size, off));
    png_fixed_point gY = static_cast<png_fixed_point>(ReadS32(Data, Size, off));
    png_fixed_point gZ = static_cast<png_fixed_point>(ReadS32(Data, Size, off));
    png_fixed_point bX = static_cast<png_fixed_point>(ReadS32(Data, Size, off));
    png_fixed_point bY = static_cast<png_fixed_point>(ReadS32(Data, Size, off));
    png_fixed_point bZ = static_cast<png_fixed_point>(ReadS32(Data, Size, off));

    double dw = ReadDoubleLike(Data, Size, off);
    double dwy = ReadDoubleLike(Data, Size, off);
    double drx = ReadDoubleLike(Data, Size, off);
    double dry = ReadDoubleLike(Data, Size, off);
    double dgx = ReadDoubleLike(Data, Size, off);
    double dgy = ReadDoubleLike(Data, Size, off);
    double dbx = ReadDoubleLike(Data, Size, off);
    double dby = ReadDoubleLike(Data, Size, off);

    png_fixed_point out_fw = 0, out_fwy = 0, out_frx = 0, out_fry = 0;
    png_fixed_point out_fgx = 0, out_fgy = 0, out_fbx = 0, out_fby = 0;
    png_fixed_point out_rX = 0, out_rY = 0, out_rZ = 0;
    png_fixed_point out_gX = 0, out_gY = 0, out_gZ = 0;
    png_fixed_point out_bX = 0, out_bY = 0, out_bZ = 0;
    double out_dw = 0.0, out_dwy = 0.0, out_drx = 0.0, out_dry = 0.0;
    double out_dgx = 0.0, out_dgy = 0.0, out_dbx = 0.0, out_dby = 0.0;

    png_get_cHRM(png_ptr, info_ptr, &out_dw, &out_dwy, &out_drx, &out_dry,
                 &out_dgx, &out_dgy, &out_dbx, &out_dby);
    png_get_cHRM_fixed(png_ptr, info_ptr, &out_fw, &out_fwy, &out_frx, &out_fry,
                       &out_fgx, &out_fgy, &out_fbx, &out_fby);
    png_get_cHRM_XYZ_fixed(png_ptr, info_ptr, &out_rX, &out_rY, &out_rZ,
                           &out_gX, &out_gY, &out_gZ, &out_bX, &out_bY, &out_bZ);

    switch (Size ? Data[0] % 6 : 0) {
        case 0:
            png_set_cHRM_fixed(png_ptr, info_ptr, fw, fwy, frx, fry, fgx, fgy, fbx, fby);
            break;
        case 1:
            png_set_cHRM_XYZ_fixed(png_ptr, info_ptr, rX, rY, rZ, gX, gY, gZ, bX, bY, bZ);
            break;
        case 2:
            png_set_cHRM(png_ptr, info_ptr, dw, dwy, drx, dry, dgx, dgy, dbx, dby);
            break;
        case 3:
            png_set_cHRM_fixed(png_ptr, info_ptr, fw, fwy, frx, fry, fgx, fgy, fbx, fby);
            png_get_cHRM_fixed(png_ptr, info_ptr, &out_fw, &out_fwy, &out_frx, &out_fry,
                               &out_fgx, &out_fgy, &out_fbx, &out_fby);
            png_set_cHRM_XYZ_fixed(png_ptr, info_ptr, rX, rY, rZ, gX, gY, gZ, bX, bY, bZ);
            break;
        case 4:
            png_set_cHRM_XYZ_fixed(png_ptr, info_ptr, rX, rY, rZ, gX, gY, gZ, bX, bY, bZ);
            png_get_cHRM_XYZ_fixed(png_ptr, info_ptr, &out_rX, &out_rY, &out_rZ,
                                   &out_gX, &out_gY, &out_gZ, &out_bX, &out_bY, &out_bZ);
            png_set_cHRM(png_ptr, info_ptr, dw, dwy, drx, dry, dgx, dgy, dbx, dby);
            break;
        case 5:
            png_set_cHRM(png_ptr, info_ptr, dw, dwy, drx, dry, dgx, dgy, dbx, dby);
            png_get_cHRM(png_ptr, info_ptr, &out_dw, &out_dwy, &out_drx, &out_dry,
                         &out_dgx, &out_dgy, &out_dbx, &out_dby);
            png_set_cHRM_fixed(png_ptr, info_ptr, fw, fwy, frx, fry, fgx, fgy, fbx, fby);
            break;
    }

    png_get_cHRM(png_ptr, info_ptr,
                 (Size > 1 && (Data[1] & 1)) ? &out_dw : nullptr,
                 (Size > 1 && (Data[1] & 2)) ? &out_dwy : nullptr,
                 (Size > 1 && (Data[1] & 4)) ? &out_drx : nullptr,
                 (Size > 1 && (Data[1] & 8)) ? &out_dry : nullptr,
                 (Size > 1 && (Data[1] & 16)) ? &out_dgx : nullptr,
                 (Size > 1 && (Data[1] & 32)) ? &out_dgy : nullptr,
                 (Size > 1 && (Data[1] & 64)) ? &out_dbx : nullptr,
                 (Size > 1 && (Data[1] & 128)) ? &out_dby : nullptr);

    png_get_cHRM_fixed(png_ptr, info_ptr,
                       (Size > 2 && (Data[2] & 1)) ? &out_fw : nullptr,
                       (Size > 2 && (Data[2] & 2)) ? &out_fwy : nullptr,
                       (Size > 2 && (Data[2] & 4)) ? &out_frx : nullptr,
                       (Size > 2 && (Data[2] & 8)) ? &out_fry : nullptr,
                       (Size > 2 && (Data[2] & 16)) ? &out_fgx : nullptr,
                       (Size > 2 && (Data[2] & 32)) ? &out_fgy : nullptr,
                       (Size > 2 && (Data[2] & 64)) ? &out_fbx : nullptr,
                       (Size > 2 && (Data[2] & 128)) ? &out_fby : nullptr);

    png_get_cHRM_XYZ_fixed(png_ptr, info_ptr,
                           (Size > 3 && (Data[3] & 1)) ? &out_rX : nullptr,
                           (Size > 3 && (Data[3] & 2)) ? &out_rY : nullptr,
                           (Size > 3 && (Data[3] & 4)) ? &out_rZ : nullptr,
                           (Size > 3 && (Data[3] & 8)) ? &out_gX : nullptr,
                           (Size > 3 && (Data[3] & 16)) ? &out_gY : nullptr,
                           (Size > 3 && (Data[3] & 32)) ? &out_gZ : nullptr,
                           (Size > 3 && (Data[3] & 64)) ? &out_bX : nullptr,
                           (Size > 3 && (Data[3] & 128)) ? &out_bY : nullptr,
                           (Size > 4 && (Data[4] & 1)) ? &out_bZ : nullptr);

    png_destroy_write_struct(&png_ptr, &info_ptr);
    return 0;
}