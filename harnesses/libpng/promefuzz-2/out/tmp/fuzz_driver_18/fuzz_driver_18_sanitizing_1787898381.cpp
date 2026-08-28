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
#include <cstring>
#include <vector>
#include <string>
#include <setjmp.h>
#include <cstdio>

static void CustomErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void CustomWarningFn(png_structp, png_const_charp) {
}

static png_uint_32 ReadU32(const uint8_t* data, size_t size, size_t offset) {
  if (offset + 4 > size) return 0;
  return (static_cast<png_uint_32>(data[offset]) << 24) |
         (static_cast<png_uint_32>(data[offset + 1]) << 16) |
         (static_cast<png_uint_32>(data[offset + 2]) << 8) |
         static_cast<png_uint_32>(data[offset + 3]);
}

static std::string MakeMessage(const uint8_t* data, size_t size, size_t start, size_t len) {
  std::string out;
  if (start >= size) return "fuzz";
  size_t end = start + len;
  if (end > size) end = size;
  out.reserve(end - start + 16);
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

static void SetChunkName(png_structp png_ptr, png_uint_32 chunk_name) {
#if defined(PNG_READ_SUPPORTED) || defined(PNG_WRITE_SUPPORTED)
  png_ptr->chunk_name = chunk_name;
#else
  (void)png_ptr;
  (void)chunk_name;
#endif
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  FILE* fp = std::fopen("./dummy_file", "wb");
  if (fp != nullptr) {
    if (Size > 0) std::fwrite(Data, 1, Size, fp);
    std::fclose(fp);
  }

  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr,
                                               CustomErrorFn, CustomWarningFn);
  if (png_ptr == nullptr) {
    return 0;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    return 0;
  }

  if (Size > 0) {
    if (Data[0] & 1)
      png_ptr->flags |= PNG_FLAG_BENIGN_ERRORS_WARN;
    else
      png_ptr->flags &= ~PNG_FLAG_BENIGN_ERRORS_WARN;
  }

  png_uint_32 chunk_name = ReadU32(Data, Size, 0);
  if (chunk_name == 0) chunk_name = PNG_UINT_32(0x49484452U);
  SetChunkName(png_ptr, chunk_name);

  std::string msg1 = MakeMessage(Data, Size, 4, Size > 4 ? (Size - 4) / 3 : 0);
  std::string msg2 = MakeMessage(Data, Size, 4 + msg1.size(), Size > 8 ? (Size - 4 - msg1.size()) / 2 : 0);
  std::string msg3 = MakeMessage(Data, Size, 4 + msg1.size() + msg2.size(), Size);

  png_warning(png_ptr, msg1.c_str());
  png_chunk_warning(png_ptr, msg2.c_str());

  if (Size > 1) {
    png_benign_error(png_ptr, msg3.c_str());
  }

  if (Size > 2) {
    png_chunk_benign_error(png_ptr, msg1.c_str());
  }

  if (Size > 3) {
    png_ptr->flags ^= PNG_FLAG_BENIGN_ERRORS_WARN;
    png_benign_error(png_ptr, msg2.c_str());
  }

  if (Size > 4) {
    png_ptr->flags ^= PNG_FLAG_BENIGN_ERRORS_WARN;
    png_chunk_benign_error(png_ptr, msg3.c_str());
  }

  if (Size > 5) {
    if (Data[1] & 1) {
      png_chunk_warning(nullptr, msg1.c_str());
    } else {
      png_warning(png_ptr, "#1234 stripped-prefix test");
    }
  }

  if (Size > 6) {
    if (Data[2] & 1) {
      png_chunk_error(png_ptr, msg2.c_str());
    } else {
      png_error(png_ptr, msg3.c_str());
    }
  }

  png_destroy_read_struct(&png_ptr, nullptr, nullptr);
  return 0;
}