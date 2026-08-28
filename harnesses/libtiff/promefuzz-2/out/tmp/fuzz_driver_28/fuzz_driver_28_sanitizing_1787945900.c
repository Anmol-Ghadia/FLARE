#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
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
    if (!fp) return (uint64_t)-1;
    if (fseek(fp, (long)off, whence) != 0) return (uint64_t)-1;
    {
        long pos = ftell(fp);
        if (pos < 0) return (uint64_t)-1;
        return (uint64_t)pos;
    }
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
    if (end < 0) {
        (void)fseek(fp, cur, SEEK_SET);
        return 0;
    }
    (void)fseek(fp, cur, SEEK_SET);
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
    FILE *fp;
    TIFF *tif;
    thandle_t old_cd, cur_cd;
    TIFFCloseProc closeproc;
    int old_fd;
    int new_fd = -1;
    int mode_selector = 0;
    int do_cleanup = 0;
    int do_close = 0;
    char mode[4] = "r";

    fp = fopen("./dummy_file", "wb");
    if (!fp) return 0;
    if (Size > 0) (void)fwrite(Data, 1, Size, fp);
    fclose(fp);

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

    fp = fopen("./dummy_file", mode_selector >= 2 ? "w+b" : "r+b");
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

    old_cd = TIFFSetClientdata(tif, (thandle_t)(uintptr_t)Size);
    (void)old_cd;
    cur_cd = TIFFClientdata(tif);
    (void)cur_cd;

    if (Size > 1) {
        thandle_t fuzz_ptr = (thandle_t)(uintptr_t)(
            ((uintptr_t)Data[1] << 24) ^
            ((uintptr_t)(Size & 0xffff) << 8) ^
            (uintptr_t)mode_selector);
        old_cd = TIFFSetClientdata(tif, fuzz_ptr);
        (void)old_cd;
        cur_cd = TIFFClientdata(tif);
        (void)cur_cd;
    }

    if (Size > 5) {
        new_fd = (int)((Data[2] << 24) | (Data[3] << 16) | (Data[4] << 8) | Data[5]);
    } else if (Size > 2) {
        new_fd = (int)Data[2];
    }
    old_fd = TIFFSetFileno(tif, new_fd);
    (void)old_fd;

    if (Size > 6) {
        old_fd = TIFFSetFileno(tif, (int)(unsigned char)Data[6]);
        (void)old_fd;
    }

    closeproc = TIFFGetCloseProc(tif);
    (void)closeproc;
    cur_cd = TIFFClientdata(tif);
    (void)cur_cd;

    do_cleanup = (Size > 7) ? (Data[7] & 1) : 0;
    do_close = (Size > 8) ? (Data[8] & 1) : 1;

    if (do_cleanup) {
        TIFFCleanup(tif);
        if (fp) fclose(fp);
        return 0;
    }

    if (do_close) {
        TIFFClose(tif);
        return 0;
    }

    TIFFClose(tif);
    return 0;
}