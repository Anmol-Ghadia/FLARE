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

extern "C" void DirectoryFuzzIgnoreHandler(const char *, const char *, va_list) {}

namespace {

constexpr uint32_t kMaxDirectories = 16;
constexpr uint16_t kMaxSubDirectories = 8;
constexpr int kMaxTags = 128;

struct DirectoryFuzzMemoryFile {
  const uint8_t *data;
  size_t size;
  uint64_t offset;
};

TIFF *DirectoryFuzzOpen(DirectoryFuzzMemoryFile *file);
void DirectoryFuzzProbeEXIFDirectory(const uint8_t *data, size_t size,
                                     toff_t offset);
void DirectoryFuzzProbeGPSDirectory(const uint8_t *data, size_t size,
                                    toff_t offset);
void DirectoryFuzzProbeSubDirectory(const uint8_t *data, size_t size,
                                    toff_t offset);

void DirectoryFuzzEnumerateTags(TIFF *tif) {
  int tag_count = TIFFGetTagListCount(tif);
  if (tag_count > kMaxTags) {
    tag_count = kMaxTags;
  }

  for (int tag_index = 0; tag_index < tag_count; ++tag_index) {
    (void)TIFFGetTagListEntry(tif, tag_index);
  }
}

void DirectoryFuzzProbeLinkedDirectories(const uint8_t *data, size_t size,
                                         TIFF *tif) {
  toff_t offset = 0;
  if (TIFFGetField(tif, TIFFTAG_EXIFIFD, &offset)) {
    DirectoryFuzzProbeEXIFDirectory(data, size, offset);
  }

  offset = 0;
  if (TIFFGetField(tif, TIFFTAG_GPSIFD, &offset)) {
    DirectoryFuzzProbeGPSDirectory(data, size, offset);
  }

  uint16_t subdirectory_count = 0;
  const uint64_t *subdirectories = nullptr;
  if (!TIFFGetField(tif, TIFFTAG_SUBIFD, &subdirectory_count, &subdirectories) ||
      subdirectories == nullptr) {
    return;
  }

  const uint16_t limit =
      subdirectory_count < kMaxSubDirectories ? subdirectory_count
                                              : kMaxSubDirectories;
  for (uint16_t index = 0; index < limit; ++index) {
    DirectoryFuzzProbeSubDirectory(data, size,
                                   static_cast<toff_t>(subdirectories[index]));
  }
}

void DirectoryFuzzVisitCurrentDirectory(const uint8_t *data, size_t size,
                                        TIFF *tif) {
  uint32_t width = 0;
  uint32_t height = 0;
  uint16_t compression = 0;
  uint16_t bits_per_sample = 0;
  uint16_t samples_per_pixel = 0;
  uint16_t photometric = 0;
  uint16_t planar_config = 0;
  uint16_t orientation = 0;

  (void)TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
  (void)TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
  (void)TIFFGetFieldDefaulted(tif, TIFFTAG_COMPRESSION, &compression);
  (void)TIFFGetFieldDefaulted(tif, TIFFTAG_BITSPERSAMPLE, &bits_per_sample);
  (void)TIFFGetFieldDefaulted(tif, TIFFTAG_SAMPLESPERPIXEL, &samples_per_pixel);
  (void)TIFFGetFieldDefaulted(tif, TIFFTAG_PHOTOMETRIC, &photometric);
  (void)TIFFGetFieldDefaulted(tif, TIFFTAG_PLANARCONFIG, &planar_config);
  (void)TIFFGetFieldDefaulted(tif, TIFFTAG_ORIENTATION, &orientation);

  (void)TIFFCurrentDirOffset(tif);
  (void)TIFFNumberOfStrips(tif);
  (void)TIFFNumberOfTiles(tif);
  (void)TIFFIsTiled(tif);

  DirectoryFuzzEnumerateTags(tif);

  char rgba_error[1024];
  (void)TIFFRGBAImageOK(tif, rgba_error);

  /* Reopen linked directories separately so the main IFD walk stays linear. */
  DirectoryFuzzProbeLinkedDirectories(data, size, tif);
}

void DirectoryFuzzProbeEXIFDirectory(const uint8_t *data, size_t size,
                                     toff_t offset) {
  DirectoryFuzzMemoryFile file = {data, size, 0};
  TIFF *tif = DirectoryFuzzOpen(&file);
  if (tif == nullptr) {
    return;
  }

  if (TIFFReadEXIFDirectory(tif, offset)) {
    DirectoryFuzzEnumerateTags(tif);
  }
  TIFFClose(tif);
}

void DirectoryFuzzProbeGPSDirectory(const uint8_t *data, size_t size,
                                    toff_t offset) {
  DirectoryFuzzMemoryFile file = {data, size, 0};
  TIFF *tif = DirectoryFuzzOpen(&file);
  if (tif == nullptr) {
    return;
  }

  if (TIFFReadGPSDirectory(tif, offset)) {
    DirectoryFuzzEnumerateTags(tif);
  }
  TIFFClose(tif);
}

void DirectoryFuzzProbeSubDirectory(const uint8_t *data, size_t size,
                                    toff_t offset) {
  DirectoryFuzzMemoryFile file = {data, size, 0};
  TIFF *tif = DirectoryFuzzOpen(&file);
  if (tif == nullptr) {
    return;
  }

  if (TIFFSetSubDirectory(tif, offset)) {
    DirectoryFuzzEnumerateTags(tif);
    char rgba_error[1024];
    (void)TIFFRGBAImageOK(tif, rgba_error);
  }
  TIFFClose(tif);
}

}  // namespace

