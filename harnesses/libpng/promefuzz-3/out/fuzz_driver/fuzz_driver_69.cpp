// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_tRNS_to_alpha at pngrtran.c:926:1 in png.h
// png_set_scale_16 at pngrtran.c:183:1 in png.h
// png_set_invert_mono at pngtrans.c:250:1 in png.h
// png_set_expand at pngrtran.c:872:1 in png.h
// png_set_invert_alpha at pngtrans.c:237:1 in png.h
// png_get_IHDR at pngget.c:825:1 in png.h
// png_read_update_info at pngread.c:267:1 in png.h
// png_get_rowbytes at pngget.c:30:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_read_row at pngread.c:383:1 in png.h
// png_read_row at pngread.c:383:1 in png.h
// png_read_row at pngread.c:383:1 in png.h
// png_read_row at pngread.c:383:1 in png.h
// png_read_end at pngread.c:766:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_read_fn at pngrio.c:87:1 in png.h
// png_set_tRNS_to_alpha at pngrtran.c:926:1 in png.h
// png_set_scale_16 at pngrtran.c:183:1 in png.h
// png_set_invert_mono at pngtrans.c:250:1 in png.h
// png_set_expand at pngrtran.c:872:1 in png.h
// png_set_invert_alpha at pngtrans.c:237:1 in png.h
// png_read_info at pngread.c:92:1 in png.h
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
#include <stdlib.h>
#include <string.h>

static void PngReadCallback(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead) {
    png_bytep io_ptr = static_cast<png_bytep>(png_get_io_ptr(png_ptr));
    if (io_ptr == NULL) {
        png_error(png_ptr, "No IO pointer");
        return;
    }

    struct State {
        const uint8_t* data;
        size_t size;
        size_t offset;
    };

    State* st = reinterpret_cast<State*>(io_ptr);
    if (st->offset > st->size || byteCountToRead > st->size - st->offset) {
        png_error(png_ptr, "Read beyond end");
        return;
    }

    memcpy(outBytes, st->data + st->offset, byteCountToRead);
    st->offset += byteCountToRead;
}

static void PngErrorCallback(png_structp png_ptr, png_const_charp) {
    longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningCallback(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    if (Data == NULL || Size == 0) {
        return 0;
    }

    FILE* fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    struct State {
        const uint8_t* data;
        size_t size;
        size_t offset;
    } state = { Data, Size, 0 };

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL,
                                                 PngErrorCallback, PngWarningCallback);
    if (png_ptr == NULL) {
        return 0;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        return 0;
    }

    png_infop end_info = png_create_info_struct(png_ptr);
    if (end_info == NULL) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return 0;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        return 0;
    }

    png_set_read_fn(png_ptr, &state, PngReadCallback);

    if (Size > 0) {
        if (Data[0] & 0x01) png_set_tRNS_to_alpha(png_ptr);
        if (Data[0] & 0x02) png_set_scale_16(png_ptr);
        if (Data[0] & 0x04) png_set_invert_mono(png_ptr);
        if (Data[0] & 0x08) png_set_expand(png_ptr);
        if (Data[0] & 0x10) png_set_invert_alpha(png_ptr);
    }

    png_read_info(png_ptr, info_ptr);

    if (Size > 1) {
        if (Data[1] & 0x01) png_set_tRNS_to_alpha(png_ptr);
        if (Data[1] & 0x02) png_set_scale_16(png_ptr);
        if (Data[1] & 0x04) png_set_invert_mono(png_ptr);
        if (Data[1] & 0x08) png_set_expand(png_ptr);
        if (Data[1] & 0x10) png_set_invert_alpha(png_ptr);
    }

    png_uint_32 width = 0, height = 0;
    int bit_depth = 0, color_type = 0, interlace_type = 0, compression_type = 0, filter_method = 0;
    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
                 &interlace_type, &compression_type, &filter_method);

    png_read_update_info(png_ptr, info_ptr);

    png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);
    if (rowbytes == 0) {
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        return 0;
    }

    if (rowbytes > (1U << 20)) {
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        return 0;
    }

    png_bytep row = static_cast<png_bytep>(malloc(rowbytes));
    png_bytep display_row = static_cast<png_bytep>(malloc(rowbytes));
    if (row == NULL || display_row == NULL) {
        free(row);
        free(display_row);
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        return 0;
    }

    memset(row, 0, rowbytes);
    memset(display_row, 0, rowbytes);

    png_uint_32 max_rows = height;
    if (max_rows > 1024) {
        max_rows = 1024;
    }

    for (png_uint_32 y = 0; y < max_rows; ++y) {
        if (Size > 2 + y) {
            switch (Data[2 + y] % 3) {
                case 0:
                    png_read_row(png_ptr, row, NULL);
                    break;
                case 1:
                    png_read_row(png_ptr, row, display_row);
                    break;
                case 2:
                    png_read_row(png_ptr, NULL, NULL);
                    break;
            }
        } else {
            png_read_row(png_ptr, row, NULL);
        }
    }

    png_read_end(png_ptr, end_info);

    free(row);
    free(display_row);
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    return 0;
}