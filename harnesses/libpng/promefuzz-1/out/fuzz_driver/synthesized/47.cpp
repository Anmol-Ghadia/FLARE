// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_user_transform_info at pngtrans.c:802:1 in png.h
// png_set_write_user_transform_fn at pngwrite.c:1322:1 in png.h
// png_set_write_user_transform_fn at pngwrite.c:1322:1 in png.h
// png_get_user_transform_ptr at pngtrans.c:833:1 in png.h
// png_set_write_user_transform_fn at pngwrite.c:1322:1 in png.h
// png_set_write_user_transform_fn at pngwrite.c:1322:1 in png.h
// png_set_write_user_transform_fn at pngwrite.c:1322:1 in png.h
// png_get_user_transform_ptr at pngtrans.c:833:1 in png.h
// png_set_swap_alpha at pngtrans.c:223:1 in png.h
// png_set_invert_alpha at pngtrans.c:237:1 in png.h
// png_get_user_transform_ptr at pngtrans.c:833:1 in png.h
// png_set_write_user_transform_fn at pngwrite.c:1322:1 in png.h
// png_set_write_user_transform_fn at pngwrite.c:1322:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_set_strip_alpha at pngrtran.c:210:1 in png.h
// png_set_swap_alpha at pngtrans.c:223:1 in png.h
// png_set_expand at pngrtran.c:872:1 in png.h
// png_set_invert_alpha at pngtrans.c:237:1 in png.h
// png_set_expand at pngrtran.c:872:1 in png.h
// png_set_strip_alpha at pngrtran.c:210:1 in png.h
// png_set_invert_alpha at pngtrans.c:237:1 in png.h
// png_set_swap_alpha at pngtrans.c:223:1 in png.h
// png_set_strip_alpha at pngrtran.c:210:1 in png.h
// png_set_swap_alpha at pngtrans.c:223:1 in png.h
// png_set_expand at pngrtran.c:872:1 in png.h
// png_set_invert_alpha at pngtrans.c:237:1 in png.h
// png_set_strip_alpha at pngrtran.c:210:1 in png.h
// png_set_swap_alpha at pngtrans.c:223:1 in png.h
// png_set_expand at pngrtran.c:872:1 in png.h
// png_set_invert_alpha at pngtrans.c:237:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
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

static void UserWriteTransform(png_structp png_ptr, png_row_infop row_info, png_bytep data)
{
    (void)png_ptr;
    if (row_info == nullptr || data == nullptr)
        return;

    png_uint_32 width = row_info->width;
    png_size_t rowbytes = row_info->rowbytes;

    if (rowbytes == 0)
        return;

    if (width > 0) {
        data[0] ^= static_cast<png_byte>(width & 0xFF);
        data[rowbytes - 1] ^= static_cast<png_byte>((row_info->color_type << 1) ^ row_info->bit_depth);
    } else {
        data[0] ^= 0x5A;
    }
}

static void PngErrorFn(png_structp png_ptr, png_const_charp)
{
    longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp)
{
}

extern "C" int LLVMFuzzerTestOneInput_47(const uint8_t *Data, size_t Size)
{
    FILE *fp = std::fopen("./dummy_file", "wb");
    if (fp != nullptr) {
        if (Size > 0)
            std::fwrite(Data, 1, Size, fp);
        std::fclose(fp);
    }

    png_structp read_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn, PngWarningFn);
    png_infop read_info = nullptr;
    if (read_ptr != nullptr)
        read_info = png_create_info_struct(read_ptr);

    if (read_ptr != nullptr && read_info != nullptr) {
        if (setjmp(png_jmpbuf(read_ptr)) == 0) {
            if (Size > 0) {
                if (Data[0] & 0x01) png_set_strip_alpha(read_ptr);
                if (Data[0] & 0x02) png_set_swap_alpha(read_ptr);
                if (Data[0] & 0x04) png_set_expand(read_ptr);
                if (Data[0] & 0x08) png_set_invert_alpha(read_ptr);

                if (Size > 1) {
                    if (Data[1] & 0x01) png_set_expand(read_ptr);
                    if (Data[1] & 0x02) png_set_strip_alpha(read_ptr);
                    if (Data[1] & 0x04) png_set_invert_alpha(read_ptr);
                    if (Data[1] & 0x08) png_set_swap_alpha(read_ptr);
                }
            } else {
                png_set_strip_alpha(read_ptr);
                png_set_swap_alpha(read_ptr);
                png_set_expand(read_ptr);
                png_set_invert_alpha(read_ptr);
            }
        }
    }

    png_set_strip_alpha(nullptr);
    png_set_swap_alpha(nullptr);
    png_set_expand(nullptr);
    png_set_invert_alpha(nullptr);

    png_structp write_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn, PngWarningFn);
    png_infop write_info = nullptr;
    if (write_ptr != nullptr)
        write_info = png_create_info_struct(write_ptr);

    if (write_ptr != nullptr && write_info != nullptr) {
        if (setjmp(png_jmpbuf(write_ptr)) == 0) {
            png_voidp user_ctx = nullptr;
            if (Size >= sizeof(uintptr_t)) {
                uintptr_t v = 0;
                std::memcpy(&v, Data, sizeof(uintptr_t));
                user_ctx = reinterpret_cast<png_voidp>(v);
            } else if (Size > 0) {
                user_ctx = const_cast<uint8_t*>(Data);
            }

            png_set_user_transform_info(write_ptr, user_ctx, 8, 4);

            if (Size == 0 || (Data[0] & 0x10))
                png_set_write_user_transform_fn(write_ptr, UserWriteTransform);
            else
                png_set_write_user_transform_fn(write_ptr, nullptr);

            (void)png_get_user_transform_ptr(write_ptr);

            if (Size > 2) {
                if (Data[2] & 0x01)
                    png_set_write_user_transform_fn(write_ptr, UserWriteTransform);
                if (Data[2] & 0x02)
                    png_set_write_user_transform_fn(write_ptr, nullptr);
                if (Data[2] & 0x04)
                    png_set_write_user_transform_fn(write_ptr, UserWriteTransform);
            }

            (void)png_get_user_transform_ptr(write_ptr);

            if (Size > 3) {
                if (Data[3] & 0x01) png_set_swap_alpha(write_ptr);
                if (Data[3] & 0x02) png_set_invert_alpha(write_ptr);
            }
        }
    }

    (void)png_get_user_transform_ptr(nullptr);
    png_set_write_user_transform_fn(nullptr, UserWriteTransform);
    png_set_write_user_transform_fn(nullptr, nullptr);

    if (read_ptr != nullptr || read_info != nullptr)
        png_destroy_read_struct(&read_ptr, &read_info, nullptr);

    if (write_ptr != nullptr || write_info != nullptr)
        png_destroy_write_struct(&write_ptr, &write_info);

    return 0;
}