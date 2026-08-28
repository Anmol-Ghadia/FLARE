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
#include <vector>

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

static void ReadDataFromMemory(png_structp png_ptr, png_bytep outBytes,
                               png_size_t byteCountToRead) {
  struct MemoryReader {
    const uint8_t* data;
    size_t size;
    size_t offset;
  };

  MemoryReader* reader =
      static_cast<MemoryReader*>(png_get_io_ptr(png_ptr));
  if (reader == nullptr || reader->offset > reader->size ||
      byteCountToRead > reader->size - reader->offset) {
    png_error(png_ptr, "read error");
    return;
  }

  if (byteCountToRead > 0) {
    memcpy(outBytes, reader->data + reader->offset, byteCountToRead);
    reader->offset += byteCountToRead;
  }
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  {
    FILE* fp = fopen("./dummy_file", "wb");
    if (fp != nullptr) {
      if (Size > 0) {
        fwrite(Data, 1, Size, fp);
      }
      fclose(fp);
    }
  }

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn, PngWarningFn);
  if (png_ptr == nullptr) {
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr) {
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    return 0;
  }

  struct MemoryReader {
    const uint8_t* data;
    size_t size;
    size_t offset;
  } reader = {Data, Size, 0};

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    return 0;
  }

  png_set_read_fn(png_ptr, &reader, ReadDataFromMemory);

  if (Size >= 8) {
    png_set_sig_bytes(png_ptr, 0);
  }

  png_read_info(png_ptr, info_ptr);

  png_set_gray_to_rgb(png_ptr);

  png_read_update_info(png_ptr, info_ptr);

  size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);
  png_byte channels = png_get_channels(png_ptr, info_ptr);
  (void)rowbytes;
  (void)channels;

  png_uint_32 width = 0, height = 0;
  int bit_depth = 0, color_type = 0, interlace_type = 0, compression_type = 0, filter_method = 0;
  png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
               &interlace_type, &compression_type, &filter_method);

  if (height > 0 && rowbytes > 0 && rowbytes < (1u << 24) && height < (1u << 20)) {
    std::vector<png_bytep> rows(height, nullptr);
    std::vector<uint8_t> image;
    size_t total = 0;
    bool ok = true;

    for (png_uint_32 i = 0; i < height; ++i) {
      if (rowbytes > static_cast<size_t>(-1) - total) {
        ok = false;
        break;
      }
      total += rowbytes;
    }

    if (ok) {
      image.resize(total);
      for (png_uint_32 i = 0; i < height; ++i) {
        rows[i] = image.data() + static_cast<size_t>(i) * rowbytes;
      }
      png_read_image(png_ptr, rows.data());
      png_read_end(png_ptr, info_ptr);
    }
  }

  png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);

  png_structp png_ptr2 =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn, PngWarningFn);
  if (png_ptr2 == nullptr) {
    return 0;
  }

  png_infop info_ptr2 = png_create_info_struct(png_ptr2);
  if (info_ptr2 == nullptr) {
    png_destroy_read_struct(&png_ptr2, nullptr, nullptr);
    return 0;
  }

  MemoryReader reader2 = {Data, Size, 0};

  if (setjmp(png_jmpbuf(png_ptr2))) {
    png_destroy_read_struct(&png_ptr2, &info_ptr2, nullptr);
    return 0;
  }

  png_set_read_fn(png_ptr2, &reader2, ReadDataFromMemory);

  int transforms = 0;
  if (Size > 0) {
    transforms = static_cast<int>(Data[0]);
  }

  png_read_png(png_ptr2, info_ptr2, transforms, nullptr);

  (void)png_get_rowbytes(png_ptr2, info_ptr2);
  (void)png_get_channels(png_ptr2, info_ptr2);
  (void)png_get_rows(png_ptr2, info_ptr2);

  png_destroy_read_struct(&png_ptr2, &info_ptr2, nullptr);

  return 0;
}