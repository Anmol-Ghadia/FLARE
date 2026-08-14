// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_get_valid at pngget.c:20:1 in png.h
// png_get_valid at pngget.c:20:1 in png.h
// png_get_valid at pngget.c:20:1 in png.h
// png_get_valid at pngget.c:20:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_read_fn at pngrio.c:87:1 in png.h
// png_read_info at pngread.c:92:1 in png.h
// png_get_IHDR at pngget.c:825:1 in png.h
// png_get_IHDR at pngget.c:825:1 in png.h
// png_get_IHDR at pngget.c:825:1 in png.h
// png_get_IHDR at pngget.c:825:1 in png.h
// png_get_tRNS at pngget.c:1093:1 in png.h
// png_get_tRNS at pngget.c:1093:1 in png.h
// png_get_bKGD at pngget.c:488:1 in png.h
// png_get_bKGD at pngget.c:488:1 in png.h
// png_get_sBIT at pngget.c:1031:1 in png.h
// png_get_sBIT at pngget.c:1031:1 in png.h
// png_get_valid at pngget.c:20:1 in png.h
// png_get_valid at pngget.c:20:1 in png.h
#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <png.h>

struct FuzzReader {
  const uint8_t* data;
  size_t size;
  size_t offset;
};

static void PngReadCallback(png_structp png_ptr, png_bytep outBytes,
                            png_size_t byteCountToRead) {
  FuzzReader* reader =
      reinterpret_cast<FuzzReader*>(png_get_io_ptr(png_ptr));
  if (reader == nullptr || outBytes == nullptr) {
    png_error(png_ptr, "invalid read state");
    return;
  }

  if (byteCountToRead > reader->size - reader->offset) {
    png_error(png_ptr, "read beyond end");
    return;
  }

  memcpy(outBytes, reader->data + reader->offset, byteCountToRead);
  reader->offset += byteCountToRead;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE* fp = fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    if (Size > 0) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (png_ptr == nullptr) {
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr) {
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    return 0;
  }

  png_infop end_info = png_create_info_struct(png_ptr);
  if (end_info == nullptr) {
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    return 0;
  }

  FuzzReader reader = {Data, Size, 0};
  png_set_read_fn(png_ptr, &reader, PngReadCallback);

  png_read_info(png_ptr, info_ptr);

  png_uint_32 width = 0, height = 0;
  int bit_depth = 0, color_type = 0, interlace_method = 0;
  int compression_method = 0, filter_method = 0;
  (void)png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth,
                     &color_type, &interlace_method, &compression_method,
                     &filter_method);
  (void)png_get_IHDR(png_ptr, info_ptr, nullptr, &height, nullptr,
                     &color_type, nullptr, &compression_method, nullptr);
  (void)png_get_IHDR(nullptr, info_ptr, &width, &height, &bit_depth,
                     &color_type, &interlace_method, &compression_method,
                     &filter_method);
  (void)png_get_IHDR(png_ptr, nullptr, &width, &height, &bit_depth,
                     &color_type, &interlace_method, &compression_method,
                     &filter_method);

  png_bytep trans_alpha = nullptr;
  int num_trans = 0;
  png_color_16p trans_color = nullptr;
  (void)png_get_tRNS(png_ptr, info_ptr, &trans_alpha, &num_trans, &trans_color);
  (void)png_get_tRNS(png_ptr, info_ptr, nullptr, &num_trans, nullptr);

  png_color_16p background = nullptr;
  (void)png_get_bKGD(png_ptr, info_ptr, &background);
  (void)png_get_bKGD(png_ptr, info_ptr, nullptr);

  png_color_8p sig_bit = nullptr;
  (void)png_get_sBIT(png_ptr, info_ptr, &sig_bit);
  (void)png_get_sBIT(png_ptr, info_ptr, nullptr);

  (void)png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS);
  (void)png_get_valid(png_ptr, info_ptr, PNG_INFO_bKGD);
  (void)png_get_valid(png_ptr, info_ptr, PNG_INFO_sBIT);
  (void)png_get_valid(png_ptr, info_ptr, PNG_INFO_PLTE);
  (void)png_get_valid(nullptr, info_ptr, PNG_INFO_tRNS);
  (void)png_get_valid(png_ptr, nullptr, PNG_INFO_tRNS);

  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
  return 0;
}