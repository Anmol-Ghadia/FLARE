// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFSetFileName at tif_open.c:523:1 in tiffio.h
// TIFFSetFileno at tif_open.c:543:1 in tiffio.h
// TIFFFileno at tif_open.c:534:1 in tiffio.h
// TIFFSetFileno at tif_open.c:543:1 in tiffio.h
// TIFFSetClientdata at tif_open.c:563:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFFdOpen at tif_unix.c:203:1 in tiffio.h
// TIFFFileno at tif_open.c:534:1 in tiffio.h
// TIFFSetMode at tif_open.c:583:1 in tiffio.h
// TIFFFileno at tif_open.c:534:1 in tiffio.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <tiffio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#if defined(_WIN32)
#include <io.h>
#include <fcntl.h>
#define OPEN _open
#define CLOSE _close
#define DUP _dup
#else
#include <unistd.h>
#include <fcntl.h>
#define OPEN open
#define CLOSE close
#define DUP dup
#endif

static int write_dummy_file(const uint8_t *Data, size_t Size) {
    int fd;
#if defined(_WIN32)
    fd = OPEN("./dummy_file", _O_RDWR | _O_CREAT | _O_TRUNC | _O_BINARY, 0600);
#else
    fd = OPEN("./dummy_file", O_RDWR | O_CREAT | O_TRUNC, 0600);
#endif
    if (fd < 0)
        return -1;

    if (Size > 0) {
        size_t off = 0;
        while (off < Size) {
#if defined(_WIN32)
            int written = _write(fd, Data + off, (unsigned int)((Size - off) > 1U << 20 ? (1U << 20) : (Size - off)));
#else
            ssize_t written = write(fd, Data + off, (Size - off) > (size_t)(1U << 20) ? (size_t)(1U << 20) : (Size - off));
#endif
            if (written <= 0)
                break;
            off += (size_t)written;
        }
    }
    return fd;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    static const char mode_r[] = "r";
    static const char mode_w[] = "w";
    static const char mode_a[] = "a";
    static const char mode_rp[] = "r+";
    static const char mode_wp[] = "w+";
    static const char mode_ap[] = "a+";
    static const char mode_rb[] = "rb";
    static const char mode_wb[] = "wb";
    static const char mode_ab[] = "ab";
    static const char mode_rpb[] = "r+b";
    static const char mode_wpb[] = "w+b";
    static const char mode_apb[] = "a+b";
    static const char *modes[] = {
        mode_r, mode_w, mode_a, mode_rp, mode_wp, mode_ap,
        mode_rb, mode_wb, mode_ab, mode_rpb, mode_wpb, mode_apb
    };

    int base_fd = write_dummy_file(Data, Size);
    if (base_fd < 0)
        return 0;

    for (size_t i = 0; i < sizeof(modes) / sizeof(modes[0]); ++i) {
        int fd = DUP(base_fd);
        if (fd < 0)
            continue;

        TIFF *tif = TIFFFdOpen(fd, "./dummy_file", modes[i]);
        if (!tif) {
            CLOSE(fd);
            continue;
        }

        (void)TIFFFileno(tif);

        {
            int mode_values[6];
            mode_values[0] = 0;
            mode_values[1] = -1;
            mode_values[2] = (int)(Size & 0x7fffffffU);
            mode_values[3] = (Size > 0) ? (int)Data[0] : 1;
            mode_values[4] = (Size > 1) ? (int)((int8_t)Data[1]) : -123;
            mode_values[5] = 0x7fffffff;

            for (size_t j = 0; j < sizeof(mode_values) / sizeof(mode_values[0]); ++j) {
                (void)TIFFSetMode(tif, mode_values[j]);
                (void)TIFFFileno(tif);
            }
        }

        {
            const char *names[5];
            names[0] = "./dummy_file";
            names[1] = "";
            names[2] = "alt_name";
            names[3] = "another/path/name.tif";
            names[4] = (Size > 0 && Data[0] & 1) ? "./dummy_file" : "fuzz_name";

            for (size_t j = 0; j < sizeof(names) / sizeof(names[0]); ++j) {
                (void)TIFFSetFileName(tif, names[j]);
            }
        }

        {
            int fd_values[5];
            fd_values[0] = fd;
            fd_values[1] = base_fd;
            fd_values[2] = -1;
            fd_values[3] = 0;
            fd_values[4] = (Size > 2) ? (int)Data[2] : 1234;

            for (size_t j = 0; j < sizeof(fd_values) / sizeof(fd_values[0]); ++j) {
                (void)TIFFSetFileno(tif, fd_values[j]);
                (void)TIFFFileno(tif);
            }

            (void)TIFFSetFileno(tif, fd);
        }

        {
            thandle_t client_values[5];
            client_values[0] = (thandle_t)0;
            client_values[1] = (thandle_t)tif;
            client_values[2] = (thandle_t)(uintptr_t)fd;
            client_values[3] = (thandle_t)(uintptr_t)Size;
            client_values[4] = (thandle_t)(uintptr_t)((Size > 3) ? Data[3] : 0xAA);

            for (size_t j = 0; j < sizeof(client_values) / sizeof(client_values[0]); ++j) {
                (void)TIFFSetClientdata(tif, client_values[j]);
            }
        }

        TIFFClose(tif);
    }

    CLOSE(base_fd);
    return 0;
}