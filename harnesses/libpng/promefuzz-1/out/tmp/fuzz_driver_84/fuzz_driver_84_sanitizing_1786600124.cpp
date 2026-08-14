#include <cstdio>
#include <cstdint>
#include <cstddef>
#include <vector>
#include <cstring>
#include <png.h>

struct BufferState {
  const uint8_t* data;
  size_t size;
  size_t offset;
};

static void PngReadCallback(png_structp png_ptr, png_bytep outBytes,
                            png_size_t byteCountToRead) {
  BufferState* state =
      reinterpret_cast<BufferState*>(png_get_io_ptr(png_ptr));
  if (state == nullptr) {
    png_error(png_ptr, "No IO pointer");
    return;
  }

  if (state->offset > state->size ||
      byteCountToRead > state->size - state->offset) {
    png_error(png_ptr, "Read beyond end");
    return;
  }

  if (byteCountToRead > 0) {
    memcpy(outBytes, state->data + state->offset, byteCountToRead);
    state->offset += byteCountToRead;
  }
}

static void PngErrorCallback(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningCallback(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE* fp = fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    if (Size > 0) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  png_set_invert_mono(nullptr);
  png_set_gray_to_rgb(nullptr);
  png_read_update_info(nullptr, nullptr);
  png_set_packswap(nullptr);
  png_set_bgr(nullptr);
  png_set_packing(nullptr);

  BufferState state = {Data, Size, 0};

  png_structp png_ptr = png_create_read_struct(
      PNG_LIBPNG_VER_STRING, nullptr, PngErrorCallback, PngWarningCallback);
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

  png_set_read_fn(png_ptr, &state, PngReadCallback);

  if (Size >= 8 && png_sig_cmp(const_cast<png_bytep>(Data), 0, 8) == 0) {
    png_set_sig_bytes(png_ptr, 0);
  }

  png_read_info(png_ptr, info_ptr);

  uint8_t mode = Size > 0 ? Data[0] : 0x1f;

  if (mode & 0x01) png_set_invert_mono(png_ptr);
  if (mode & 0x02) png_set_gray_to_rgb(png_ptr);
  if (mode & 0x04) png_set_packswap(png_ptr);
  if (mode & 0x08) png_set_bgr(png_ptr);
  if (mode & 0x10) png_set_packing(png_ptr);

  png_read_update_info(png_ptr, info_ptr);

  if (mode & 0x20) {
    png_set_invert_mono(png_ptr);
    png_set_gray_to_rgb(png_ptr);
    png_set_packswap(png_ptr);
    png_set_bgr(png_ptr);
    png_set_packing(png_ptr);
  }

  if (mode & 0x40) {
    png_read_update_info(png_ptr, info_ptr);
  }

  png_uint_32 height = png_get_image_height(png_ptr, info_ptr);
  png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);

  if (height > 0 && rowbytes > 0 && height < 100000 && rowbytes < 1000000 &&
      rowbytes <= static_cast<png_size_t>(-1) / height) {
    std::vector<png_byte> image(rowbytes * height);
    std::vector<png_bytep> rows(height);
    for (png_uint_32 y = 0; y < height; ++y) {
      rows[y] = image.data() + y * rowbytes;
    }
    png_read_image(png_ptr, rows.data());
  }

  png_read_end(png_ptr, end_info);
  png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
  return 0;
}