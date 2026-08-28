// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_get_sPLT at pngget.c:764:1 in png.h
// png_get_text at pngget.c:1049:1 in png.h
// png_get_tRNS at pngget.c:1093:1 in png.h
// png_free_data at png.c:471:1 in png.h
// png_free_data at png.c:471:1 in png.h
// png_free_data at png.c:471:1 in png.h
// png_free_data at png.c:471:1 in png.h
// png_free_data at png.c:471:1 in png.h
// png_free_data at png.c:471:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_init_io at png.c:703:1 in png.h
// png_set_sig_bytes at png.c:47:1 in png.h
// png_read_info at pngread.c:92:1 in png.h
// png_get_unknown_chunks at pngget.c:1139:1 in png.h
// png_get_sPLT at pngget.c:764:1 in png.h
// png_get_tRNS at pngget.c:1093:1 in png.h
// png_get_text at pngget.c:1049:1 in png.h
// png_get_unknown_chunks at pngget.c:1139:1 in png.h
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

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarnFn(png_structp, png_const_charp) {
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE *fp = fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    if (Size > 0) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  png_structp png_ptr =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, PngErrorFn, PngWarnFn);
  if (png_ptr == nullptr) {
    return 0;
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == nullptr) {
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    return 0;
  }

  fp = fopen("./dummy_file", "rb");
  if (fp != nullptr) {
    png_init_io(png_ptr, fp);

    if (Size >= 8) {
      png_set_sig_bytes(png_ptr, 0);
    }

    png_read_info(png_ptr, info_ptr);

    png_unknown_chunkp unknowns = nullptr;
    int unknown_count = png_get_unknown_chunks(png_ptr, info_ptr, &unknowns);
    if (unknown_count > 0 && unknowns != nullptr) {
      volatile png_byte b = unknowns[0].name[0];
      (void)b;
    }

    png_sPLT_tp splt_entries = nullptr;
    int splt_count = png_get_sPLT(png_ptr, info_ptr, &splt_entries);
    if (splt_count > 0 && splt_entries != nullptr) {
      volatile png_byte d = splt_entries[0].depth;
      (void)d;
    }

    png_bytep trans_alpha = nullptr;
    int num_trans = 0;
    png_color_16p trans_color = nullptr;
    png_uint_32 trns_flags =
        png_get_tRNS(png_ptr, info_ptr, &trans_alpha, &num_trans, &trans_color);
    if (trns_flags != 0) {
      if (trans_alpha != nullptr && num_trans > 0) {
        volatile png_byte a = trans_alpha[0];
        (void)a;
      }
      if (trans_color != nullptr) {
        volatile png_uint_16 r = trans_color->red;
        (void)r;
      }
    }

    png_textp text_ptr = nullptr;
    int num_text = 0;
    int text_count = png_get_text(png_ptr, info_ptr, &text_ptr, &num_text);
    if (text_count > 0 && text_ptr != nullptr && num_text > 0) {
      volatile int c = text_ptr[0].compression;
      (void)c;
    }

    png_unknown_chunkp unknowns2 = nullptr;
    (void)png_get_unknown_chunks(png_ptr, info_ptr, &unknowns2);

    png_sPLT_tp splt_entries2 = nullptr;
    (void)png_get_sPLT(png_ptr, info_ptr, &splt_entries2);

    png_textp text_ptr2 = nullptr;
    (void)png_get_text(png_ptr, info_ptr, &text_ptr2, nullptr);

    (void)png_get_tRNS(png_ptr, info_ptr, nullptr, nullptr, nullptr);

    png_free_data(png_ptr, info_ptr, PNG_FREE_TEXT, -1);
#ifdef PNG_FREE_TRNS
    png_free_data(png_ptr, info_ptr, PNG_FREE_TRNS, -1);
#endif
#ifdef PNG_FREE_UNKN
    png_free_data(png_ptr, info_ptr, PNG_FREE_UNKN, -1);
#endif
#ifdef PNG_FREE_SPLT
    png_free_data(png_ptr, info_ptr, PNG_FREE_SPLT, -1);
#endif
    png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, 0);
    png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);

    fclose(fp);
  }

  png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
  return 0;
}