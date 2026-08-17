/* Copyright (c) 1988-1997 Sam Leffler
 * Copyright (c) 1991-1997 Silicon Graphics, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee, provided
 * that (i) the above copyright notices and this permission notice appear in
 * all copies of the software and related documentation, and (ii) the names of
 * Sam Leffler and Silicon Graphics may not be used in any advertising or
 * publicity relating to the software without the specific, prior written
 * permission of Sam Leffler and Silicon Graphics.
 *
 * THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * IN NO EVENT SHALL SAM LEFFLER OR SILICON GRAPHICS BE LIABLE FOR
 * ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND,
 * OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF
 * LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE.
 */

#include <cstddef>
#include <cstdint>
#include <cstring>

#include <tiffio.h>

extern "C" void DecodeFuzzIgnoreHandler(const char *, const char *, va_list) {}

namespace {

constexpr uint64_t kMaxAllocation = 64ULL * 1024 * 1024;
constexpr uint32_t kMaxDirectories = 16;
constexpr uint32_t kMaxRows = 64;
constexpr uint32_t kMaxStriles = 32;
constexpr uint16_t kMaxSamples = 4;

struct DecodeFuzzMemoryFile {
  const uint8_t *data;
  size_t size;
  uint64_t offset;
};

bool DecodeFuzzCanAllocate(uint64_t size) {
  return size > 0 && size <= kMaxAllocation &&
         size <= static_cast<uint64_t>(TIFF_TMSIZE_T_MAX);
}

void *DecodeFuzzAllocate(uint64_t size) {
  if (!DecodeFuzzCanAllocate(size)) {
    return nullptr;
  }
  return _TIFFmalloc(static_cast<tmsize_t>(size));
}

uint32_t *DecodeFuzzAllocateRGBA(uint64_t pixels) {
  const uint64_t bytes = pixels * sizeof(uint32_t);
  if (pixels == 0 || bytes / sizeof(uint32_t) != pixels ||
      !DecodeFuzzCanAllocate(bytes)) {
    return nullptr;
  }
  return static_cast<uint32_t *>(_TIFFmalloc(static_cast<tmsize_t>(bytes)));
}

uint32_t DecodeFuzzClampCount(uint32_t value, uint32_t limit) {
  return value < limit ? value : limit;
}

uint16_t DecodeFuzzClampSamples(uint16_t value) {
  return value < kMaxSamples ? value : kMaxSamples;
}

TIFF *DecodeFuzzOpen(DecodeFuzzMemoryFile *file);
void DecodeFuzzDecodeCurrentDirectory(TIFF *tif);

void DecodeFuzzReadRawStrile(TIFF *tif, uint32_t strile, int is_tiled) {
  int err = 0;
  const uint64_t raw_size = TIFFGetStrileByteCountWithErr(tif, strile, &err);
  if (err || !DecodeFuzzCanAllocate(raw_size)) {
    return;
  }

  void *raw = _TIFFmalloc(static_cast<tmsize_t>(raw_size));
  if (raw == nullptr) {
    return;
  }

  if (is_tiled) {
    (void)TIFFReadRawTile(tif, strile, raw, static_cast<tmsize_t>(raw_size));
  } else {
    (void)TIFFReadRawStrip(tif, strile, raw, static_cast<tmsize_t>(raw_size));
  }
  _TIFFfree(raw);
}

void DecodeFuzzDecodeTiled(TIFF *tif) {
  const uint64_t tile_size = TIFFTileSize64(tif);
  const uint32_t tile_count = DecodeFuzzClampCount(TIFFNumberOfTiles(tif), kMaxStriles);

  void *decoded = DecodeFuzzAllocate(tile_size);
  if (decoded != nullptr) {
    for (uint32_t tile = 0; tile < tile_count; ++tile) {
      (void)TIFFReadEncodedTile(tif, tile, decoded,
                                static_cast<tmsize_t>(tile_size));
    }
    _TIFFfree(decoded);
  }

  for (uint32_t tile = 0; tile < tile_count; ++tile) {
    DecodeFuzzReadRawStrile(tif, tile, 1);
  }

  uint32_t tile_width = 0;
  uint32_t tile_length = 0;
  (void)TIFFGetField(tif, TIFFTAG_TILEWIDTH, &tile_width);
  (void)TIFFGetField(tif, TIFFTAG_TILELENGTH, &tile_length);

  char rgba_error[1024];
  if (!TIFFRGBAImageOK(tif, rgba_error) || tile_width == 0 || tile_length == 0) {
    return;
  }

  const uint64_t tile_pixels = static_cast<uint64_t>(tile_width) * tile_length;
  uint32_t *rgba = DecodeFuzzAllocateRGBA(tile_pixels);
  if (rgba == nullptr) {
    return;
  }

  (void)TIFFReadRGBATile(tif, 0, 0, rgba);
  (void)TIFFReadRGBATileExt(tif, 0, 0, rgba, 1);
  _TIFFfree(rgba);
}

void DecodeFuzzDecodeStripped(TIFF *tif) {
  const uint64_t strip_size = TIFFStripSize64(tif);
  const uint32_t strip_count = DecodeFuzzClampCount(TIFFNumberOfStrips(tif), kMaxStriles);

  void *decoded = DecodeFuzzAllocate(strip_size);
  if (decoded != nullptr) {
    for (uint32_t strip = 0; strip < strip_count; ++strip) {
      (void)TIFFReadEncodedStrip(tif, strip, decoded,
                                 static_cast<tmsize_t>(strip_size));
    }
    _TIFFfree(decoded);
  }

  for (uint32_t strip = 0; strip < strip_count; ++strip) {
    DecodeFuzzReadRawStrile(tif, strip, 0);
  }

  const uint64_t scanline_size = TIFFScanlineSize64(tif);
  void *scanline = DecodeFuzzAllocate(scanline_size);
  if (scanline != nullptr) {
    uint32_t image_length = 0;
    uint16_t samples_per_pixel = 1;
    uint16_t planar_config = PLANARCONFIG_CONTIG;
    (void)TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &image_length);
    (void)TIFFGetFieldDefaulted(tif, TIFFTAG_SAMPLESPERPIXEL,
                                &samples_per_pixel);
    (void)TIFFGetFieldDefaulted(tif, TIFFTAG_PLANARCONFIG, &planar_config);

    const uint32_t row_limit = DecodeFuzzClampCount(image_length, kMaxRows);
    const uint16_t sample_limit =
        planar_config == PLANARCONFIG_SEPARATE
            ? DecodeFuzzClampSamples(samples_per_pixel)
            : 1;

    for (uint16_t sample = 0; sample < sample_limit; ++sample) {
      for (uint32_t row = 0; row < row_limit; ++row) {
        (void)TIFFReadScanline(tif, scanline, row, sample);
      }
    }
    _TIFFfree(scanline);
  }

