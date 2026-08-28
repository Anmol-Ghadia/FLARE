// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_longjmp at pngerror.c:757:1 in png.h
// png_sig_cmp at png.c:74:1 in png.h
// png_sig_cmp at png.c:74:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_mem_fn at pngmem.c:260:1 in png.h
#include <png.h>

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <setjmp.h>

static void PNGAPI FuzzErrorFn(png_structp png_ptr, png_const_charp) {
  png_longjmp(png_ptr, 1);
}

static void PNGAPI FuzzWarnFn(png_structp, png_const_charp) {
}

static png_voidp PNGAPI FuzzMallocFn(png_structp, png_alloc_size_t size) {
  if (size == 0) return nullptr;
  return std::malloc(size);
}

static void PNGAPI FuzzFreeFn(png_structp, png_voidp ptr) {
  std::free(ptr);
}

extern "C" int LLVMFuzzerTestOneInput_4(const uint8_t *Data, size_t Size) {
  FILE *fp = std::fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    if (Data != nullptr && Size > 0) {
      std::fwrite(Data, 1, Size, fp);
    }
    std::fclose(fp);
  }

  size_t start = 0;
  size_t num_to_check = 0;
  if (Size > 0) start = Data[0] % 10;
  if (Size > 1) num_to_check = Data[1] % 10;

  if (Data != nullptr && Size > 0 && start < Size) {
    size_t max_available = Size - start;
    if (num_to_check > max_available) num_to_check = max_available;
    (void)png_sig_cmp(reinterpret_cast<png_const_bytep>(Data), start, num_to_check);
  } else {
    static const png_byte kDummySig[8] = {0};
    (void)png_sig_cmp(kDummySig, start, num_to_check);
  }

  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr,
                                               FuzzErrorFn, FuzzWarnFn);
  if (png_ptr == nullptr) {
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr)) != 0) {
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    return 0;
  }

  png_infop info_ptr1 = png_create_info_struct(png_ptr);
  png_destroy_read_struct(&png_ptr, &info_ptr1, nullptr);

  png_infop info_ptr2 = png_create_info_struct(png_ptr);
  png_destroy_read_struct(&png_ptr, &info_ptr2, nullptr);

  png_set_mem_fn(png_ptr,
                 Size > 2 ? const_cast<uint8_t*>(Data) : nullptr,
                 (Size > 2 && (Data[2] & 1)) ? FuzzMallocFn : nullptr,
                 (Size > 3 && (Data[3] & 1)) ? FuzzFreeFn : nullptr);

  return 0;
}