#include <png.h>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>

static uint32_t ReadU32(const uint8_t *data, size_t size, size_t offset) {
  if (offset + 4 > size) return 0;
  return (static_cast<uint32_t>(data[offset]) << 24) |
         (static_cast<uint32_t>(data[offset + 1]) << 16) |
         (static_cast<uint32_t>(data[offset + 2]) << 8) |
         static_cast<uint32_t>(data[offset + 3]);
}

static void WriteDummyFile(const uint8_t *data, size_t size) {
  FILE *fp = std::fopen("./dummy_file", "wb");
  if (!fp) return;
  if (size > 0) std::fwrite(data, 1, size, fp);
  std::fclose(fp);
}

static png_uint_32 ChooseFormat(uint32_t selector) {
  switch (selector % 6) {
    case 0: return PNG_FORMAT_RGBA;
    case 1: return PNG_FORMAT_RGB;
    case 2: return PNG_FORMAT_GA;
    case 3: return PNG_FORMAT_GRAY;
    case 4: return PNG_FORMAT_BGRA;
    default: return PNG_FORMAT_ABGR;
  }
}

static size_t SafeImageSize(png_image &image) {
  if (image.width == 0 || image.height == 0) return 0;
  png_alloc_size_t size = PNG_IMAGE_SIZE(image);
  if (size > (1u << 26)) return 0;
  return static_cast<size_t>(size);
}

static png_int_32 ChooseRowStride(uint32_t selector, const png_image &image) {
  png_int_32 natural = 0;
  png_alloc_size_t rs = PNG_IMAGE_ROW_STRIDE(image);
  if (rs <= 0x7fffffffU) natural = static_cast<png_int_32>(rs);

  switch (selector % 8) {
    case 0: return 0;
    case 1: return natural;
    case 2: return natural > 0 ? natural + 1 : 1;
    case 3: return natural > 1 ? natural - 1 : 0;
    case 4: return 1;
    case 5: return -1;
    case 6: return 0x7fffffff;
    default: return static_cast<png_int_32>(0x40000000);
  }
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  WriteDummyFile(Data, Size);

  png_color background;
  std::memset(&background, 0, sizeof(background));
  if (Size > 0) background.red = Data[0];
  if (Size > 1) background.green = Data[1];
  if (Size > 2) background.blue = Data[2];

  png_color colormap_storage[256];
  std::memset(colormap_storage, 0, sizeof(colormap_storage));
  for (size_t i = 0; i < 256; ++i) {
    size_t base = i * 3;
    if (base < Size) colormap_storage[i].red = Data[base];
    if (base + 1 < Size) colormap_storage[i].green = Data[base + 1];
    if (base + 2 < Size) colormap_storage[i].blue = Data[base + 2];
  }

  {
    png_image image;
    std::memset(&image, 0, sizeof(image));
    image.version = PNG_IMAGE_VERSION;

    if (Size > 0 && png_image_begin_read_from_memory(&image, Data, Size)) {
      image.format = ChooseFormat(ReadU32(Data, Size, 4));
      size_t buf_size = SafeImageSize(image);
      std::vector<uint8_t> buffer(buf_size ? buf_size
                                           : std::min<size_t>(Size + 64, 1u << 20));

      png_int_32 row_stride = ChooseRowStride(ReadU32(Data, Size, 8), image);
      png_const_colorp bg_ptr = (Size & 1) ? &background : nullptr;
      void *cmap_ptr = (Size & 2) ? static_cast<void*>(colormap_storage) : nullptr;

      (void)png_image_finish_read(&image, bg_ptr,
                                  buffer.empty() ? nullptr : buffer.data(),
                                  row_stride, cmap_ptr);
      png_image_free(&image);
      png_image_free(&image);
    } else {
      png_image_free(&image);
    }
  }

  {
    png_image image;
    std::memset(&image, 0, sizeof(image));
    image.version = PNG_IMAGE_VERSION;

    FILE *fp = std::fopen("./dummy_file", "rb");
    if (fp) {
      if (png_image_begin_read_from_stdio(&image, fp)) {
        image.format = ChooseFormat(ReadU32(Data, Size, 12));
        size_t buf_size = SafeImageSize(image);
        std::vector<uint8_t> buffer(buf_size ? buf_size
                                             : std::min<size_t>(Size + 128, 1u << 20));

        png_int_32 row_stride = ChooseRowStride(ReadU32(Data, Size, 16), image);
        png_const_colorp bg_ptr = (Size & 4) ? &background : nullptr;
        void *cmap_ptr = (Size & 8) ? static_cast<void*>(colormap_storage) : nullptr;

        (void)png_image_finish_read(&image, bg_ptr,
                                    buffer.empty() ? nullptr : buffer.data(),
                                    row_stride, cmap_ptr);
        png_image_free(&image);
      } else {
        png_image_free(&image);
        std::fclose(fp);
      }
    }
  }

  {
    png_image image;
    std::memset(&image, 0, sizeof(image));
    image.version = PNG_IMAGE_VERSION;

    if (png_image_begin_read_from_file(&image, "./dummy_file")) {
      image.format = ChooseFormat(ReadU32(Data, Size, 20));
      size_t buf_size = SafeImageSize(image);
      std::vector<uint8_t> buffer(buf_size ? buf_size
                                           : std::min<size_t>(Size + 256, 1u << 20));

      png_int_32 row_stride = ChooseRowStride(ReadU32(Data, Size, 24), image);
      png_const_colorp bg_ptr = (Size & 16) ? &background : nullptr;
      void *cmap_ptr = (Size & 32) ? static_cast<void*>(colormap_storage) : nullptr;

      (void)png_image_finish_read(&image, bg_ptr,
                                  buffer.empty() ? nullptr : buffer.data(),
                                  row_stride, cmap_ptr);
      png_image_free(&image);
    } else {
      png_image_free(&image);
    }
  }

  {
    png_image image;
    std::memset(&image, 0, sizeof(image));
    image.version = PNG_IMAGE_VERSION;
    image.width = (ReadU32(Data, Size, 28) % 64) + 1;
    image.height = (ReadU32(Data, Size, 32) % 64) + 1;
    image.format = ChooseFormat(ReadU32(Data, Size, 36));

    size_t buf_size = SafeImageSize(image);
    std::vector<uint8_t> buffer(buf_size ? buf_size
                                         : std::min<size_t>(Size + 512, 1u << 20));

    if (!buffer.empty() && Size > 0) {
      for (size_t i = 0; i < buffer.size(); ++i) buffer[i] = Data[i % Size];
    }

    png_int_32 row_stride = ChooseRowStride(ReadU32(Data, Size, 40), image);
    const void *cmap_ptr = (Size & 64) ? static_cast<const void*>(colormap_storage) : nullptr;
    int convert_to_8bit = static_cast<int>(ReadU32(Data, Size, 44) & 1);

    (void)png_image_write_to_file(&image, "./dummy_file", convert_to_8bit,
                                  buffer.empty() ? nullptr : buffer.data(),
                                  row_stride, cmap_ptr);
    png_image_free(&image);
  }

  return 0;
}