  uint32_t image_width = 0;
  uint32_t image_length = 0;
  uint32_t rows_per_strip = 0;
  (void)TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &image_width);
  (void)TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &image_length);
  (void)TIFFGetFieldDefaulted(tif, TIFFTAG_ROWSPERSTRIP, &rows_per_strip);

  if (image_width == 0 || image_length == 0) {
    return;
  }
  if (rows_per_strip == 0 || rows_per_strip > image_length) {
    rows_per_strip = image_length;
  }

  char rgba_error[1024];
  if (!TIFFRGBAImageOK(tif, rgba_error)) {
    return;
  }

  const uint64_t strip_pixels = static_cast<uint64_t>(image_width) * rows_per_strip;
  uint32_t *rgba = DecodeFuzzAllocateRGBA(strip_pixels);
  if (rgba == nullptr) {
    return;
  }

  (void)TIFFReadRGBAStrip(tif, 0, rgba);
  (void)TIFFReadRGBAStripExt(tif, 0, rgba, 1);
  _TIFFfree(rgba);
}

void DecodeFuzzDecodeFullImage(TIFF *tif) {
  uint32_t width = 0;
  uint32_t height = 0;
  (void)TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
  (void)TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);

  char rgba_error[1024];
  if (!TIFFRGBAImageOK(tif, rgba_error) || width == 0 || height == 0) {
    return;
  }

  const uint64_t pixels = static_cast<uint64_t>(width) * height;
  uint32_t *raster = DecodeFuzzAllocateRGBA(pixels);
  if (raster == nullptr) {
    return;
  }

  (void)TIFFReadRGBAImageOriented(tif, width, height, raster,
                                  ORIENTATION_BOTLEFT, 0);
  _TIFFfree(raster);
}

