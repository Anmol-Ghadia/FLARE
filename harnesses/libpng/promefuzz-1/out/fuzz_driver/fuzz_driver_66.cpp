// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_save_uint_32 at pngwutil.c:24:1 in png.h
// png_save_int_32 at png.c:726:1 in png.h
// png_save_uint_16 at pngwutil.c:37:1 in png.h
// png_set_oFFs at pngset.c:295:1 in png.h
// png_set_oFFs at pngset.c:295:1 in png.h
// png_set_oFFs at pngset.c:295:1 in png.h
// png_destroy_info_struct at png.c:386:1 in png.h
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
#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <vector>

static png_structp CreatePngStruct() {
    return png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
}

static png_infop CreatePngInfo(png_structp png_ptr) {
    if (png_ptr == nullptr) return nullptr;
    return png_create_info_struct(png_ptr);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    uint8_t in4[4] = {0, 0, 0, 0};
    uint8_t in2[2] = {0, 0};

    if (Size > 0) {
        size_t copy4 = Size < 4 ? Size : 4;
        memcpy(in4, Data, copy4);

        size_t copy2 = Size < 2 ? Size : 2;
        memcpy(in2, Data, copy2);
    }

    png_uint_32 u32 = png_get_uint_32(in4);
    png_int_32 i32 = png_get_int_32(in4);

    uint8_t out4a[4] = {0};
    uint8_t out4b[4] = {0};
    uint8_t out2[2] = {0};

    png_save_uint_32(out4a, u32);
    png_save_int_32(out4b, i32);
    png_save_uint_16(out2, static_cast<unsigned int>((in2[0] << 8) | in2[1]));

    png_uint_32 round_u32_a = png_get_uint_32(out4a);
    png_int_32 round_i32_b = png_get_int_32(out4b);

    png_structp png_ptr = CreatePngStruct();
    if (png_ptr != nullptr) {
        png_infop info_ptr = CreatePngInfo(png_ptr);
        if (info_ptr != nullptr) {
            int unit_type = 0;
            if (Size > 4) {
                unit_type = static_cast<int>(Data[4]);
            }

            png_set_oFFs(png_ptr, info_ptr, i32, static_cast<png_int_32>(round_u32_a), unit_type);
            png_set_oFFs(png_ptr, info_ptr, static_cast<png_int_32>(round_i32_b), i32 ^ static_cast<png_int_32>(u32), unit_type & 1);

            if (Size >= 8) {
                png_int_32 x = static_cast<png_int_32>(png_get_int_32(Data));
                png_int_32 y = static_cast<png_int_32>(png_get_int_32(Data + 4));
                png_set_oFFs(png_ptr, info_ptr, x, y, unit_type % 4);
            }

            png_destroy_info_struct(png_ptr, &info_ptr);
        }
        png_destroy_write_struct(&png_ptr, nullptr);
    }

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp != nullptr) {
        if (Size > 0) {
            fwrite(Data, 1, Size, fp);
        } else {
            fwrite(out4a, 1, sizeof(out4a), fp);
            fwrite(out2, 1, sizeof(out2), fp);
        }
        fclose(fp);
    }

    return 0;
}