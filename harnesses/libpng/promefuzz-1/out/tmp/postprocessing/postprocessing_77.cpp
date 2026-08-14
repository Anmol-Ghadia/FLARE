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
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <setjmp.h>

static png_voidp fuzz_malloc(png_structp, png_alloc_size_t size) {
    if (size == 0) return nullptr;
    return malloc((size_t)size);
}

static void fuzz_free(png_structp, png_voidp ptr) {
    free(ptr);
}

static void fuzz_error_fn(png_structp png_ptr, png_const_charp) {
    jmp_buf* jb = reinterpret_cast<jmp_buf*>(png_get_error_ptr(png_ptr));
    if (jb) longjmp(*jb, 1);
}

static void fuzz_warning_fn(png_structp, png_const_charp) {
}

static uint32_t read_u32(const uint8_t* data, size_t size, size_t* off) {
    if (*off + 4 > size) return 0;
    uint32_t v = ((uint32_t)data[*off] << 24) |
                 ((uint32_t)data[*off + 1] << 16) |
                 ((uint32_t)data[*off + 2] << 8) |
                 ((uint32_t)data[*off + 3]);
    *off += 4;
    return v;
}

static int32_t read_i32(const uint8_t* data, size_t size, size_t* off) {
    return (int32_t)read_u32(data, size, off);
}

static uint8_t read_u8(const uint8_t* data, size_t size, size_t* off) {
    if (*off >= size) return 0;
    return data[(*off)++];
}

static char* make_cstring(const uint8_t* data, size_t size, size_t* off, size_t max_len) {
    size_t remaining = (*off < size) ? (size - *off) : 0;
    size_t len = remaining ? (read_u8(data, size, off) % (max_len + 1)) : 0;
    if (len > size - *off) len = size - *off;
    char* s = (char*)malloc(len + 1);
    if (!s) return nullptr;
    for (size_t i = 0; i < len; ++i) {
        unsigned char c = data[*off + i];
        if (c == 0) c = '0';
        s[i] = (char)c;
    }
    s[len] = '\0';
    *off += len;
    return s;
}

static char* make_numeric_string(const uint8_t* data, size_t size, size_t* off) {
    size_t remaining = (*off < size) ? (size - *off) : 0;
    size_t len = remaining ? (read_u8(data, size, off) % 24) : 0;
    char* s = (char*)malloc(len + 2);
    if (!s) return nullptr;
    if (len == 0) {
        s[0] = '1';
        s[1] = '\0';
        return s;
    }

    static const char alphabet[] = "0123456789.";
    int have_digit = 0;
    int have_dot = 0;
    size_t out = 0;

    for (size_t i = 0; i < len && *off < size; ++i, ++(*off)) {
        char c = alphabet[data[*off] % (sizeof(alphabet) - 1)];
        if (c == '.') {
            if (have_dot) c = '0';
            else have_dot = 1;
        } else {
            have_digit = 1;
        }
        s[out++] = c;
    }

    if (!have_digit) s[out++] = '1';
    s[out] = '\0';
    return s;
}

static double read_doubleish(const uint8_t* data, size_t size, size_t* off) {
    uint32_t a = read_u32(data, size, off);
    double v = (double)(a % 1000000) / 1000.0;
    if (v <= 0.0) v = 1.0;
    return v;
}

static png_fixed_point read_fixed_positive(const uint8_t* data, size_t size, size_t* off) {
    int32_t v = read_i32(data, size, off);
    if (v <= 0) v = (int32_t)((read_u32(data, size, off) % 100000) + 1);
    return v;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE* fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size) fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    jmp_buf jmpbuf;
    png_structp png_ptr = png_create_write_struct_2(
        PNG_LIBPNG_VER_STRING, &jmpbuf, fuzz_error_fn, fuzz_warning_fn,
        nullptr, fuzz_malloc, fuzz_free);
    if (!png_ptr) return 0;

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_write_struct(&png_ptr, nullptr);
        return 0;
    }

    if (setjmp(jmpbuf)) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return 0;
    }

    size_t off = 0;

    for (int iter = 0; iter < 8 && off < Size; ++iter) {
        uint8_t op = read_u8(Data, Size, &off) % 6;

        if (op == 0) {
            int unit = (read_u8(Data, Size, &off) % 2) + 1;
            char* swidth = make_numeric_string(Data, Size, &off);
            char* sheight = make_numeric_string(Data, Size, &off);
            if (swidth && sheight) {
                png_set_sCAL_s(png_ptr, info_ptr, unit, swidth, sheight);
            }
            free(swidth);
            free(sheight);
        } else if (op == 1) {
            int unit = (read_u8(Data, Size, &off) % 4);
            double width = read_doubleish(Data, Size, &off);
            double height = read_doubleish(Data, Size, &off);
            png_set_sCAL(png_ptr, info_ptr, unit, width, height);
        } else if (op == 2) {
            int unit = (read_u8(Data, Size, &off) % 4);
            png_fixed_point width = read_fixed_positive(Data, Size, &off);
            png_fixed_point height = read_fixed_positive(Data, Size, &off);
            png_set_sCAL_fixed(png_ptr, info_ptr, unit, width, height);
        } else if (op == 3) {
            char* purpose = make_cstring(Data, Size, &off, 32);
            char* units = make_cstring(Data, Size, &off, 32);
            int32_t X0 = read_i32(Data, Size, &off);
            int32_t X1 = read_i32(Data, Size, &off);
            int type = read_u8(Data, Size, &off) % 4;
            int nparams = read_u8(Data, Size, &off) % 8;

            png_charpp params = nullptr;
            if (nparams > 0) {
                params = (png_charpp)malloc(sizeof(char*) * (size_t)nparams);
                if (params) {
                    memset(params, 0, sizeof(char*) * (size_t)nparams);
                    for (int i = 0; i < nparams; ++i) {
                        params[i] = make_cstring(Data, Size, &off, 24);
                    }
                } else {
                    nparams = 0;
                }
            }

            if (purpose && units) {
                png_set_pCAL(png_ptr, info_ptr, purpose, X0, X1, type, nparams, units, params);
            }

            if (params) {
                for (int i = 0; i < nparams; ++i) free(params[i]);
                free(params);
            }
            free(purpose);
            free(units);
        } else if (op == 4) {
            int unit = 0;
            png_charp swidth = nullptr;
            png_charp sheight = nullptr;
            (void)png_get_sCAL_s(png_ptr, info_ptr, &unit, &swidth, &sheight);
        } else {
            png_charp purpose = nullptr;
            png_int_32 X0 = 0, X1 = 0;
            int type = 0, nparams = 0;
            png_charp units = nullptr;
            png_charpp params = nullptr;
            (void)png_get_pCAL(png_ptr, info_ptr, &purpose, &X0, &X1, &type, &nparams, &units, &params);
        }
    }

    {
        int unit = 0;
        png_charp swidth = nullptr;
        png_charp sheight = nullptr;
        (void)png_get_sCAL_s(png_ptr, info_ptr, &unit, &swidth, &sheight);
    }

    {
        png_charp purpose = nullptr;
        png_int_32 X0 = 0, X1 = 0;
        int type = 0, nparams = 0;
        png_charp units = nullptr;
        png_charpp params = nullptr;
        (void)png_get_pCAL(png_ptr, info_ptr, &purpose, &X0, &X1, &type, &nparams, &units, &params);
    }

    png_destroy_write_struct(&png_ptr, &info_ptr);
    return 0;
}