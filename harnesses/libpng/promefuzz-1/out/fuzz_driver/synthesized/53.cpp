// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_read_end at pngread.c:766:1 in png.h
// png_get_palette_max at pngget.c:1239:1 in png.h
// png_get_IHDR at pngget.c:825:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_option at png.c:4332:1 in png.h
// png_get_IHDR at pngget.c:825:1 in png.h
// png_get_palette_max at pngget.c:1239:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_init_io at png.c:703:1 in png.h
// png_set_sig_bytes at png.c:47:1 in png.h
// png_set_check_for_invalid_index at pngset.c:1701:1 in png.h
// png_set_expand at pngrtran.c:872:1 in png.h
// png_set_option at png.c:4332:1 in png.h
// png_set_option at png.c:4332:1 in png.h
// png_set_option at png.c:4332:1 in png.h
// png_read_info at pngread.c:92:1 in png.h
// png_set_interlace_handling at pngtrans.c:99:1 in png.h
// png_get_IHDR at pngget.c:825:1 in png.h
// png_get_IHDR at pngget.c:825:1 in png.h
// png_get_IHDR at pngget.c:825:1 in png.h
// png_get_palette_max at pngget.c:1239:1 in png.h
// png_read_update_info at pngread.c:267:1 in png.h
// png_get_rowbytes at pngget.c:30:1 in png.h
// png_get_image_height at pngget.c:61:1 in png.h
// png_read_row at pngread.c:383:1 in png.h
#include <cstdio>
#include <cstdint>
#include <cstddef>
#include <vector>
#include <png.h>
#include <setjmp.h>

static void WriteToDummyFile(const uint8_t *Data, size_t Size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    if (Size > 0) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }
}

static void ReadPngWithLibpng(const uint8_t *Data, size_t Size) {
  WriteToDummyFile(Data, Size);

  FILE *fp = fopen("./dummy_file", "rb");
  if (fp == nullptr) {
    return;
  }

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (png_ptr == nullptr) {
    fclose(fp);
    return;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr) {
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    fclose(fp);
    return;
  }

  png_infop end_info = png_create_info_struct(png_ptr);
  if (end_info == nullptr) {
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    fclose(fp);
    return;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    fclose(fp);
    return;
  }

  png_init_io(png_ptr, fp);
  png_set_sig_bytes(png_ptr, 0);

  png_set_check_for_invalid_index(png_ptr, (Size > 0 && (Data[0] & 1)) ? 1 : 0);
  png_set_expand(png_ptr);

#ifdef PNG_MAXIMUM_INFLATE_WINDOW
  (void)png_set_option(png_ptr, PNG_MAXIMUM_INFLATE_WINDOW,
                       (Size > 1) ? (Data[1] & 1) : 0);
#endif
#ifdef PNG_SKIP_sRGB_CHECK_PROFILE
  (void)png_set_option(png_ptr, PNG_SKIP_sRGB_CHECK_PROFILE,
                       (Size > 2) ? (Data[2] & 1) : 0);
#endif

  if (Size > 3) {
    int opt = static_cast<int>(Data[3] & 0x7e);
    (void)png_set_option(png_ptr, opt, (Size > 4) ? (Data[4] & 1) : 0);
  }

  png_read_info(png_ptr, info_ptr);

  (void)png_set_interlace_handling(png_ptr);

  png_uint_32 width = 0, height = 0;
  int bit_depth = 0, color_type = 0, interlace_method = 0;
  int compression_method = 0, filter_method = 0;

  (void)png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
                     &interlace_method, &compression_method, &filter_method);
  (void)png_get_IHDR(png_ptr, info_ptr, nullptr, &height, &bit_depth, nullptr,
                     &interlace_method, nullptr, &filter_method);
  (void)png_get_IHDR(png_ptr, info_ptr, &width, nullptr, nullptr, &color_type,
                     nullptr, &compression_method, nullptr);

  (void)png_get_palette_max(png_ptr, info_ptr);

  png_read_update_info(png_ptr, info_ptr);

  png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);
  png_uint_32 image_height = png_get_image_height(png_ptr, info_ptr);

  if (rowbytes > 0 && image_height > 0 && rowbytes < (1u << 20)) {
    std::vector<png_byte> row(rowbytes);
    png_uint_32 rows_to_read = image_height > 64 ? 64 : image_height;
    for (png_uint_32 y = 0; y < rows_to_read; ++y) {
      png_read_row(png_ptr, row.data(), nullptr);
    }
  }

  png_read_end(png_ptr, end_info);

  (void)png_get_palette_max(png_ptr, end_info);
  (void)png_get_IHDR(png_ptr, end_info, &width, &height, &bit_depth, &color_type,
                     &interlace_method, &compression_method, &filter_method);

  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
  fclose(fp);
}

extern "C" int LLVMFuzzerTestOneInput_53(const uint8_t *Data, size_t Size) {
  (void)png_set_option(nullptr, 0, 0);
  (void)png_get_IHDR(nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
                     nullptr, nullptr, nullptr);
  (void)png_get_palette_max(nullptr, nullptr);

  ReadPngWithLibpng(Data, Size);
  return 0;
}