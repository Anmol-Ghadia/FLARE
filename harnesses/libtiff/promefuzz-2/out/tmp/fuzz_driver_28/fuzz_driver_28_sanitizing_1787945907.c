#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "tiffio.h"

static tmsize_t fuzz_read(thandle_t handle, void *buf, tmsize_t size) {
    FILE *fp = (FILE *)handle;
    if (!fp || !buf || size < 0) return 0;
    return (tmsize_t)fread(buf, 1, (size_t)size, fp);
}

static tmsize_t fuzz_write(thandle_t handle, void *buf, tmsize_t size) {
    FILE *fp = (FILE *)handle;
    if (!fp || !buf || size < 0) return 0;
    return (tmsize_t)fwrite(buf, 1, (size_t)size, fp);
}

static uint64_t fuzz_seek(thandle_t handle, uint64_t off, int whence) {
    FILE *fp = (FILE *)handle;
    long pos;
    if (!fp) return (uint64_t)-1;
    if (fseek(fp, (long)off, whence) != 0) return (uint64_t)-1;
    pos = ftell(fp);
    if (pos < 0) return (uint64_t)-1;
    return (uint64_t)pos;
}

static int fuzz_close(thandle_t handle) {
    FILE *fp = (FILE *)handle;
    if (!fp) return 0;
    return fclose(fp);
}

static uint64_t fuzz_size(thandle_t handle) {
    FILE *fp = (FILE *)handle;
    long cur, end;
    if (!fp) return 0;
    cur = ftell(fp);
    if (cur < 0) return 0;
    if (fseek(fp, 0, SEEK_END) != 0) return 0;
    end = ftell(fp);
    (void)fseek(fp, cur, SEEK_SET);
    if (end < 0) return 0;
    return (uint64_t)end;
}

static int fuzz_map(thandle_t handle, void **base, toff_t *size) {
    (void)handle;
    if (base) *base = NULL;
    if (size) *size = 0;
    return 0;
}

static void fuzz_unmap(thandle_t handle, void *base, toff_t size) {
    (void)handle;
    (void)base;
    (void)size;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *seedfp = NULL;
    FILE *fp = NULL;
    TIFF *tif = NULL;
    TIFFCloseProc closeproc;
    thandle_t old_cd, cur_cd;
    int old_fd;
    int new_fd = -1;
    int mode_selector = 0;
    int use_cleanup = 0;
    char mode[4] = "r";

    seedfp = fopen("./dummy_file", "wb");
    if (!seedfp) return 0;
    if (Size > 0) (void)fwrite(Data, 1, Size, seedfp);
    fclose(seedfp);
    seedfp = NULL;

    if (Size > 0) mode_selector = Data[0] % 4;
    switch (mode_selector) {
        case 0:
            strcpy(mode, "r");
            break;
        case 1:
            strcpy(mode, "r+");
            break;
        case 2:
            strcpy(mode, "w");
            break;
        default:
            strcpy(mode, "w+");
            break;
    }

    fp = fopen("./dummy_file", (mode_selector >= 2) ? "w+b" : "r+b");
    if (!fp) {
        fp = fopen("./dummy_file", "rb");
        if (!fp) return 0;
    }

    tif = TIFFClientOpen("./dummy_file", mode, (thandle_t)fp,
                         fuzz_read, fuzz_write, fuzz_seek, fuzz_close,
                         fuzz_size, fuzz_map, fuzz_unmap);
    if (!tif) {
        fclose(fp);
        return 0;
    }

    closeproc = TIFFGetCloseProc(tif);
    (void)closeproc;

    cur_cd = TIFFClientdata(tif);
    (void)cur_cd;

    old_cd = TIFFSetClientdata(tif, (thandle_t)fp);
    (void)old_cd;
    cur_cd = TIFFClientdata(tif);
    (void)cur_cd;

    if (Size > 1) {
        old_cd = TIFFSetClientdata(tif, old_cd ? old_cd : (thandle_t)fp);
        (void)old_cd;
        cur_cd = TIFFClientdata(tif);
        (void)cur_cd;
        (void)TIFFSetClientdata(tif, (thandle_t)fp);
    }

    if (Size > 5) {
        new_fd = (int)((unsigned int)Data[2] << 24 |
                       (unsigned int)Data[3] << 16 |
                       (unsigned int)Data[4] << 8 |
                       (unsigned int)Data[5]);
    } else if (Size > 2) {
        new_fd = (int)Data[2];
    }
    old_fd = TIFFSetFileno(tif, new_fd);
    (void)old_fd;

    if (Size > 6) {
        old_fd = TIFFSetFileno(tif, (int)(signed char)Data[6]);
        (void)old_fd;
    }

    closeproc = TIFFGetCloseProc(tif);
    (void)closeproc;
    cur_cd = TIFFClientdata(tif);
    (void)cur_cd;

    use_cleanup = (Size > 7) ? (Data[7] & 1) : 0;

    if (use_cleanup) {
        (void)TIFFSetClientdata(tif, (thandle_t)fp);
        TIFFCleanup(tif);
        fclose(fp);
        return 0;
    }

    (void)TIFFSetClientdata(tif, (thandle_t)fp);
    TIFFClose(tif);
    return 0;
}