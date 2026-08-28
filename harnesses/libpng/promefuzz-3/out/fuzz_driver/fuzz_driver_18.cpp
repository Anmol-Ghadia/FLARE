// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_get_rowbytes at pngget.c:30:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_read_row at pngread.c:383:1 in png.h
// png_read_image at pngread.c:700:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_progressive_read_fn at pngpread.c:1074:1 in png.h
// png_set_tRNS_to_alpha at pngrtran.c:926:1 in png.h
// png_set_gray_to_rgb at pngrtran.c:955:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_process_data at pngpread.c:36:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_get_progressive_ptr at pngpread.c:1089:1 in png.h
// png_get_rowbytes at pngget.c:30:1 in png.h
// png_progressive_combine_row at pngpread.c:1058:1 in png.h
// png_progressive_combine_row at pngpread.c:1058:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_read_fn at pngrio.c:87:1 in png.h
// png_set_tRNS_to_alpha at pngrtran.c:926:1 in png.h
// png_set_gray_to_rgb at pngrtran.c:955:1 in png.h
// png_read_info at pngread.c:92:1 in png.h
// png_set_tRNS_to_alpha at pngrtran.c:926:1 in png.h
// png_set_gray_to_rgb at pngrtran.c:955:1 in png.h
// png_read_update_info at pngread.c:267:1 in png.h
// png_get_IHDR at pngget.c:825:1 in png.h
#include <png.h>
#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <vector>

struct BufferState {
    const uint8_t* data;
    size_t size;
    size_t offset;
};

static void PngReadCallback(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead) {
    BufferState* state = static_cast<BufferState*>(png_get_io_ptr(png_ptr));
    if (state == nullptr || outBytes == nullptr) {
        png_error(png_ptr, "invalid read state");
        return;
    }

    if (state->offset > state->size || byteCountToRead > state->size - state->offset) {
        png_error(png_ptr, "read overflow");
        return;
    }

    for (png_size_t i = 0; i < byteCountToRead; ++i) {
        outBytes[i] = state->data[state->offset + i];
    }
    state->offset += byteCountToRead;
}

static void NoopProgressiveInfo(png_structp, png_infop) {}

static void NoopProgressiveEnd(png_structp, png_infop) {}

static void ProgressiveRowCallback(png_structp png_ptr, png_bytep new_row, png_uint_32, int) {
    png_infop info_ptr = static_cast<png_infop>(png_get_progressive_ptr(png_ptr));
    if (info_ptr == nullptr) {
        return;
    }

    png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);
    if (rowbytes == 0) {
        return;
    }

    std::vector<png_byte> old_row(rowbytes);
    png_progressive_combine_row(png_ptr, old_row.data(), new_row);
    png_progressive_combine_row(png_ptr, old_row.data(), nullptr);
}

static void FuzzReadFromMemory(const uint8_t* Data, size_t Size, bool use_row_mode, bool set_trns_first, bool set_gray_first) {
    BufferState state{Data, Size, 0};

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (png_ptr == nullptr) {
        return;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == nullptr) {
        png_destroy_read_struct(&png_ptr, nullptr, nullptr);
        return;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
        return;
    }

    png_set_read_fn(png_ptr, &state, PngReadCallback);

    if (set_trns_first) {
        png_set_tRNS_to_alpha(png_ptr);
    }
    if (set_gray_first) {
        png_set_gray_to_rgb(png_ptr);
    }

    png_read_info(png_ptr, info_ptr);

    if (!set_trns_first) {
        png_set_tRNS_to_alpha(png_ptr);
    }
    if (!set_gray_first) {
        png_set_gray_to_rgb(png_ptr);
    }

    png_read_update_info(png_ptr, info_ptr);

    png_uint_32 width = 0, height = 0;
    int bit_depth = 0, color_type = 0, interlace_type = 0, compression_type = 0, filter_method = 0;
    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
                 &interlace_type, &compression_type, &filter_method);

    (void)bit_depth;
    (void)color_type;
    (void)interlace_type;
    (void)compression_type;
    (void)filter_method;
    (void)width;

    png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);
    if (rowbytes == 0 || height == 0) {
        png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
        return;
    }

    if (use_row_mode) {
        std::vector<png_byte> row(rowbytes);
        std::vector<png_byte> display_row(rowbytes);
        for (png_uint_32 y = 0; y < height; ++y) {
            png_bytep display = (y & 1) ? display_row.data() : nullptr;
            png_read_row(png_ptr, row.data(), display);
        }
    } else {
        std::vector<std::vector<png_byte> > rows(height, std::vector<png_byte>(rowbytes));
        std::vector<png_bytep> row_ptrs(height);
        for (png_uint_32 y = 0; y < height; ++y) {
            row_ptrs[y] = rows[y].data();
        }
        png_read_image(png_ptr, row_ptrs.data());
    }

    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
}

static void FuzzProgressive(const uint8_t* Data, size_t Size) {
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (png_ptr == nullptr) {
        return;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == nullptr) {
        png_destroy_read_struct(&png_ptr, nullptr, nullptr);
        return;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
        return;
    }

    png_set_progressive_read_fn(png_ptr, info_ptr, NoopProgressiveInfo, ProgressiveRowCallback, NoopProgressiveEnd);
    png_set_tRNS_to_alpha(png_ptr);
    png_set_gray_to_rgb(png_ptr);

    size_t offset = 0;
    while (offset < Size) {
        size_t chunk = 1 + (Data[offset] % 32);
        if (chunk > Size - offset) {
            chunk = Size - offset;
        }
        png_process_data(png_ptr, info_ptr, const_cast<png_bytep>(Data + offset), static_cast<png_size_t>(chunk));
        offset += chunk;
    }

    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE* fp = std::fopen("./dummy_file", "wb");
    if (fp != nullptr) {
        if (Size > 0) {
            std::fwrite(Data, 1, Size, fp);
        }
        std::fclose(fp);
    }

    FuzzReadFromMemory(Data, Size, false, false, false);
    FuzzReadFromMemory(Data, Size, true, true, false);
    FuzzReadFromMemory(Data, Size, true, false, true);
    FuzzReadFromMemory(Data, Size, false, true, true);
    FuzzProgressive(Data, Size);

    return 0;
}