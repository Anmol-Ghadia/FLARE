// This fuzz driver is generated for library libpng, aiming to fuzz the following functions:
// png_create_write_struct at pngwrite.c:492:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_init_io at png.c:703:1 in png.h
// png_set_IHDR at pngset.c:254:1 in png.h
// png_set_pCAL at pngset.c:312:1 in png.h
// png_set_text at pngset.c:762:1 in png.h
// png_write_info at pngwrite.c:192:1 in png.h
// png_write_row at pngwrite.c:693:1 in png.h
// png_get_pCAL at pngget.c:891:1 in png.h
// png_get_text at pngget.c:1049:1 in png.h
// png_write_end at pngwrite.c:358:1 in png.h
// png_free_data at png.c:471:1 in png.h
// png_free_data at png.c:471:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_destroy_write_struct at pngwrite.c:974:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_create_read_struct at pngread.c:25:1 in png.h
// png_create_info_struct at png.c:353:1 in png.h
// png_set_longjmp_fn at pngerror.c:597:1 in png.h
// png_init_io at png.c:703:1 in png.h
// png_set_sig_bytes at png.c:47:1 in png.h
// png_read_info at pngread.c:92:1 in png.h
// png_get_text at pngget.c:1049:1 in png.h
// png_get_pCAL at pngget.c:891:1 in png.h
// png_free_data at png.c:471:1 in png.h
// png_free_data at png.c:471:1 in png.h
// png_free_data at png.c:471:1 in png.h
// png_destroy_read_struct at pngread.c:1011:1 in png.h
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

#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <string>

static void PngErrorFn(png_structp png_ptr, png_const_charp) {
  longjmp(png_jmpbuf(png_ptr), 1);
}

static void PngWarningFn(png_structp, png_const_charp) {
}

static png_uint_32 ReadU32(const uint8_t* data, size_t size, size_t offset) {
  if (offset + 4 > size) return 0;
  return (static_cast<png_uint_32>(data[offset]) << 24) |
         (static_cast<png_uint_32>(data[offset + 1]) << 16) |
         (static_cast<png_uint_32>(data[offset + 2]) << 8) |
         (static_cast<png_uint_32>(data[offset + 3]));
}

static int ReadS32(const uint8_t* data, size_t size, size_t offset) {
  return static_cast<int>(ReadU32(data, size, offset));
}

static std::string MakeString(const uint8_t* data, size_t size, size_t offset, size_t max_len) {
  if (offset >= size) return std::string();
  size_t len = data[offset] % (max_len + 1);
  if (offset + 1 + len > size) len = size - offset - 1;
  return std::string(reinterpret_cast<const char*>(data + offset + 1), len);
}

