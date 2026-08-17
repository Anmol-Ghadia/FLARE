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
#include <limits>
#include <vector>

#include <tiffio.h>

namespace {

struct MemoryFile {
    const uint8_t *data;
    toff_t size;
    toff_t offset;
};

constexpr uint64_t kMaxDirectories = 4;
constexpr uint64_t kMaxDecodedStrileBytes = 64ULL * 1024ULL * 1024ULL;
constexpr uint64_t kMaxRasterBytes = 256ULL * 1024ULL * 1024ULL;

extern "C" void IgnoreHandler(const char *, const char *, va_list) {}

tmsize_t MemoryRead(thandle_t handle, void *buf, tmsize_t size) {
    MemoryFile *file = static_cast<MemoryFile *>(handle);
    if (size <= 0 || file->offset >= file->size) {
        return 0;
    }

    uint64_t bytes = static_cast<uint64_t>(size);
    const uint64_t available = file->size - file->offset;
    if (bytes > available) {
        bytes = available;
    }

    std::memcpy(buf, file->data + static_cast<size_t>(file->offset),
                static_cast<size_t>(bytes));
    file->offset += bytes;
    return static_cast<tmsize_t>(bytes);
}

tmsize_t MemoryWrite(thandle_t, void *, tmsize_t) {
    return 0;
}

toff_t MemorySeek(thandle_t handle, toff_t offset, int whence) {
    MemoryFile *file = static_cast<MemoryFile *>(handle);
    uint64_t base = 0;
    switch (whence) {
        case SEEK_SET:
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

    if (offset > std::numeric_limits<uint64_t>::max() - base) {
        return static_cast<toff_t>(-1);
    }

    file->offset = base + offset;
    return file->offset;
}

int MemoryClose(thandle_t) {
    return 0;
}

toff_t MemorySize(thandle_t handle) {
    return static_cast<MemoryFile *>(handle)->size;
}

bool SafeMultiply(uint64_t lhs, uint64_t rhs, uint64_t limit, uint64_t *result) {
    if (lhs == 0 || rhs == 0) {
        *result = 0;
        return true;
    }

    if (lhs > limit / rhs) {
        return false;
    }

    *result = lhs * rhs;
    return true;
}

TIFF *OpenTiff(MemoryFile *file) {
    return TIFFClientOpen("memory.tif", "rmO", static_cast<thandle_t>(file),
                          MemoryRead, MemoryWrite, MemorySeek, MemoryClose,
                          MemorySize, nullptr, nullptr);
}

void ProcessDirectory(TIFF *tif) {
    uint32_t width = 0;
    uint32_t height = 0;
    if (!TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width) ||
        !TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height) || width == 0 ||
        height == 0) {
        return;
    }

    const uint64_t decoded_strile_bytes =
        TIFFIsTiled(tif) ? TIFFTileSize64(tif) : TIFFStripSize64(tif);
    if (decoded_strile_bytes == 0 ||
        decoded_strile_bytes > kMaxDecodedStrileBytes) {
        return;
    }

    if (width > kMaxRasterBytes / (2U * sizeof(uint32_t))) {
        return;
    }

    uint64_t pixel_count = 0;
    if (!SafeMultiply(width, height, kMaxRasterBytes / sizeof(uint32_t),
                      &pixel_count) ||
        pixel_count == 0) {
        return;
    }

    char errmsg[1024];
    if (!TIFFRGBAImageOK(tif, errmsg)) {
        return;
    }

    std::vector<uint32_t> raster(static_cast<size_t>(pixel_count));
    TIFFReadRGBAImageOriented(tif, width, height, raster.data(),
                              ORIENTATION_TOPLEFT, 0);
}

}  // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    TIFFSetErrorHandler(IgnoreHandler);
    TIFFSetWarningHandler(IgnoreHandler);

    MemoryFile file = {data, static_cast<toff_t>(size), 0};
    TIFF *tif = OpenTiff(&file);
    if (tif == nullptr) {
        return 0;
    }

    for (uint64_t directory = 0; directory < kMaxDirectories; ++directory) {
        ProcessDirectory(tif);
        if (!TIFFReadDirectory(tif)) {
            break;
        }
    }

    TIFFClose(tif);
    return 0;
}
