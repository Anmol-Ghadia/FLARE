#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "tiffio.h"

static void write_input_file(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) return;
    if (Size > 0) {
        (void)fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
}

static uint64_t read_u64(const uint8_t *p, size_t n) {
    uint64_t v = 0;
    size_t i;
    size_t lim = n < 8 ? n : 8;
    for (i = 0; i < lim; i++) {
        v |= ((uint64_t)p[i]) << (8 * i);
    }
    return v;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    TIFF *tif = NULL;
    uint64_t diroff = 0;
    uint64_t out_diroff = 0;
    int mode_selector = 0;
    const char *open_mode = "r";
    TIFFTagMethods *tag_methods;
    TIFFFieldArray custom_info;
    TIFFField custom_field;
    int did_cleanup = 0;

    write_input_file(Data, Size);

    if (Size > 0) {
        mode_selector = Data[0] % 4;
    }

    switch (mode_selector) {
        case 0: open_mode = "r"; break;
        case 1: open_mode = "rM"; break;
        case 2: open_mode = "w"; break;
        default: open_mode = "w8"; break;
    }

    tif = TIFFOpen("./dummy_file", open_mode);
    if (!tif) {
        tif = TIFFOpen("./dummy_file", "r+");
    }
    if (!tif) {
        tif = TIFFOpen("./dummy_file", "w");
    }
    if (!tif) {
        return 0;
    }

    tag_methods = TIFFAccessTagMethods(tif);
    (void)tag_methods;

    if (Size >= 9) {
        diroff = read_u64(Data + 1, Size - 1);
    } else {
        diroff = 0;
    }

    memset(&custom_field, 0, sizeof(custom_field));
    memset(&custom_info, 0, sizeof(custom_info));

    custom_field.field_tag = (Size > 9) ? (uint32_t)Data[9] : 0;
    custom_field.field_readcount = 1;
    custom_field.field_writecount = 1;
    custom_field.field_name = (char *)"FuzzCustomField";
    custom_field.field_subfields = NULL;

    custom_info.type = (TIFFFieldArrayType)0;
    custom_info.allocated_size = 0;
    custom_info.count = 1;
    custom_info.fields = &custom_field;

    (void)TIFFReadCustomDirectory(tif, (toff_t)diroff, &custom_info);
    (void)TIFFReadEXIFDirectory(tif, (toff_t)diroff);

    TIFFFreeDirectory(tif);

    if (mode_selector >= 2) {
        (void)TIFFWriteCustomDirectory(tif, &out_diroff);
        if (Size > 10) {
            (void)TIFFReadCustomDirectory(tif, (toff_t)out_diroff, &custom_info);
            (void)TIFFReadEXIFDirectory(tif, (toff_t)out_diroff);
        }
        TIFFFreeDirectory(tif);
    }

    if (Size > 16) {
        (void)TIFFWriteCustomDirectory(tif, &out_diroff);
        TIFFFreeDirectory(tif);
    }

    TIFFCleanup(tif);
    did_cleanup = 1;

    if (!did_cleanup) {
        TIFFClose(tif);
    }

    return 0;
}