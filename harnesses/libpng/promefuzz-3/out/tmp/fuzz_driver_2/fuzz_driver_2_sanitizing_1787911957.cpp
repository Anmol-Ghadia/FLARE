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
#include <csetjmp>
#include <cstdio>
#include <cstring>

struct FuzzReadState {
  const uint8_t* data;
  size_t size;
  size_t offset;
};

static void FuzzReadCallback(png_structp png_ptr, png_bytep out_bytes, png_size_t byte_count) {
  FuzzReadState* state =
      static_cast<FuzzReadState*>(png_get_io_ptr(png_ptr));
  if (state == nullptr || out_bytes == nullptr) {
    png_error(png_ptr, "invalid read state");
    return;
  }

  size_t remaining = (state->offset <= state->size) ? (state->size - state->offset) : 0;
  size_t to_copy = byte_count;
  if (to_copy > remaining) {
    if (remaining > 0) {
      memcpy(out_bytes, state->data + state->offset, remaining);
      state->offset += remaining;
    }
    memset(out_bytes + remaining, 0, byte_count - remaining);
    png_error(png_ptr, "short read");
    return;
  }

  memcpy(out_bytes, state->data + state->offset, to_copy);
  state->offset += to_copy;
}

static void FuzzLongjmp(png_structp png_ptr, int val) {
  jmp_buf* jb = png_get_longjmp_ptr(png_ptr);
  if (jb != nullptr) {
    longjmp(*jb, val != 0 ? val : 1);
  }
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

  jmp_buf* jb = png_set_longjmp_fn(png_ptr, FuzzLongjmp, sizeof(jmp_buf));
  if (jb == nullptr) {
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    return 0;
  }

  if (setjmp(*jb) != 0) {
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    return 0;
  }

  int crit_action_vals[] = {
      PNG_CRC_DEFAULT, PNG_CRC_ERROR_QUIT, PNG_CRC_WARN_USE, PNG_CRC_QUIET_USE
  };
  int ancil_action_vals[] = {
      PNG_CRC_DEFAULT, PNG_CRC_ERROR_QUIT, PNG_CRC_WARN_DISCARD,
      PNG_CRC_WARN_USE, PNG_CRC_QUIET_USE
  };

  int crit_action = crit_action_vals[(Size > 0 ? Data[0] : 0) % 4];
  int ancil_action = ancil_action_vals[(Size > 1 ? Data[1] : 0) % 5];

  png_set_crc_action(png_ptr, crit_action, ancil_action);

  int option = (Size > 2) ? static_cast<int>(Data[2]) : 0;
  option &= ~1;
  int onoff = (Size > 3) ? (Data[3] & 1) : 0;
  (void)png_set_option(png_ptr, option, onoff);

  FuzzReadState state = {Data, Size, 0};
  png_set_read_fn(png_ptr, &state, FuzzReadCallback);

  int sig_bytes = (Size > 4) ? static_cast<int>(Data[4] % 9) : 0;
  if (sig_bytes > static_cast<int>(Size)) {
    sig_bytes = static_cast<int>(Size);
  }
  state.offset = static_cast<size_t>(sig_bytes);
  png_set_sig_bytes(png_ptr, sig_bytes);

  (void)png_set_longjmp_fn(png_ptr, FuzzLongjmp, sizeof(jmp_buf));

  png_read_info(png_ptr, info_ptr);

  png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
  return 0;
}