void DecodeFuzzDecodeCurrentDirectory(TIFF *tif) {
  (void)TIFFSetField(tif, TIFFTAG_JPEGCOLORMODE, JPEGCOLORMODE_RGB);
  if (TIFFIsTiled(tif)) {
    DecodeFuzzDecodeTiled(tif);
  } else {
    DecodeFuzzDecodeStripped(tif);
  }
  DecodeFuzzDecodeFullImage(tif);
}

}  // namespace

extern "C" tmsize_t DecodeFuzzRead(thandle_t handle, void *buf, tmsize_t size) {
  if (size <= 0) {
    return 0;
  }

  DecodeFuzzMemoryFile *file = static_cast<DecodeFuzzMemoryFile *>(handle);
  if (file->offset >= file->size) {
    return 0;
  }

  const size_t remaining = file->size - static_cast<size_t>(file->offset);
  const size_t requested = static_cast<size_t>(size);
  const size_t count = remaining < requested ? remaining : requested;

  if (count != 0) {
    std::memcpy(buf, file->data + file->offset, count);
    file->offset += count;
  }
  return static_cast<tmsize_t>(count);
}

extern "C" tmsize_t DecodeFuzzWrite(thandle_t, void *, tmsize_t) {
  return 0;
}

extern "C" toff_t DecodeFuzzSeek(thandle_t handle, toff_t offset, int whence) {
  DecodeFuzzMemoryFile *file = static_cast<DecodeFuzzMemoryFile *>(handle);
  uint64_t base = 0;

  switch (whence) {
    case SEEK_SET:
      base = 0;
      break;
    case SEEK_CUR:
      base = file->offset;
      break;
    case SEEK_END:
      base = file->size;
      break;
    default:
      return static_cast<toff_t>(-1);
  }

  if (base > UINT64_MAX - static_cast<uint64_t>(offset)) {
    return static_cast<toff_t>(-1);
  }

  file->offset = base + static_cast<uint64_t>(offset);
  return static_cast<toff_t>(file->offset);
}

extern "C" int DecodeFuzzClose(thandle_t) {
  return 0;
}

extern "C" toff_t DecodeFuzzSize(thandle_t handle) {
  const DecodeFuzzMemoryFile *file =
      static_cast<const DecodeFuzzMemoryFile *>(handle);
  return static_cast<toff_t>(file->size);
}

namespace {

TIFF *DecodeFuzzOpen(DecodeFuzzMemoryFile *file) {
  return TIFFClientOpen("<memory>", "r", reinterpret_cast<thandle_t>(file),
                        DecodeFuzzRead, DecodeFuzzWrite, DecodeFuzzSeek,
                        DecodeFuzzClose, DecodeFuzzSize, nullptr, nullptr);
}

}  // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  if (size < 4) {
    return 0;
  }

  TIFFSetErrorHandler(DecodeFuzzIgnoreHandler);
  TIFFSetWarningHandler(DecodeFuzzIgnoreHandler);

  DecodeFuzzMemoryFile file = {data, size, 0};
  TIFF *tif = DecodeFuzzOpen(&file);
  if (tif == nullptr) {
    return 0;
  }

  for (uint32_t dir = 0; dir < kMaxDirectories; ++dir) {
    DecodeFuzzDecodeCurrentDirectory(tif);
    if (TIFFLastDirectory(tif) || !TIFFReadDirectory(tif)) {
      break;
    }
  }

  TIFFClose(tif);
  return 0;
}
