#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "tiffio.h"

static uint16_t rd16(const uint8_t *p) {
    return (uint16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8));
}

static uint32_t rd32(const uint8_t *p) {
    return (uint32_t)p[0] |
           ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

static float rdfloat(const uint8_t *p) {
    uint32_t v = rd32(p);
    float f;
    memcpy(&f, &v, sizeof(f));
    return f;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) return 0;
    if (Size > 0) fwrite(Data, 1, Size, fp);
    fclose(fp);

    TIFF *tif = TIFFOpen("./dummy_file", "w");
    if (!tif) return 0;

    uint8_t local[256];
    size_t copy_sz = Size < sizeof(local) ? Size : sizeof(local);
    memset(local, 0, sizeof(local));
    if (copy_sz > 0) memcpy(local, Data, copy_sz);

    uint32_t width = (Size >= 4 ? rd32(local) : 1U);
    uint32_t height = (Size >= 8 ? rd32(local + 4) : 1U);
    uint16_t bps = (Size >= 10 ? (uint16_t)(rd16(local + 8) % 32U) : 8U);
    uint16_t spp = (Size >= 12 ? (uint16_t)((rd16(local + 10) % 4U) + 1U) : 1U);
    uint16_t planar = (Size >= 14 && (local[12] & 1)) ? PLANARCONFIG_SEPARATE : PLANARCONFIG_CONTIG;
    uint16_t photometric = (spp >= 3) ? PHOTOMETRIC_RGB : PHOTOMETRIC_MINISBLACK;
    uint16_t compression = COMPRESSION_NONE;
    uint16_t orientation = (Size >= 15) ? (uint16_t)((local[13] % 8U) + 1U) : ORIENTATION_TOPLEFT;
    uint16_t fillorder = (Size >= 16 && (local[14] & 1)) ? FILLORDER_LSB2MSB : FILLORDER_MSB2LSB;
    uint32_t rowsperstrip = (Size >= 20 ? (rd32(local + 16) % 64U) + 1U : 1U);
    float xres = (Size >= 24 ? rdfloat(local + 20) : 72.0f);
    float yres = (Size >= 28 ? rdfloat(local + 24) : 72.0f);
    uint16_t resunit = (Size >= 29) ? (uint16_t)((local[28] % 3U) + 1U) : RESUNIT_INCH;
    uint16_t extrasamples_count = (spp > 1) ? 1 : 0;
    uint16_t extrasamples_val[1] = { EXTRASAMPLE_ASSOCALPHA };
    char software[32];
    char artist[32];
    char datetime[32];
    char make[32];
    char model[32];

    snprintf(software, sizeof(software), "fuzz-%u", (unsigned)(Size & 0xffffffffu));
    snprintf(artist, sizeof(artist), "a%u", (unsigned)((Size > 0) ? local[0] : 0));
    snprintf(datetime, sizeof(datetime), "2024:01:01 00:00:%02u", (unsigned)((Size > 1) ? (local[1] % 60) : 0));
    snprintf(make, sizeof(make), "mk%u", (unsigned)((Size > 2) ? local[2] : 0));
    snprintf(model, sizeof(model), "md%u", (unsigned)((Size > 3) ? local[3] : 0));

    if (width == 0) width = 1;
    if (height == 0) height = 1;
    if (width > 4096) width = 4096;
    if (height > 4096) height = 4096;
    if (bps == 0) bps = 8;
    if (bps != 1 && bps != 2 && bps != 4 && bps != 8 && bps != 16 && bps != 32)
        bps = 8;
    if (!(xres > 0.0f && xres < 100000.0f)) xres = 72.0f;
    if (!(yres > 0.0f && yres < 100000.0f)) yres = 72.0f;

    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, bps);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, spp);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, planar);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, photometric);
    TIFFSetField(tif, TIFFTAG_COMPRESSION, compression);
    TIFFSetField(tif, TIFFTAG_ORIENTATION, orientation);
    TIFFSetField(tif, TIFFTAG_FILLORDER, fillorder);
    TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, rowsperstrip);
    TIFFSetField(tif, TIFFTAG_XRESOLUTION, xres);
    TIFFSetField(tif, TIFFTAG_YRESOLUTION, yres);
    TIFFSetField(tif, TIFFTAG_RESOLUTIONUNIT, resunit);
    TIFFSetField(tif, TIFFTAG_SOFTWARE, software);
    TIFFSetField(tif, TIFFTAG_ARTIST, artist);
    TIFFSetField(tif, TIFFTAG_DATETIME, datetime);

    (void)TIFFFindField(tif, TIFFTAG_IMAGEWIDTH, TIFF_ANY);

    TIFFSetField(tif, TIFFTAG_MAKE, make);
    if (extrasamples_count)
        TIFFSetField(tif, TIFFTAG_EXTRASAMPLES, extrasamples_count, extrasamples_val);
    else
        TIFFSetField(tif, TIFFTAG_MODEL, model);

    {
        tmsize_t scanline = TIFFScanlineSize(tif);
        if (scanline > 0 && scanline < (tmsize_t)(1 << 20)) {
            unsigned char *buf = (unsigned char *)malloc((size_t)scanline);
            if (buf) {
                memset(buf, 0, (size_t)scanline);
                if (Size > 0) {
                    size_t n = (size_t)scanline < Size ? (size_t)scanline : Size;
                    memcpy(buf, Data, n);
                }
                (void)TIFFWriteScanline(tif, buf, 0, 0);
                free(buf);
            }
        }
    }

    (void)TIFFWriteDirectory(tif);

    if (TIFFCreateGPSDirectory(tif)) {
        uint8_t gps_version[4] = {
            (uint8_t)((Size > 0) ? Data[0] : 2),
            (uint8_t)((Size > 1) ? Data[1] : 3),
            (uint8_t)((Size > 2) ? Data[2] : 0),
            (uint8_t)((Size > 3) ? Data[3] : 0)
        };
        char gps_lat_ref[2] = { (Size > 4 && (Data[4] & 1)) ? 'S' : 'N', '\0' };
        char gps_lon_ref[2] = { (Size > 5 && (Data[5] & 1)) ? 'W' : 'E', '\0' };
        char gps_status[2] = { (Size > 6 && (Data[6] & 1)) ? 'V' : 'A', '\0' };
        char gps_measure_mode[2] = { (Size > 7 && (Data[7] & 1)) ? '3' : '2', '\0' };
        char gps_date[] = "2024:01:01";
        char gps_satellites[16];
        char gps_mapdatum[16];
        double lat[3] = {
            (double)((Size > 10) ? (Data[10] % 90) : 0),
            (double)((Size > 11) ? (Data[11] % 60) : 0),
            (double)((Size > 12) ? (Data[12] % 60) : 0)
        };
        double lon[3] = {
            (double)((Size > 13) ? (Data[13] % 180) : 0),
            (double)((Size > 14) ? (Data[14] % 60) : 0),
            (double)((Size > 15) ? (Data[15] % 60) : 0)
        };
        double altitude = (double)((Size > 16) ? Data[16] : 0);
        float dop = (float)((Size > 17) ? Data[17] : 1.0f);
        float speed = (float)((Size > 18) ? Data[18] : 0.0f);
        float track = (float)((Size > 19) ? Data[19] : 0.0f);
        float imgdir = (float)((Size > 20) ? Data[20] : 0.0f);

        snprintf(gps_satellites, sizeof(gps_satellites), "%u", (unsigned)((Size > 8) ? Data[8] : 0));
        snprintf(gps_mapdatum, sizeof(gps_mapdatum), "M%u", (unsigned)((Size > 9) ? Data[9] : 0));

        TIFFSetField(tif, GPSTAG_VERSIONID, gps_version);
        TIFFSetField(tif, GPSTAG_LATITUDEREF, gps_lat_ref);
        TIFFSetField(tif, GPSTAG_LATITUDE, lat);
        TIFFSetField(tif, GPSTAG_LONGITUDEREF, gps_lon_ref);
        TIFFSetField(tif, GPSTAG_LONGITUDE, lon);
        TIFFSetField(tif, GPSTAG_ALTITUDEREF, (uint8_t)((Size > 21) ? (Data[21] & 1) : 0));
        TIFFSetField(tif, GPSTAG_ALTITUDE, altitude);
        TIFFSetField(tif, GPSTAG_STATUS, gps_status);
        TIFFSetField(tif, GPSTAG_MEASUREMODE, gps_measure_mode);
        TIFFSetField(tif, GPSTAG_DOP, dop);
        TIFFSetField(tif, GPSTAG_SPEED, speed);
        TIFFSetField(tif, GPSTAG_TRACK, track);
        TIFFSetField(tif, GPSTAG_IMGDIRECTION, imgdir);
        TIFFSetField(tif, GPSTAG_MAPDATUM, gps_mapdatum);
        TIFFSetField(tif, GPSTAG_SATELLITES, gps_satellites);
        TIFFSetField(tif, GPSTAG_DATESTAMP, gps_date);
        TIFFSetField(tif, GPSTAG_DIFFERENTIAL, (uint16_t)((Size > 22) ? (Data[22] & 1) : 0));

        {
            uint64_t dir_offset = 0;
            (void)TIFFWriteCustomDirectory(tif, &dir_offset);
        }
    }

    TIFFClose(tif);
    return 0;
}