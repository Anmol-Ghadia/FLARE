// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_read_update_info at pngread.c:267:1 in png.h
// png_read_end at pngread.c:766:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_get_bit_depth at pngget.c:70:1 in png.h
// png_get_color_type at pngget.c:79:1 in png.h
// png_get_image_width at pngget.c:52:1 in png.h
// png_get_image_height at pngget.c:61:1 in png.h
// png_get_interlace_type at pngget.c:97:1 in png.h
// png_get_channels at pngget.c:467:1 in png.h
// png_get_bit_depth at pngget.c:70:1 in png.h
// png_get_color_type at pngget.c:79:1 in png.h
// png_get_image_width at pngget.c:52:1 in png.h
// png_get_image_height at pngget.c:61:1 in png.h
// png_get_interlace_type at pngget.c:97:1 in png.h
// png_get_channels at pngget.c:467:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_init_io at png.c:703:1 in png.h
// png_set_sig_bytes at png.c:47:1 in png.h
// png_read_info at pngread.c:92:1 in png.h
// png_get_image_width at pngget.c:52:1 in png.h
// png_get_image_height at pngget.c:61:1 in png.h
// png_get_color_type at pngget.c:79:1 in png.h
// png_get_bit_depth at pngget.c:70:1 in png.h
// png_get_interlace_type at pngget.c:97:1 in png.h
// png_get_channels at pngget.c:467:1 in png.h
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
#include <vector>

static void ConsumeAccessors(png_structp png_ptr, png_infop info_ptr) {
    (void)png_get_bit_depth(png_ptr, info_ptr);
    (void)png_get_color_type(png_ptr, info_ptr);
    (void)png_get_image_width(png_ptr, info_ptr);
    (void)png_get_image_height(png_ptr, info_ptr);
    (void)png_get_interlace_type(png_ptr, info_ptr);
    (void)png_get_channels(png_ptr, info_ptr);
}

extern "C" int LLVMFuzzerTestOneInput_76(const uint8_t *Data, size_t Size) {
    // Exercise documented NULL-handling paths.
    (void)png_get_bit_depth(nullptr, nullptr);
    (void)png_get_color_type(nullptr, nullptr);
    (void)png_get_image_width(nullptr, nullptr);
    (void)png_get_image_height(nullptr, nullptr);
    (void)png_get_interlace_type(nullptr, nullptr);
    (void)png_get_channels(nullptr, nullptr);

    FILE *fp = std::fopen("./dummy_file", "wb");
    if (fp == nullptr) {
        return 0;
    }

    if (Size > 0) {
        (void)std::fwrite(Data, 1, Size, fp);
    }
    std::fclose(fp);

    fp = std::fopen("./dummy_file", "rb");
    if (fp == nullptr) {
        return 0;
    }

    png_structp png_ptr =
        png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (png_ptr == nullptr) {
        std::fclose(fp);
        return 0;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == nullptr) {
        png_destroy_read_struct(&png_ptr, nullptr, nullptr);
        std::fclose(fp);
        return 0;
    }

    png_infop end_info = png_create_info_struct(png_ptr);
    if (setjmp(png_jmpbuf(png_ptr))) {
        ConsumeAccessors(png_ptr, info_ptr);
        if (end_info != nullptr) {
            ConsumeAccessors(png_ptr, end_info);
        }
        png_destroy_read_struct(&png_ptr, &info_ptr, end_info ? &end_info : nullptr);
        std::fclose(fp);
        return 0;
    }

    // Accessors before any parsing.
    ConsumeAccessors(png_ptr, info_ptr);
    if (end_info != nullptr) {
        ConsumeAccessors(png_ptr, end_info);
    }

    png_init_io(png_ptr, fp);

    // Vary parser state based on input.
    if (Size > 0) {
        png_set_sig_bytes(png_ptr, static_cast<int>(Data[0] & 0x7));
    }

    // Try a lightweight parse first.
    png_read_info(png_ptr, info_ptr);
    ConsumeAccessors(png_ptr, info_ptr);

    // Explore additional states if IHDR was parsed successfully.
    png_uint_32 width = png_get_image_width(png_ptr, info_ptr);
    png_uint_32 height = png_get_image_height(png_ptr, info_ptr);
    png_byte color_type = png_get_color_type(png_ptr, info_ptr);
    png_byte bit_depth = png_get_bit_depth(png_ptr, info_ptr);
    png_byte interlace_type = png_get_interlace_type(png_ptr, info_ptr);
    png_byte channels = png_get_channels(png_ptr, info_ptr);

    (void)width;
    (void)height;
    (void)color_type;
    (void)bit_depth;
    (void)interlace_type;
    (void)channels;

    // Optionally continue deeper into the decoder to mutate info state.
    if (Size > 1 && (Data[1] & 1)) {
        png_read_update_info(png_ptr, info_ptr);
        ConsumeAccessors(png_ptr, info_ptr);
    }

    if (Size > 2 && (Data[2] & 1)) {
        png_read_end(png_ptr, end_info);
        if (end_info != nullptr) {
            ConsumeAccessors(png_ptr, end_info);
        }
    }

    png_destroy_read_struct(&png_ptr, &info_ptr, end_info ? &end_info : nullptr);
    std::fclose(fp);
    return 0;
}