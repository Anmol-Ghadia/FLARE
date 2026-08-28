// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_sig_bytes at png.c:47:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_read_info at pngread.c:92:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_get_io_ptr at png.c:686:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_crc_action at pngrtran.c:36:1 in png.h
// png_set_option at png.c:4332:1 in png.h
// png_set_read_fn at pngrio.c:87:1 in png.h
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
#include <setjmp.h>
#include <stdio.h>
#include <string.h>

struct FuzzReadState {
  const uint8_t* data;
  size_t size;
  size_t offset;
};

static void FuzzReadCallback(png_structp png_ptr, png_bytep out_bytes,
                             png_size_t byte_count_to_read) {
  FuzzReadState* state =
      reinterpret_cast<FuzzReadState*>(png_get_io_ptr(png_ptr));
  if (state == NULL || out_bytes == NULL) {
    png_error(png_ptr, "invalid read state");
    return;
  }

  if (byte_count_to_read > state->size - state->offset) {
    size_t available = state->size - state->offset;
    if (available > 0) {
      memcpy(out_bytes, state->data + state->offset, available);
      state->offset += available;
    }
    memset(out_bytes + available, 0, byte_count_to_read - available);
    png_error(png_ptr, "read beyond end");
    return;
  }

  memcpy(out_bytes, state->data + state->offset, byte_count_to_read);
  state->offset += byte_count_to_read;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE* fp = fopen("./dummy_file", "wb");
  if (fp != NULL) {
    if (Size > 0 && Data != NULL) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (png_ptr == NULL) {
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == NULL) {
    png_destroy_read_struct(&png_ptr, NULL, NULL);
    return 0;
  }

  jmp_buf* jb = png_set_longjmp_fn(png_ptr, longjmp, sizeof(jmp_buf));
  if (jb == NULL) {
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    return 0;
  }

  if (setjmp(*jb)) {
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    return 0;
  }

  int crit_action = 0;
  int ancil_action = 0;
  int option = 0;
  int onoff = 0;
  int sig_bytes = 0;

  if (Size > 0) crit_action = static_cast<int>(Data[0]);
  if (Size > 1) ancil_action = static_cast<int>(Data[1]);
  if (Size > 2) option = static_cast<int>(Data[2] & 0xFE);
  if (Size > 3) onoff = (Data[3] & 1) ? 1 : 0;
  if (Size > 4) sig_bytes = static_cast<int>(Data[4] % 9);

  png_set_crc_action(png_ptr, crit_action, ancil_action);
  (void)png_set_option(png_ptr, option, onoff);

  FuzzReadState state;
  state.data = Data;
  state.size = Size;
  state.offset = 0;

  png_set_read_fn(png_ptr, &state, FuzzReadCallback);
  png_set_sig_bytes(png_ptr, sig_bytes);
  (void)png_set_longjmp_fn(png_ptr, longjmp, sizeof(jmp_buf));

  if (Size >= 8) {
    png_read_info(png_ptr, info_ptr);
  }

  png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
  return 0;
}