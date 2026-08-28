#include <png.h>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

static uint32_t ReadU32(const uint8_t *data, size_t size, size_t &off) {
  uint32_t v = 0;
  for (int i = 0; i < 4; ++i) {
    v <<= 8;
    if (off < size) v |= data[off++];
  }
  return v;
}

static int ReadS32(const uint8_t *data, size_t size, size_t &off) {
  return static_cast<int>(ReadU32(data, size, off));
}

static uint8_t ReadU8(const uint8_t *data, size_t size, size_t &off) {
  if (off < size) return data[off++];
  return 0;
}

static png_uint_32 ClampDim(uint32_t v) {
  const png_uint_32 kMaxDim = 64;
  return static_cast<png_uint_32>((v % kMaxDim) + 1);
}

static png_uint_32 ChooseFormat(uint8_t selector) {
  switch (selector % 8) {
    case 0: return PNG_FORMAT_GRAY;
    case 1: return PNG_FORMAT_GA;
    case 2: return PNG_FORMAT_RGB;
    case 3: return PNG_FORMAT_RGBA;
    case 4: return PNG_FORMAT_LINEAR_Y;
    case 5: return PNG_FORMAT_LINEAR_Y_ALPHA;
    case 6: return PNG_FORMAT_LINEAR_RGB;
    default: return PNG_FORMAT_LINEAR_RGB_ALPHA;
  }
}

static png_uint_32 GetChannelsFromFormat(png_uint_32 format) {
  if ((format & PNG_FORMAT_FLAG_COLOR) != 0)
    return ((format & PNG_FORMAT_FLAG_ALPHA) != 0) ? 4u : 3u;
  return ((format & PNG_FORMAT_FLAG_ALPHA) != 0) ? 2u : 1u;
}

static png_uint_32 GetSampleSizeFromFormat(png_uint_32 format) {
  return ((format & PNG_FORMAT_FLAG_LINEAR) != 0) ? 2u : 1u;
}

static size_t SafeImageSize(png_uint_32 width, png_uint_32 height, png_uint_32 format) {
  png_uint_32 channels = GetChannelsFromFormat(format);
  png_uint_32 sample_size = GetSampleSizeFromFormat(format);
  if (channels == 0 || sample_size == 0) return 0;
  size_t row = static_cast<size_t>(width) * channels * sample_size;
  size_t total = row * static_cast<size_t>(height);
  const size_t kMax = 1 << 20;
  if (total == 0 || total > kMax) return 0;
  return total;
}

static void FillBuffer(std::vector<uint8_t> &buf, const uint8_t *data, size_t size, size_t off) {
  if (buf.empty()) return;
  if (off >= size) {
    std::fill(buf.begin(), buf.end(), 0);
    return;
  }
  for (size_t i = 0; i < buf.size(); ++i) {
    buf[i] = data[(off + i) % size];
  }
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  size_t off = 0;

  png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
  png_infop info_ptr = nullptr;
  if (png_ptr != nullptr) {
    info_ptr = png_create_info_struct(png_ptr);
    int window_bits = ReadS32(Data, Size, off);
    int flush_rows = ReadS32(Data, Size, off);
    png_set_compression_window_bits(png_ptr, window_bits);
    png_set_flush(png_ptr, flush_rows);

    png_set_compression_window_bits(nullptr, window_bits);
    png_set_flush(nullptr, flush_rows);
  }

  png_image image;
  std::memset(&image, 0, sizeof(image));
  image.version = PNG_IMAGE_VERSION;
  image.width = ClampDim(ReadU32(Data, Size, off));
  image.height = ClampDim(ReadU32(Data, Size, off));
  image.format = ChooseFormat(ReadU8(Data, Size, off));
  image.flags = 0;
  image.colormap_entries = 0;
  image.warning_or_error = 0;
  image.message[0] = '\0';

  size_t pixel_bytes = SafeImageSize(image.width, image.height, image.format);
  if (pixel_bytes == 0) {
    if (info_ptr != nullptr || png_ptr != nullptr)
      png_destroy_write_struct(&png_ptr, &info_ptr);
    png_image_free(&image);
    return 0;
  }

  std::vector<uint8_t> buffer(pixel_bytes);
  FillBuffer(buffer, Data, Size, off);

  png_int_32 row_stride = 0;
  if (ReadU8(Data, Size, off) & 1) {
    png_uint_32 channels = GetChannelsFromFormat(image.format);
    png_uint_32 sample_size = GetSampleSizeFromFormat(image.format);
    size_t min_row = static_cast<size_t>(image.width) * channels * sample_size;
    row_stride = static_cast<png_int_32>(min_row);
    if (ReadU8(Data, Size, off) & 1)
      row_stride = -row_stride;
  }

  int convert_to_8_bit = ReadU8(Data, Size, off) & 1;

  png_alloc_size_t memory_bytes = 0;
  (void)png_image_write_to_memory(&image, nullptr, &memory_bytes, convert_to_8_bit,
                                  buffer.data(), row_stride, nullptr);

  if (memory_bytes > 0 && memory_bytes <= (1u << 20)) {
    std::vector<uint8_t> out(static_cast<size_t>(memory_bytes));
    png_image image_mem;
    std::memset(&image_mem, 0, sizeof(image_mem));
    image_mem.version = PNG_IMAGE_VERSION;
    image_mem.width = image.width;
    image_mem.height = image.height;
    image_mem.format = image.format;

    png_alloc_size_t out_size = memory_bytes;
    (void)png_image_write_to_memory(&image_mem, out.data(), &out_size, convert_to_8_bit,
                                    buffer.data(), row_stride, nullptr);
    png_image_free(&image_mem);
  }

  {
    FILE *fp = std::fopen("./dummy_file", "wb");
    if (fp != nullptr) {
      png_image image_stdio;
      std::memset(&image_stdio, 0, sizeof(image_stdio));
      image_stdio.version = PNG_IMAGE_VERSION;
      image_stdio.width = image.width;
      image_stdio.height = image.height;
      image_stdio.format = image.format;

      (void)png_image_write_to_stdio(&image_stdio, fp, convert_to_8_bit,
                                     buffer.data(), row_stride, nullptr);
      std::fclose(fp);
      png_image_free(&image_stdio);
    }
  }

  {
    png_image image_file;
    std::memset(&image_file, 0, sizeof(image_file));
    image_file.version = PNG_IMAGE_VERSION;
    image_file.width = image.width;
    image_file.height = image.height;
    image_file.format = image.format;

    (void)png_image_write_to_file(&image_file, "./dummy_file", convert_to_8_bit,
                                  buffer.data(), row_stride, nullptr);
    png_image_free(&image_file);
  }

  png_image_free(&image);

  if (info_ptr != nullptr || png_ptr != nullptr)
    png_destroy_write_struct(&png_ptr, &info_ptr);

  return 0;
}