static void WriteFile(const uint8_t* data, size_t size) {
  FILE* fp = std::fopen("./dummy_file", "wb");
  if (!fp) return;
  if (size) std::fwrite(data, 1, size, fp);
  std::fclose(fp);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  WriteFile(Data, Size);

  // Read path: exercise png_read_info, png_get_text, png_get_pCAL, png_free_data.
  {
    FILE* fp = std::fopen("./dummy_file", "rb");
    if (fp) {
      png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr,
                                                   PngErrorFn, PngWarningFn);
      if (png_ptr) {
        png_infop info_ptr = png_create_info_struct(png_ptr);
        if (info_ptr) {
          if (setjmp(png_jmpbuf(png_ptr)) == 0) {
            png_init_io(png_ptr, fp);

            if (Size >= 8 && (Data[0] & 1)) {
              png_byte sig[8];
              if (std::fread(sig, 1, 8, fp) == 8) {
                png_set_sig_bytes(png_ptr, 8);
              } else {
                std::rewind(fp);
              }
            }

            png_read_info(png_ptr, info_ptr);

            png_textp text_ptr = nullptr;
            int num_text = -1;
            (void)png_get_text(png_ptr, info_ptr, &text_ptr, &num_text);

            png_charp purpose = nullptr;
            png_int_32 X0 = 0, X1 = 0;
            int type = 0, nparams = 0;
            png_charp units = nullptr;
            png_charpp params = nullptr;
            (void)png_get_pCAL(png_ptr, info_ptr, &purpose, &X0, &X1, &type, &nparams,
                               &units, &params);

            png_free_data(png_ptr, info_ptr, PNG_FREE_TEXT, -1);
            png_free_data(png_ptr, info_ptr, PNG_FREE_PCAL, -1);
            if (num_text > 0) {
              png_free_data(png_ptr, info_ptr, PNG_FREE_TEXT, 0);
            }
          }
          png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
        } else {
          png_destroy_read_struct(&png_ptr, nullptr, nullptr);
        }
      }
      std::fclose(fp);
    }
  }

  // Write path: exercise png_set_pCAL, png_write_end, png_get_pCAL, png_get_text, png_free_data.
  {
    FILE* fp = std::fopen("./dummy_file", "wb");
    if (fp) {
      png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr,
                                                    PngErrorFn, PngWarningFn);
      if (png_ptr) {
        png_infop info_ptr = png_create_info_struct(png_ptr);
        if (info_ptr) {
          if (setjmp(png_jmpbuf(png_ptr)) == 0) {
            png_init_io(png_ptr, fp);

            png_uint_32 width = (ReadU32(Data, Size, 0) % 8) + 1;
            png_uint_32 height = (ReadU32(Data, Size, 4) % 8) + 1;
            int color_type = (Size > 8 && (Data[8] & 1)) ? PNG_COLOR_TYPE_RGB : PNG_COLOR_TYPE_GRAY;
            int bit_depth = 8;

            png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, color_type,
                         PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

            std::string purpose_s = MakeString(Data, Size, 9, 32);
            std::string units_s = MakeString(Data, Size, 50, 32);
            int type = Size > 90 ? (Data[90] % 4) : 0;
            int nparams = Size > 91 ? (Data[91] % 4) : 0;
            int x0 = ReadS32(Data, Size, 92);
            int x1 = ReadS32(Data, Size, 96);

            std::vector<std::string> param_storage;
            std::vector<png_charp> params;
            for (int i = 0; i < nparams; ++i) {
              size_t off = 100 + static_cast<size_t>(i) * 8;
              param_storage.push_back(MakeString(Data, Size, off, 16));
            }
            for (int i = 0; i < nparams; ++i) {
              params.push_back(const_cast<png_charp>(param_storage[i].c_str()));
            }

            png_set_pCAL(png_ptr, info_ptr,
                         purpose_s.empty() ? "p" : purpose_s.c_str(),
                         x0, x1, type, nparams,
                         units_s.empty() ? "u" : units_s.c_str(),
                         params.empty() ? nullptr : params.data());

            if (Size > 10) {
              png_text text;
              std::memset(&text, 0, sizeof(text));
              std::string key = "K";
              std::string value = MakeString(Data, Size, 120, 64);
              text.compression = PNG_TEXT_COMPRESSION_NONE;
              text.key = const_cast<png_charp>(key.c_str());
              text.text = const_cast<png_charp>(value.c_str());
              text.text_length = value.size();
              png_set_text(png_ptr, info_ptr, &text, 1);
            }

            png_write_info(png_ptr, info_ptr);

            std::vector<png_byte> row(width * (color_type == PNG_COLOR_TYPE_RGB ? 3u : 1u), 0);
            for (png_uint_32 y = 0; y < height; ++y) {
              for (size_t i = 0; i < row.size(); ++i) {
                row[i] = (Size > 0) ? Data[(i + y) % Size] : 0;
              }
              png_write_row(png_ptr, row.data());
            }

            png_charp purpose = nullptr;
            png_int_32 X0 = 0, X1 = 0;
            int out_type = 0, out_nparams = 0;
            png_charp units = nullptr;
            png_charpp out_params = nullptr;
            (void)png_get_pCAL(png_ptr, info_ptr, &purpose, &X0, &X1, &out_type,
                               &out_nparams, &units, &out_params);

            png_textp text_ptr = nullptr;
            int num_text = 0;
            (void)png_get_text(png_ptr, info_ptr, &text_ptr, &num_text);

            png_write_end(png_ptr, info_ptr);

            png_free_data(png_ptr, info_ptr, PNG_FREE_TEXT, -1);
            png_free_data(png_ptr, info_ptr, PNG_FREE_PCAL, -1);
          }
          png_destroy_write_struct(&png_ptr, &info_ptr);
        } else {
          png_destroy_write_struct(&png_ptr, nullptr);
        }
      }
      std::fclose(fp);
    }
  }

  return 0;
}