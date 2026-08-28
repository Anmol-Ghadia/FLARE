// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
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
// png_set_sig_bytes at png.c:47:1 in png.h
#include <png.h>
#include <cstdint>
#include <cstddef>
#include <csetjmp>
#include <cstdio>
#include <cstring>
#include <cstdlib>

struct FuzzReadState {
  const uint8_t* data;
  size_t size;
  size_t offset;
};

static void FuzzReadCallback(png_structp png_ptr, png_bytep out_bytes, png_size_t byte_count) {
  FuzzReadState* state = static_cast<FuzzReadState*>(png_get_io_ptr(png_ptr));
  if (state == nullptr || out_bytes == nullptr) {
    png_error(png_ptr, "invalid read state");
    return;
  }

  size_t remaining = state->offset <= state->size ? (state->size - state->offset) : 0;
  size_t to_copy = static_cast<size_t>(byte_count);

  if (to_copy > remaining) {
    if (remaining > 0) {
      memcpy(out_bytes, state->data + state->offset, remaining);
      state->offset += remaining;
    }
    memset(out_bytes + remaining, 0, to_copy - remaining);
    png_error(png_ptr, "short read");
    return;
  }

  memcpy(out_bytes, state->data + state->offset, to_copy);
  state->offset += to_copy;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE* fp = fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    if (Size > 0) {
      fwrite(Data, 1, Size, fp);
    }
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

  jmp_buf* jb = png_set_longjmp_fn(png_ptr, longjmp, sizeof(jmp_buf));
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

  FuzzReadState state;
  state.data = Data;
  state.size = Size;
  state.offset = 0;
  png_set_read_fn(png_ptr, &state, FuzzReadCallback);

  int sig_bytes = (Size > 4) ? static_cast<int>(Data[4] % 9) : 0;
  if (sig_bytes > static_cast<int>(Size)) {
    sig_bytes = static_cast<int>(Size);
  }
  state.offset = static_cast<size_t>(sig_bytes);
  png_set_sig_bytes(png_ptr, sig_bytes);

  (void)png_set_longjmp_fn(png_ptr, longjmp, sizeof(jmp_buf));

  png_read_info(png_ptr, info_ptr);

  png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
  return 0;
}