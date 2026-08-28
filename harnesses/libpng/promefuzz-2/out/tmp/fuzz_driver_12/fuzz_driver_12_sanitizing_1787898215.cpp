#include <png.h>
#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <csetjmp>

static uint32_t ReadU32(const uint8_t* data, size_t size, size_t offset) {
    if (offset + 4 > size) return 0;
    return (static_cast<uint32_t>(data[offset]) << 24) |
           (static_cast<uint32_t>(data[offset + 1]) << 16) |
           (static_cast<uint32_t>(data[offset + 2]) << 8) |
           static_cast<uint32_t>(data[offset + 3]);
}

static int32_t ReadS32(const uint8_t* data, size_t size, size_t offset) {
    return static_cast<int32_t>(ReadU32(data, size, offset));
}

static void SafeWriteDummyFile(const uint8_t* data, size_t size) {
    FILE* fp = std::fopen("./dummy_file", "wb");
    if (!fp) return;
    if (size > 0) std::fwrite(data, 1, size, fp);
    std::fclose(fp);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    SafeWriteDummyFile(Data, Size);

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png_ptr) return 0;

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_read_struct(&png_ptr, nullptr, nullptr);
        return 0;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
        return 0;
    }

    int32_t offset_x1 = ReadS32(Data, Size, 0);
    int32_t offset_y1 = ReadS32(Data, Size, 4);
    int32_t offset_x2 = ReadS32(Data, Size, 8);
    int32_t offset_y2 = ReadS32(Data, Size, 12);

    int unit_type1 = (Size > 16) ? static_cast<int>(Data[16] % 4) : 0;
    int unit_type2 = (Size > 17) ? static_cast<int>(Data[17] % 4) : 1;

    png_fixed_point width1 = ReadS32(Data, Size, 18);
    png_fixed_point height1 = ReadS32(Data, Size, 22);
    png_fixed_point width2 = ReadS32(Data, Size, 26);
    png_fixed_point height2 = ReadS32(Data, Size, 30);

    int scal_unit1 = (Size > 34) ? static_cast<int>(Data[34] % 4) : 0;
    int scal_unit2 = (Size > 35) ? static_cast<int>(Data[35] % 4) : 1;

    png_set_oFFs(png_ptr, info_ptr, offset_x1, offset_y1, unit_type1);
    (void)png_get_y_offset_pixels(png_ptr, info_ptr);
    (void)png_get_x_offset_microns(png_ptr, info_ptr);
    (void)png_get_y_offset_microns(png_ptr, info_ptr);
#ifdef PNG_FIXED_POINT_SUPPORTED
    (void)png_get_x_offset_inches_fixed(png_ptr, info_ptr);
#endif

#ifdef PNG_FIXED_POINT_SUPPORTED
    if (width1 > 0 && height1 > 0) {
        png_set_sCAL_fixed(png_ptr, info_ptr, scal_unit1, width1, height1);
    }
#endif

    png_set_oFFs(png_ptr, info_ptr, offset_x2, offset_y2, unit_type2);
    (void)png_get_y_offset_pixels(png_ptr, info_ptr);
    (void)png_get_x_offset_microns(png_ptr, info_ptr);
    (void)png_get_y_offset_microns(png_ptr, info_ptr);
#ifdef PNG_FIXED_POINT_SUPPORTED
    (void)png_get_x_offset_inches_fixed(png_ptr, info_ptr);
#endif

#ifdef PNG_FIXED_POINT_SUPPORTED
    if (width2 > 0 && height2 > 0) {
        png_set_sCAL_fixed(png_ptr, info_ptr, scal_unit2, width2, height2);
    }
#endif

    (void)png_get_y_offset_pixels(nullptr, info_ptr);
    (void)png_get_x_offset_microns(png_ptr, nullptr);
    (void)png_get_y_offset_microns(nullptr, nullptr);
#ifdef PNG_FIXED_POINT_SUPPORTED
    (void)png_get_x_offset_inches_fixed(nullptr, info_ptr);
#endif

    png_set_oFFs(nullptr, info_ptr, offset_x1, offset_y1, unit_type1);
    png_set_oFFs(png_ptr, nullptr, offset_x1, offset_y1, unit_type1);
#ifdef PNG_FIXED_POINT_SUPPORTED
    png_set_sCAL_fixed(nullptr, info_ptr, scal_unit1, width1, height1);
    png_set_sCAL_fixed(png_ptr, nullptr, scal_unit1, width1, height1);
#endif

    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    return 0;
}