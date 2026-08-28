// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_interlace_handling at pngtrans.c:99:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_get_current_pass_number at pngtrans.c:856:1 in png.h
// png_read_png at pngread.c:1049:1 in png.h
// png_get_io_state at pngget.c:1224:1 in png.h
// png_get_current_pass_number at pngtrans.c:856:1 in png.h
// png_get_io_chunk_type at pngget.c:1230:1 in png.h
// png_read_info at pngread.c:92:1 in png.h
// png_get_io_state at pngget.c:1224:1 in png.h
// png_get_io_chunk_type at pngget.c:1230:1 in png.h
// png_set_interlace_handling at pngtrans.c:99:1 in png.h
// png_get_current_pass_number at pngtrans.c:856:1 in png.h
// png_read_update_info at pngread.c:267:1 in png.h
// png_get_image_height at pngget.c:61:1 in png.h
// png_get_rowbytes at pngget.c:30:1 in png.h
// png_get_current_pass_number at pngtrans.c:856:1 in png.h
// png_read_rows at pngread.c:645:1 in png.h
// png_get_io_state at pngget.c:1224:1 in png.h
// png_get_io_chunk_type at pngget.c:1230:1 in png.h
// png_read_rows at pngread.c:645:1 in png.h
// png_get_io_state at pngget.c:1224:1 in png.h
// png_get_current_pass_number at pngtrans.c:856:1 in png.h
// png_get_io_chunk_type at pngget.c:1230:1 in png.h
// png_read_end at pngread.c:766:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_get_io_chunk_type at pngget.c:1230:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_init_io at png.c:703:1 in png.h
// png_set_sig_bytes at png.c:47:1 in png.h
// png_get_io_state at pngget.c:1224:1 in png.h
// png_get_current_pass_number at pngtrans.c:856:1 in png.h
// png_get_io_chunk_type at pngget.c:1230:1 in png.h
#include <png.h>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <vector>

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

static void DestroyReadStruct(png_structp* png_ptr_ptr, png_infop* info_ptr_ptr) {
  png_infop end_info = nullptr;
  png_destroy_read_struct(png_ptr_ptr, info_ptr_ptr, &end_info);
}

extern "C" int LLVMFuzzerTestOneInput_26(const uint8_t *Data, size_t Size) {
  FILE *fp = std::fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    if (Size > 0) {
      std::fwrite(Data, 1, Size, fp);
    }
    std::fclose(fp);
  }

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn, PngWarningFn);
  if (png_ptr == nullptr) {
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr) {
    DestroyReadStruct(&png_ptr, nullptr);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    if (fp != nullptr) {
      std::fclose(fp);
    }
    DestroyReadStruct(&png_ptr, &info_ptr);
    return 0;
  }

  fp = std::fopen("./dummy_file", "rb");
  if (fp == nullptr) {
    DestroyReadStruct(&png_ptr, &info_ptr);
    return 0;
  }

  png_init_io(png_ptr, fp);
  png_set_sig_bytes(png_ptr, 0);

  (void)png_get_io_state(png_ptr);
  (void)png_get_current_pass_number(png_ptr);
  (void)png_get_io_chunk_type(png_ptr);

  int transforms = 0;
#ifdef PNG_TRANSFORM_STRIP_16
  if (Size > 0 && (Data[0] & 1)) transforms |= PNG_TRANSFORM_STRIP_16;
#endif
#ifdef PNG_TRANSFORM_STRIP_ALPHA
  if (Size > 1 && (Data[1] & 1)) transforms |= PNG_TRANSFORM_STRIP_ALPHA;
#endif
#ifdef PNG_TRANSFORM_PACKING
  if (Size > 2 && (Data[2] & 1)) transforms |= PNG_TRANSFORM_PACKING;
#endif
#ifdef PNG_TRANSFORM_PACKSWAP
  if (Size > 3 && (Data[3] & 1)) transforms |= PNG_TRANSFORM_PACKSWAP;
#endif
#ifdef PNG_TRANSFORM_EXPAND
  if (Size > 4 && (Data[4] & 1)) transforms |= PNG_TRANSFORM_EXPAND;
#endif
#ifdef PNG_TRANSFORM_INVERT_MONO
  if (Size > 5 && (Data[5] & 1)) transforms |= PNG_TRANSFORM_INVERT_MONO;
#endif
#ifdef PNG_TRANSFORM_SHIFT
  if (Size > 6 && (Data[6] & 1)) transforms |= PNG_TRANSFORM_SHIFT;
#endif
#ifdef PNG_TRANSFORM_BGR
  if (Size > 7 && (Data[7] & 1)) transforms |= PNG_TRANSFORM_BGR;
