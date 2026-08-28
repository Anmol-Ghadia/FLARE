// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_error_fn at pngerror.c:835:1 in png.h
// png_init_io at png.c:703:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_init_io at png.c:703:1 in png.h
// png_write_end at pngwrite.c:358:1 in png.h
// png_set_error_fn at pngerror.c:835:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_error_fn at pngerror.c:835:1 in png.h
// png_init_io at png.c:703:1 in png.h
// png_write_end at pngwrite.c:358:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_create_write_struct_2 at pngwrite.c:505:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_error_fn at pngerror.c:835:1 in png.h
// png_set_error_fn at pngerror.c:835:1 in png.h
// png_init_io at png.c:703:1 in png.h
// png_write_end at pngwrite.c:358:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_create_read_struct_2 at pngread.c:40:1 in png.h
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
#include <cstdio>
#include <cstdint>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <setjmp.h>

static void fuzz_png_error(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void fuzz_png_warn(png_structp, png_const_charp) {
}

static png_voidp fuzz_malloc(png_structp, png_alloc_size_t size) {
  if (size == 0) return nullptr;
  return std::malloc(size);
}

static void fuzz_free(png_structp, png_voidp ptr) {
  std::free(ptr);
}

static void write_dummy_file(const uint8_t *Data, size_t Size) {
  FILE *fp = std::fopen("./dummy_file", "wb");
  if (!fp) return;
  if (Data && Size) {
    std::fwrite(Data, 1, Size, fp);
  }
  std::fclose(fp);
}

static void exercise_write_struct_basic(const uint8_t *Data, size_t Size) {
  png_structp png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, fuzz_png_error, fuzz_png_warn);
  if (!png_ptr) return;

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_write_struct(&png_ptr, nullptr);
    return;
  }

  png_set_error_fn(png_ptr, const_cast<uint8_t*>(Data), fuzz_png_error, fuzz_png_warn);

  FILE *fp = std::fopen("./dummy_file", "wb");
  if (fp) {
    png_init_io(png_ptr, fp);
    png_write_end(png_ptr, nullptr);
    std::fclose(fp);
  }

  png_destroy_write_struct(&png_ptr, nullptr);
}

static void exercise_write_struct_custom(const uint8_t *Data, size_t Size) {
  png_voidp error_ctx = Size ? (png_voidp)(Data) : nullptr;
  png_voidp mem_ctx = Size > 1 ? (png_voidp)(Data + 1) : nullptr;

  png_structp png_ptr = png_create_write_struct_2(
      PNG_LIBPNG_VER_STRING, error_ctx, fuzz_png_error, fuzz_png_warn,
      mem_ctx, fuzz_malloc, fuzz_free);
  if (!png_ptr) return;

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_write_struct(&png_ptr, nullptr);
    return;
  }

  if (Size & 1) {
    png_set_error_fn(png_ptr, mem_ctx, fuzz_png_error, fuzz_png_warn);
  } else {
    png_set_error_fn(png_ptr, error_ctx, fuzz_png_error, nullptr);
  }

  FILE *fp = std::fopen("./dummy_file", "ab");
  if (fp) {
    png_init_io(png_ptr, fp);
    png_write_end(png_ptr, nullptr);
    std::fclose(fp);
  }

  png_destroy_write_struct(&png_ptr, nullptr);
}

static void exercise_read_struct_custom(const uint8_t *Data, size_t Size) {
  png_voidp error_ctx = Size ? (png_voidp)(Data) : nullptr;
  png_voidp mem_ctx = Size > 2 ? (png_voidp)(Data + 2) : nullptr;

  png_structp png_ptr = png_create_read_struct_2(
      PNG_LIBPNG_VER_STRING, error_ctx, fuzz_png_error, fuzz_png_warn,
      mem_ctx, fuzz_malloc, fuzz_free);
  if (!png_ptr) return;

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    return;
  }

  png_set_error_fn(png_ptr, mem_ctx, fuzz_png_error, fuzz_png_warn);

  FILE *fp = std::fopen("./dummy_file", "rb");
  if (fp) {
    png_init_io(png_ptr, fp);
    std::fclose(fp);
  }

  png_destroy_read_struct(&png_ptr, nullptr, nullptr);
}

extern "C" int LLVMFuzzerTestOneInput_67(const uint8_t *Data, size_t Size) {
  write_dummy_file(Data, Size);

  exercise_write_struct_basic(Data, Size);
  exercise_write_struct_custom(Data, Size);
  exercise_read_struct_custom(Data, Size);

  if ((Size % 3) == 0) {
    png_init_io(nullptr, nullptr);
    png_write_end(nullptr, nullptr);
    png_set_error_fn(nullptr, nullptr, fuzz_png_error, fuzz_png_warn);
  }

  return 0;
}