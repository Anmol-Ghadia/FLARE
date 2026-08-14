// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_progressive_read_fn at pngpread.c:1074:1 in png.h
// png_set_read_status_fn at pngread.c:1037:1 in png.h
// png_set_read_status_fn at pngread.c:1037:1 in png.h
// png_process_data at pngpread.c:36:1 in png.h
// png_process_data at pngpread.c:36:1 in png.h
// png_read_update_info at pngread.c:267:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_read_status_fn at pngread.c:1037:1 in png.h
// png_set_progressive_read_fn at pngpread.c:1074:1 in png.h
// png_read_update_info at pngread.c:267:1 in png.h
// png_set_IHDR at pngset.c:254:1 in png.h
// png_read_update_info at pngread.c:267:1 in png.h
// png_get_rowbytes at pngget.c:30:1 in png.h
// png_read_row at pngread.c:383:1 in png.h
// png_read_row at pngread.c:383:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_get_error_ptr at pngerror.c:856:1 in png.h
// png_get_progressive_ptr at pngpread.c:1089:1 in png.h
// png_read_update_info at pngread.c:267:1 in png.h
// png_read_update_info at pngread.c:267:1 in png.h
// png_process_data_pause at pngpread.c:51:1 in png.h
// png_get_progressive_ptr at pngpread.c:1089:1 in png.h
// png_process_data_pause at pngpread.c:51:1 in png.h
// png_read_update_info at pngread.c:267:1 in png.h
// png_process_data at pngpread.c:36:1 in png.h
// png_set_progressive_read_fn at pngpread.c:1074:1 in png.h
// png_set_read_status_fn at pngread.c:1037:1 in png.h
// png_read_row at pngread.c:383:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
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
#include <vector>
#include <fstream>
#include <algorithm>

struct FuzzProgressiveState {
  bool pause_in_info;
  bool pause_in_row;
  bool save_on_pause;
  bool update_info_called;
  bool duplicate_update;
  bool read_status_set;
  size_t last_pause_result;
};

static void FuzzPngError(png_structp png_ptr, png_const_charp) {
  jmp_buf *jb = reinterpret_cast<jmp_buf*>(png_get_error_ptr(png_ptr));
  if (jb != nullptr) longjmp(*jb, 1);
}

static void FuzzPngWarning(png_structp, png_const_charp) {
}

static void ReadStatusCallback(png_structp, png_uint_32, int) {
}

static void ProgressiveInfoCallback(png_structp png_ptr, png_infop info_ptr) {
  if (png_ptr == nullptr) return;
  FuzzProgressiveState* st =
      reinterpret_cast<FuzzProgressiveState*>(png_get_progressive_ptr(png_ptr));
  if (st == nullptr) return;

  if (!st->update_info_called) {
    png_read_update_info(png_ptr, info_ptr);
    st->update_info_called = true;
    if (st->duplicate_update) {
      png_read_update_info(png_ptr, info_ptr);
    }
  }

  if (st->pause_in_info) {
    st->last_pause_result = png_process_data_pause(png_ptr, st->save_on_pause ? 1 : 0);
  }
}

static void ProgressiveRowCallback(png_structp png_ptr, png_bytep, png_uint_32, int) {
  if (png_ptr == nullptr) return;
  FuzzProgressiveState* st =
      reinterpret_cast<FuzzProgressiveState*>(png_get_progressive_ptr(png_ptr));
  if (st == nullptr) return;

  if (st->pause_in_row) {
    st->last_pause_result = png_process_data_pause(png_ptr, st->save_on_pause ? 1 : 0);
  }
}

