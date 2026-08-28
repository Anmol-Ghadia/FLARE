// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_get_user_transform_ptr at pngtrans.c:833:1 in png.h
// png_get_user_chunk_ptr at pngget.c:1162:1 in png.h
// png_get_error_ptr at pngerror.c:856:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_get_error_ptr at pngerror.c:856:1 in png.h
// png_get_error_ptr at pngerror.c:856:1 in png.h
// png_get_user_chunk_ptr at pngget.c:1162:1 in png.h
// png_get_user_transform_ptr at pngtrans.c:833:1 in png.h
// png_get_error_ptr at pngerror.c:856:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_set_error_fn at pngerror.c:835:1 in png.h
// png_set_write_user_transform_fn at pngwrite.c:1322:1 in png.h
// png_get_error_ptr at pngerror.c:856:1 in png.h
// png_get_user_transform_ptr at pngtrans.c:833:1 in png.h
// png_get_user_chunk_ptr at pngget.c:1162:1 in png.h
// png_set_write_user_transform_fn at pngwrite.c:1322:1 in png.h
// png_get_user_transform_ptr at pngtrans.c:833:1 in png.h
// png_set_write_user_transform_fn at pngwrite.c:1322:1 in png.h
// png_set_error_fn at pngerror.c:835:1 in png.h
// png_get_error_ptr at pngerror.c:856:1 in png.h
// png_set_error_fn at pngerror.c:835:1 in png.h
// png_set_error_fn at pngerror.c:835:1 in png.h
// png_set_write_user_transform_fn at pngwrite.c:1322:1 in png.h
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
#include <stdio.h>
#include <string.h>
#include <setjmp.h>

struct FuzzCtx {
  uint8_t bytes[32];
};

static void FuzzErrorFn(png_structp png_ptr, png_const_charp) {
  jmp_buf *jb = png_get_error_ptr(png_ptr) ? static_cast<jmp_buf*>(png_get_error_ptr(png_ptr)) : nullptr;
  if (jb) longjmp(*jb, 1);
}

static void FuzzWarningFn(png_structp, png_const_charp) {
}

static void FuzzWriteTransform(png_structp png_ptr, png_row_infop row_info, png_bytep data) {
  (void)png_ptr;
  if (row_info == nullptr || data == nullptr) return;
  if (row_info->rowbytes > 0) {
    data[0] ^= static_cast<png_byte>(row_info->color_type ^ row_info->bit_depth);
  }
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    if (Data != nullptr && Size > 0) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  png_voidp null1 = png_get_user_chunk_ptr(nullptr);
  (void)null1;
  png_voidp null2 = png_get_user_transform_ptr(nullptr);
  (void)null2;
  png_voidp null3 = png_get_error_ptr(nullptr);
  (void)null3;

  png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  if (png_ptr == nullptr) {
    png_structp null_png = nullptr;
    png_infop null_info = nullptr;
    png_destroy_write_struct(&null_png, &null_info);
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);

  jmp_buf jb;
  if (setjmp(jb) == 0) {
    png_set_error_fn(png_ptr, &jb, FuzzErrorFn, FuzzWarningFn);

    FuzzCtx ctx;
    memset(&ctx, 0, sizeof(ctx));
    if (Data != nullptr && Size > 0) {
      size_t n = Size < sizeof(ctx.bytes) ? Size : sizeof(ctx.bytes);
      memcpy(ctx.bytes, Data, n);
    }

    png_set_write_user_transform_fn(png_ptr, FuzzWriteTransform);

    png_voidp err_ptr = png_get_error_ptr(png_ptr);
    (void)err_ptr;

    png_voidp transform_ptr_before = png_get_user_transform_ptr(png_ptr);
    (void)transform_ptr_before;

    png_voidp chunk_ptr_before = png_get_user_chunk_ptr(png_ptr);
    (void)chunk_ptr_before;

    if (Size > 0 && (Data[0] & 1) == 0) {
      png_set_write_user_transform_fn(png_ptr, nullptr);
      png_voidp transform_ptr_after_clear = png_get_user_transform_ptr(png_ptr);
      (void)transform_ptr_after_clear;
      png_set_write_user_transform_fn(png_ptr, FuzzWriteTransform);
    }

    if (Size > 1 && (Data[1] & 1) != 0) {
      png_set_error_fn(png_ptr, &ctx, FuzzErrorFn, FuzzWarningFn);
      png_voidp err_ptr2 = png_get_error_ptr(png_ptr);
      (void)err_ptr2;
      png_set_error_fn(png_ptr, &jb, FuzzErrorFn, FuzzWarningFn);
    }

    png_set_error_fn(nullptr, &ctx, FuzzErrorFn, FuzzWarningFn);
    png_set_write_user_transform_fn(nullptr, FuzzWriteTransform);

    png_voidp transform_ptr_final = png_get_user_transform_ptr(png_ptr);
    (void)transform_ptr_final;
    png_voidp chunk_ptr_final = png_get_user_chunk_ptr(png_ptr);
    (void)chunk_ptr_final;
    png_voidp err_ptr_final = png_get_error_ptr(png_ptr);
    (void)err_ptr_final;
  }

  png_destroy_write_struct(&png_ptr, &info_ptr);

  png_destroy_write_struct(&png_ptr, &info_ptr);

  return 0;
}