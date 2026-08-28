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
#include <vector>

static void PngReadCallback(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead) {
  png_voidp io_ptr = png_get_io_ptr(png_ptr);
  if (io_ptr == nullptr) {
    png_error(png_ptr, "no io_ptr");
    return;
  }

  struct BufferState {
    const uint8_t* data;
    size_t size;
    size_t offset;
  };

  BufferState* state = reinterpret_cast<BufferState*>(io_ptr);
  if (state->offset > state->size || byteCountToRead > state->size - state->offset) {
    png_error(png_ptr, "read overflow");
    return;
  }

  memcpy(outBytes, state->data + state->offset, byteCountToRead);
  state->offset += byteCountToRead;
}

static void ExerciseGetters(png_structp png_ptr, png_infop info_ptr) {
  if (png_ptr == nullptr || info_ptr == nullptr) return;

  int unit = 0;
  double width = 0.0, height = 0.0;
  png_charp swidth = nullptr;
  png_charp sheight = nullptr;

  png_charp purpose = nullptr;
  png_int_32 X0 = 0, X1 = 0;
  int type = 0, nparams = 0;
  png_charp units = nullptr;
  png_charpp params = nullptr;

  png_timep mod_time = nullptr;

  const png_uint_32 flags[] = {
      0,
      PNG_INFO_sCAL,
      PNG_INFO_pCAL,
      PNG_INFO_tIME,
      PNG_INFO_sCAL | PNG_INFO_pCAL,
      PNG_INFO_sCAL | PNG_INFO_tIME,
      PNG_INFO_pCAL | PNG_INFO_tIME,
      0xffffffffu
  };

  for (png_uint_32 flag : flags) {
    (void)png_get_valid(png_ptr, info_ptr, flag);
  }

#if defined(PNG_FLOATING_ARITHMETIC_SUPPORTED) || defined(PNG_FLOATING_POINT_SUPPORTED)
  (void)png_get_sCAL(png_ptr, info_ptr, &unit, &width, &height);
#endif

  (void)png_get_sCAL_s(png_ptr, info_ptr, &unit, &swidth, &sheight);
  (void)png_get_pCAL(png_ptr, info_ptr, &purpose, &X0, &X1, &type, &nparams, &units, &params);
  (void)png_get_tIME(png_ptr, info_ptr, &mod_time);

#if defined(PNG_FLOATING_ARITHMETIC_SUPPORTED) || defined(PNG_FLOATING_POINT_SUPPORTED)
  unit = 0;
  width = -1.0;
  height = -1.0;
  (void)png_get_sCAL(png_ptr, info_ptr, &unit, &width, &height);
#endif

  swidth = nullptr;
  sheight = nullptr;
  (void)png_get_sCAL_s(png_ptr, info_ptr, &unit, &swidth, &sheight);

  purpose = nullptr;
  units = nullptr;
  params = nullptr;
  X0 = 123;
  X1 = 456;
  type = 7;
  nparams = 9;
  (void)png_get_pCAL(png_ptr, info_ptr, &purpose, &X0, &X1, &type, &nparams, &units, &params);

  mod_time = nullptr;
  (void)png_get_tIME(png_ptr, info_ptr, &mod_time);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE* fp = fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    if (Size > 0) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (png_ptr == nullptr) {
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr) {
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_info_struct(png_ptr, &info_ptr);
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    return 0;
  }

  struct BufferState {
    const uint8_t* data;
    size_t size;
    size_t offset;
  } state = {Data, Size, 0};

  png_set_read_fn(png_ptr, &state, PngReadCallback);

  if (Size >= 8 && png_sig_cmp(const_cast<png_bytep>(reinterpret_cast<const png_bytep>(Data)), 0, 8) == 0) {
    png_set_sig_bytes(png_ptr, 0);
  }

  png_read_info(png_ptr, info_ptr);
  ExerciseGetters(png_ptr, info_ptr);

  if (setjmp(png_jmpbuf(png_ptr)) == 0) {
    png_read_update_info(png_ptr, info_ptr);
    ExerciseGetters(png_ptr, info_ptr);
  }

  png_destroy_info_struct(png_ptr, &info_ptr);
  png_destroy_read_struct(&png_ptr, nullptr, nullptr);
  return 0;
}