static void ProgressiveEndCallback(png_structp, png_infop) {
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  if (Data == nullptr) return 0;

  {
    std::ofstream ofs("./dummy_file", std::ios::binary);
    if (ofs) ofs.write(reinterpret_cast<const char*>(Data), static_cast<std::streamsize>(Size));
  }

  if (Size == 0) {
    png_read_update_info(nullptr, nullptr);
    png_process_data(nullptr, nullptr, nullptr, 0);
    png_set_progressive_read_fn(nullptr, nullptr, nullptr, nullptr, nullptr);
    png_set_read_status_fn(nullptr, nullptr);
    png_read_row(nullptr, nullptr, nullptr);
    return 0;
  }

  jmp_buf jmp_progressive;
  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                               &jmp_progressive,
                                               FuzzPngError,
                                               FuzzPngWarning);
  if (png_ptr == nullptr) return 0;

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr) {
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    return 0;
  }

  if (setjmp(jmp_progressive) == 0) {
    FuzzProgressiveState st{};
    if (Size > 0) {
      st.pause_in_info = (Data[0] & 0x01) != 0;
      st.pause_in_row = (Data[0] & 0x02) != 0;
      st.save_on_pause = (Data[0] & 0x04) != 0;
      st.duplicate_update = (Data[0] & 0x08) != 0;
      st.read_status_set = (Data[0] & 0x10) != 0;
    }

    png_set_progressive_read_fn(
        png_ptr, &st,
        ProgressiveInfoCallback,
        ProgressiveRowCallback,
        ProgressiveEndCallback);

    if (st.read_status_set) {
      png_set_read_status_fn(png_ptr, ReadStatusCallback);
    } else {
      png_set_read_status_fn(png_ptr, nullptr);
    }

    size_t offset = 0;
    size_t control = Size > 1 ? Data[1] : 0;
    while (offset < Size) {
      size_t remaining = Size - offset;
      size_t chunk = 1 + ((control + offset) % remaining);
      png_process_data(png_ptr, info_ptr,
                       const_cast<png_bytep>(reinterpret_cast<const png_byte*>(Data + offset)),
                       chunk);
      offset += chunk;
    }

    png_process_data(png_ptr, info_ptr, nullptr, 0);

    if ((Size > 2) && (Data[2] & 1)) {
      png_read_update_info(png_ptr, info_ptr);
    }
  }

  png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);

  jmp_buf jmp_sequential;
  png_structp png_ptr2 = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                                &jmp_sequential,
                                                FuzzPngError,
                                                FuzzPngWarning);
  if (png_ptr2 == nullptr) return 0;

  png_infop info_ptr2 = png_create_info_struct(png_ptr2);
  if (info_ptr2 == nullptr) {
    png_destroy_read_struct(&png_ptr2, nullptr, nullptr);
    return 0;
  }

  if (setjmp(jmp_sequential) == 0) {
    png_set_read_status_fn(png_ptr2, ReadStatusCallback);
    png_set_progressive_read_fn(png_ptr2, nullptr, nullptr, nullptr, nullptr);
    png_read_update_info(png_ptr2, info_ptr2);

    png_uint_32 width = 0;
    png_size_t rowbytes = 0;
    if (Size >= 9) {
      width = static_cast<png_uint_32>(Data[3]) |
              (static_cast<png_uint_32>(Data[4]) << 8) |
              (static_cast<png_uint_32>(Data[5]) << 16) |
              (static_cast<png_uint_32>(Data[6]) << 24);
      rowbytes = static_cast<png_size_t>(Data[7]) + 1;
    } else {
      width = 1;
      rowbytes = 8;
    }

    png_set_IHDR(png_ptr2, info_ptr2,
                 width == 0 ? 1 : width,
                 1,
                 8,
                 PNG_COLOR_TYPE_RGBA,
                 PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE,
                 PNG_FILTER_TYPE_BASE);

    png_read_update_info(png_ptr2, info_ptr2);

    rowbytes = png_get_rowbytes(png_ptr2, info_ptr2);
    if (rowbytes == 0) rowbytes = 1;
    rowbytes = std::min<png_size_t>(rowbytes, 1 << 20);

    std::vector<png_byte> row(rowbytes);
    std::vector<png_byte> display(rowbytes);

    if (Size > 8) {
      size_t copy = std::min<size_t>(row.size(), Size - 8);
      std::copy(Data + 8, Data + 8 + copy, row.begin());
      std::copy(row.begin(), row.end(), display.begin());
    }

    png_read_row(png_ptr2, row.data(), display.data());
    png_read_row(png_ptr2, row.data(), nullptr);
  }

  png_destroy_read_struct(&png_ptr2, &info_ptr2, nullptr);
  return 0;
}