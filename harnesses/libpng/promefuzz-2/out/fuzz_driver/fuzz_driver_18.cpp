// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_create_read_struct at pngread.c:25:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_warning at pngerror.c:216:1 in png.h
// png_chunk_warning at pngerror.c:497:1 in png.h
// png_benign_error at pngerror.c:362:1 in png.h
// png_chunk_benign_error at pngerror.c:514:1 in png.h
// png_warning at pngerror.c:216:1 in png.h
// png_chunk_warning at pngerror.c:497:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_chunk_warning at pngerror.c:497:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_error at pngerror.c:38:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_chunk_error at pngerror.c:479:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_benign_error at pngerror.c:362:1 in png.h
// png_chunk_benign_error at pngerror.c:514:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
#include <png.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <setjmp.h>
#include <string>

static void CustomErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void CustomWarningFn(png_structp, png_const_charp) {
}

static uint32_t ReadU32(const uint8_t* data, size_t size, size_t offset) {
  if (offset + 4 > size) return 0;
  return (static_cast<uint32_t>(data[offset]) << 24) |
         (static_cast<uint32_t>(data[offset + 1]) << 16) |
         (static_cast<uint32_t>(data[offset + 2]) << 8) |
         static_cast<uint32_t>(data[offset + 3]);
}

static std::string MakeMessage(const uint8_t* data, size_t size,
                               size_t start, size_t max_len) {
  std::string out;
  if (start >= size) return "fuzz";
  size_t end = start + max_len;
  if (end > size) end = size;
  out.reserve(end - start + 1);
  for (size_t i = start; i < end; ++i) {
    char c = static_cast<char>(data[i]);
    if (c == '\0' || c == '\n' || c == '\r')
      out.push_back(' ');
    else
      out.push_back(c);
  }
  if (out.empty()) out = "fuzz";
  return out;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE* fp = fopen("./dummy_file", "wb");
  if (fp != NULL) {
    if (Size > 0) fwrite(Data, 1, Size, fp);
    fclose(fp);
  }

  std::string msg1 = MakeMessage(Data, Size, 0, Size / 3 + 1);
  std::string msg2 = MakeMessage(Data, Size, Size / 3, Size / 3 + 1);
  std::string msg3 = MakeMessage(Data, Size, (Size / 3) * 2, Size);

  png_structp png_ptr_warn = png_create_read_struct(
      PNG_LIBPNG_VER_STRING, NULL, CustomErrorFn, CustomWarningFn);
  if (png_ptr_warn != NULL) {
    if (setjmp(png_jmpbuf(png_ptr_warn)) == 0) {
      png_warning(png_ptr_warn, msg1.c_str());
      png_chunk_warning(png_ptr_warn, msg2.c_str());
      png_benign_error(png_ptr_warn, msg3.c_str());
      png_chunk_benign_error(png_ptr_warn, msg1.c_str());
      png_warning(png_ptr_warn, "#1234 prefixed warning");
      png_chunk_warning(png_ptr_warn, "#5678 prefixed chunk warning");
    }
    png_destroy_read_struct(&png_ptr_warn, NULL, NULL);
  }

  if (Size > 0 && (Data[0] & 1)) {
    png_chunk_warning(NULL, msg2.c_str());
  }

  png_structp png_ptr_fatal1 = png_create_read_struct(
      PNG_LIBPNG_VER_STRING, NULL, CustomErrorFn, CustomWarningFn);
  if (png_ptr_fatal1 != NULL) {
    if (setjmp(png_jmpbuf(png_ptr_fatal1)) == 0) {
      png_error(png_ptr_fatal1, msg2.c_str());
    }
    png_destroy_read_struct(&png_ptr_fatal1, NULL, NULL);
  }

  png_structp png_ptr_fatal2 = png_create_read_struct(
      PNG_LIBPNG_VER_STRING, NULL, CustomErrorFn, CustomWarningFn);
  if (png_ptr_fatal2 != NULL) {
    if (setjmp(png_jmpbuf(png_ptr_fatal2)) == 0) {
      png_chunk_error(png_ptr_fatal2, msg3.c_str());
    }
    png_destroy_read_struct(&png_ptr_fatal2, NULL, NULL);
  }

  if (Size > 4 && (ReadU32(Data, Size, 0) & 1)) {
    png_structp png_ptr_more = png_create_read_struct(
        PNG_LIBPNG_VER_STRING, NULL, CustomErrorFn, CustomWarningFn);
    if (png_ptr_more != NULL) {
      if (setjmp(png_jmpbuf(png_ptr_more)) == 0) {
        png_benign_error(png_ptr_more, "#9999 benign message");
        png_chunk_benign_error(png_ptr_more, "#1111 chunk benign message");
      }
      png_destroy_read_struct(&png_ptr_more, NULL, NULL);
    }
  }

  return 0;
}