extern "C" tmsize_t DirectoryFuzzRead(thandle_t handle, void *buf, tmsize_t size) {
  if (size <= 0) {
    return 0;
  }

  DirectoryFuzzMemoryFile *file = static_cast<DirectoryFuzzMemoryFile *>(handle);
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

extern "C" tmsize_t DirectoryFuzzWrite(thandle_t, void *, tmsize_t) {
  return 0;
}

extern "C" toff_t DirectoryFuzzSeek(thandle_t handle, toff_t offset,
                                     int whence) {
  DirectoryFuzzMemoryFile *file = static_cast<DirectoryFuzzMemoryFile *>(handle);
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

extern "C" int DirectoryFuzzClose(thandle_t) {
  return 0;
}

extern "C" toff_t DirectoryFuzzSize(thandle_t handle) {
  const DirectoryFuzzMemoryFile *file =
      static_cast<const DirectoryFuzzMemoryFile *>(handle);
  return static_cast<toff_t>(file->size);
}

namespace {

TIFF *DirectoryFuzzOpen(DirectoryFuzzMemoryFile *file) {
  return TIFFClientOpen("<memory>", "r", reinterpret_cast<thandle_t>(file),
                        DirectoryFuzzRead, DirectoryFuzzWrite,
                        DirectoryFuzzSeek, DirectoryFuzzClose,
                        DirectoryFuzzSize, nullptr, nullptr);
}

}  // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  if (size < 4) {
    return 0;
  }

  TIFFSetErrorHandler(DirectoryFuzzIgnoreHandler);
  TIFFSetWarningHandler(DirectoryFuzzIgnoreHandler);

  DirectoryFuzzMemoryFile file = {data, size, 0};
  TIFF *tif = DirectoryFuzzOpen(&file);
  if (tif == nullptr) {
    return 0;
  }

  for (uint32_t dir = 0; dir < kMaxDirectories; ++dir) {
    DirectoryFuzzVisitCurrentDirectory(data, size, tif);
    if (TIFFLastDirectory(tif) || !TIFFReadDirectory(tif)) {
      break;
    }
  }

  TIFFClose(tif);
  return 0;
}