#endif
#ifdef PNG_TRANSFORM_SWAP_ALPHA
  if (Size > 8 && (Data[8] & 1)) transforms |= PNG_TRANSFORM_SWAP_ALPHA;
#endif
#ifdef PNG_TRANSFORM_SWAP_ENDIAN
  if (Size > 9 && (Data[9] & 1)) transforms |= PNG_TRANSFORM_SWAP_ENDIAN;
#endif
#ifdef PNG_TRANSFORM_INVERT_ALPHA
  if (Size > 10 && (Data[10] & 1)) transforms |= PNG_TRANSFORM_INVERT_ALPHA;
#endif
#ifdef PNG_TRANSFORM_GRAY_TO_RGB
  if (Size > 11 && (Data[11] & 1)) transforms |= PNG_TRANSFORM_GRAY_TO_RGB;
#endif
#ifdef PNG_TRANSFORM_EXPAND_16
  if (Size > 12 && (Data[12] & 1)) transforms |= PNG_TRANSFORM_EXPAND_16;
#endif
#ifdef PNG_TRANSFORM_SCALE_16
  if (Size > 13 && (Data[13] & 1)) transforms |= PNG_TRANSFORM_SCALE_16;
#endif

  if (Size > 1 && (Data[1] & 0x80)) {
    if (Size > 0 && (Data[0] & 0x80)) {
      (void)png_set_interlace_handling(png_ptr);
      (void)png_get_current_pass_number(png_ptr);
    }
    png_read_png(png_ptr, info_ptr, transforms, nullptr);
    (void)png_get_io_state(png_ptr);
    (void)png_get_current_pass_number(png_ptr);
    (void)png_get_io_chunk_type(png_ptr);
  } else {
    png_read_info(png_ptr, info_ptr);
    (void)png_get_io_state(png_ptr);
    (void)png_get_io_chunk_type(png_ptr);

    int passes = 1;
    if (Size > 2 && (Data[2] & 0x80)) {
      passes = png_set_interlace_handling(png_ptr);
      (void)png_get_current_pass_number(png_ptr);
    }

    png_read_update_info(png_ptr, info_ptr);

    png_uint_32 height = png_get_image_height(png_ptr, info_ptr);
    png_size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);
    if (rowbytes == 0) {
      rowbytes = 1;
    }

    const png_uint_32 max_rows = 32;
    png_uint_32 rows_to_buffer = std::min(height, max_rows);

    std::vector<std::vector<png_byte> > row_storage(rows_to_buffer);
    std::vector<std::vector<png_byte> > display_storage(rows_to_buffer);
    std::vector<png_bytep> row_ptrs(rows_to_buffer, nullptr);
    std::vector<png_bytep> display_ptrs(rows_to_buffer, nullptr);

    for (png_uint_32 i = 0; i < rows_to_buffer; ++i) {
      row_storage[i].resize(rowbytes);
      row_ptrs[i] = row_storage[i].data();
      if (Size > 3 && (Data[3] & 1)) {
        display_storage[i].resize(rowbytes);
        display_ptrs[i] = display_storage[i].data();
      }
    }

    for (int pass = 0; pass < std::max(1, passes); ++pass) {
      (void)png_get_current_pass_number(png_ptr);

      png_uint_32 consumed = 0;
      while (consumed < rows_to_buffer) {
        png_uint_32 chunk = 1;
        if (Size > 4) {
          chunk = 1 + (Data[(4 + consumed) % Size] % 4);
        }
        if (chunk > rows_to_buffer - consumed) {
          chunk = rows_to_buffer - consumed;
        }

        png_bytepp rows = &row_ptrs[consumed];
        png_bytepp displays =
            (!display_ptrs.empty() && display_ptrs[0] != nullptr) ? &display_ptrs[consumed]
                                                                  : nullptr;

        png_read_rows(png_ptr, rows, displays, chunk);
        (void)png_get_io_state(png_ptr);
        (void)png_get_io_chunk_type(png_ptr);
        consumed += chunk;
      }

      for (png_uint_32 y = rows_to_buffer; y < height; ++y) {
        std::vector<png_byte> single_row(rowbytes);
        png_bytep single_row_ptr = single_row.data();
        png_read_rows(png_ptr, &single_row_ptr, nullptr, 1);
        if ((y & 7u) == 0) {
          (void)png_get_io_state(png_ptr);
          (void)png_get_current_pass_number(png_ptr);
          (void)png_get_io_chunk_type(png_ptr);
        }
      }
    }

    if (Size > 5 && (Data[5] & 1)) {
      png_read_end(png_ptr, info_ptr);
      (void)png_get_io_chunk_type(png_ptr);
    }
  }

  std::fclose(fp);
  DestroyReadStruct(&png_ptr, &info_ptr);
  return 0;
}