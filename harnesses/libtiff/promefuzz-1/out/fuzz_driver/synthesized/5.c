// This fuzz driver is generated for library libtiff, aiming to fuzz the following functions:
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFFindField at tif_dirinfo.c:680:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFScanlineSize at tif_strip.c:307:1 in tiffio.h
// TIFFWriteScanline at tif_write.c:47:1 in tiffio.h
// TIFFWriteDirectory at tif_dirwrite.c:194:1 in tiffio.h
// TIFFCreateGPSDirectory at tif_dir.c:1410:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFWriteCustomDirectory at tif_dirwrite.c:263:1 in tiffio.h
// TIFFClose at tif_close.c:121:1 in tiffio.h
// TIFFOpen at tif_unix.c:223:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
// TIFFSetField at tif_dir.c:828:1 in tiffio.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "tiffio.h"

static uint16_t rd16(const uint8_t *p, size_t n, size_t off) {
    if (off + 1 >= n) return 0;
    return (uint16_t)(p[off] | ((uint16_t)p[off + 1] << 8));
}

static uint32_t rd32(const uint8_t *p, size_t n, size_t off) {
    if (off + 3 >= n) return 0;
    return (uint32_t)p[off] |
           ((uint32_t)p[off + 1] << 8) |
           ((uint32_t)p[off + 2] << 16) |
           ((uint32_t)p[off + 3] << 24);
}

static float rdfloat(const uint8_t *p, size_t n, size_t off) {
    uint32_t v = rd32(p, n, off);
    float f;
    memcpy(&f, &v, sizeof(f));
    return f;
}

int LLVMFuzzerTestOneInput_5(const uint8_t *Data, size_t Size) {
    FILE *fp;
    TIFF *tif;
    uint32_t width, height, rowsperstrip;
    uint16_t bitspersample, samplesperpixel, compression, photometric, planarconfig;
    uint16_t orientation, fillorder, resolutionunit, sampleformat;
    uint16_t extrasamples_count, predictor;
    uint16_t extrasample_value;
    uint16_t inkset;
    float xres, yres;
    uint16_t page_number[2];
    uint16_t halftonehints[2];
    uint16_t dotrange[2];
    uint16_t subfiletype16;
    uint32_t row;
    uint8_t *scanline = NULL;
    tmsize_t scanline_size;
    uint64_t custom_dir_offset = 0;
    const TIFFField *field;
    double gps_lat[3], gps_lon[3], gps_time[3];
    char gps_lat_ref[2], gps_lon_ref[2], gps_status[2], gps_measure_mode[2];
    char gps_satellites[32], gps_date[16], gps_mapdatum[32];
    uint16_t gps_differential;
    float gps_dop, gps_speed, gps_track, gps_imgdir, gps_altitude;
    uint8_t gps_altitude_ref;
    uint8_t gps_version[4];
    uint32_t sample_for_write;

    fp = fopen("./dummy_file", "wb");
    if (!fp) return 0;
    if (Size > 0) {
        fwrite(Data, 1, Size, fp);
    }
    fclose(fp);

    tif = TIFFOpen("./dummy_file", "w");
    if (!tif) return 0;

    width = (rd32(Data, Size, 0) % 64U) + 1U;
    height = (rd32(Data, Size, 4) % 64U) + 1U;
    bitspersample = (uint16_t)(((rd16(Data, Size, 8) % 4U) + 1U) * 8U);
    samplesperpixel = (uint16_t)((rd16(Data, Size, 10) % 4U) + 1U);
    compression = (rd16(Data, Size, 12) % 2U) ? COMPRESSION_NONE : COMPRESSION_LZW;
    photometric = (samplesperpixel >= 3) ? PHOTOMETRIC_RGB : PHOTOMETRIC_MINISBLACK;
    planarconfig = (rd16(Data, Size, 14) & 1U) ? PLANARCONFIG_SEPARATE : PLANARCONFIG_CONTIG;
    orientation = (uint16_t)((rd16(Data, Size, 16) % 8U) + 1U);
    fillorder = (rd16(Data, Size, 18) & 1U) ? FILLORDER_MSB2LSB : FILLORDER_LSB2MSB;
    resolutionunit = (uint16_t)((rd16(Data, Size, 20) % 3U) + 1U);
    sampleformat = (uint16_t)(((rd16(Data, Size, 22) % 3U) == 0U) ? SAMPLEFORMAT_UINT :
                              ((rd16(Data, Size, 22) % 3U) == 1U) ? SAMPLEFORMAT_INT :
                                                                     SAMPLEFORMAT_IEEEFP);
    rowsperstrip = (rd32(Data, Size, 24) % height) + 1U;
    predictor = (compression == COMPRESSION_LZW) ? (uint16_t)((rd16(Data, Size, 28) % 2U) + 1U) : 1U;
    extrasamples_count = (samplesperpixel > 1) ? 1U : 0U;
    extrasample_value = EXTRASAMPLE_ASSOCALPHA;
    xres = rdfloat(Data, Size, 32);
    yres = rdfloat(Data, Size, 36);
    if (!(xres > 0.0f && xres < 100000.0f)) xres = 72.0f;
    if (!(yres > 0.0f && yres < 100000.0f)) yres = 72.0f;
    page_number[0] = rd16(Data, Size, 40);
    page_number[1] = rd16(Data, Size, 42);
    halftonehints[0] = rd16(Data, Size, 44);
    halftonehints[1] = rd16(Data, Size, 46);
    dotrange[0] = rd16(Data, Size, 48);
    dotrange[1] = rd16(Data, Size, 50);
    subfiletype16 = rd16(Data, Size, 52);
    inkset = INKSET_CMYK;
    (void)inkset;
    (void)predictor;

    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, bitspersample);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, samplesperpixel);
    TIFFSetField(tif, TIFFTAG_COMPRESSION, compression);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, photometric);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, planarconfig);
    TIFFSetField(tif, TIFFTAG_ORIENTATION, orientation);
    TIFFSetField(tif, TIFFTAG_FILLORDER, fillorder);
    TIFFSetField(tif, TIFFTAG_XRESOLUTION, xres);
    TIFFSetField(tif, TIFFTAG_YRESOLUTION, yres);
    TIFFSetField(tif, TIFFTAG_RESOLUTIONUNIT, resolutionunit);
    TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, rowsperstrip);
    TIFFSetField(tif, TIFFTAG_SAMPLEFORMAT, sampleformat);
    TIFFSetField(tif, TIFFTAG_PAGENUMBER, page_number[0], page_number[1]);
    TIFFSetField(tif, TIFFTAG_SUBFILETYPE, (uint32_t)subfiletype16);

    field = TIFFFindField(tif, TIFFTAG_GPSIFD, TIFF_ANY);
    (void)field;

    TIFFSetField(tif, TIFFTAG_HALFTONEHINTS, halftonehints[0], halftonehints[1]);
    if (samplesperpixel > 1) {
        TIFFSetField(tif, TIFFTAG_EXTRASAMPLES, extrasamples_count, &extrasample_value);
    } else {
        TIFFSetField(tif, TIFFTAG_DOTRANGE, dotrange[0], dotrange[1]);
    }

    scanline_size = TIFFScanlineSize(tif);
    if (scanline_size <= 0) {
        scanline_size = (tmsize_t)(((uint64_t)width * samplesperpixel * bitspersample + 7U) / 8U);
        if (scanline_size <= 0) scanline_size = 1;
    }

    scanline = (uint8_t *)malloc((size_t)scanline_size);
    if (scanline) {
        for (row = 0; row < height; row++) {
            size_t i;
            for (i = 0; i < (size_t)scanline_size; i++) {
                scanline[i] = (uint8_t)(Size ? Data[(i + row) % Size] ^ (uint8_t)row : (uint8_t)row);
            }
            sample_for_write = 0;
            if (planarconfig == PLANARCONFIG_SEPARATE && samplesperpixel > 0) {
                sample_for_write = row % samplesperpixel;
            }
            TIFFWriteScanline(tif, scanline, row, (uint16_t)sample_for_write);
        }
    }

    TIFFWriteDirectory(tif);

    TIFFCreateGPSDirectory(tif);

    gps_lat_ref[0] = (Size > 0 && (Data[0] & 1U)) ? 'S' : 'N';
    gps_lat_ref[1] = '\0';
    gps_lon_ref[0] = (Size > 1 && (Data[1] & 1U)) ? 'W' : 'E';
    gps_lon_ref[1] = '\0';
    gps_status[0] = (Size > 2 && (Data[2] & 1U)) ? 'V' : 'A';
    gps_status[1] = '\0';
    gps_measure_mode[0] = (Size > 3 && (Data[3] & 1U)) ? '2' : '3';
    gps_measure_mode[1] = '\0';

    snprintf(gps_satellites, sizeof(gps_satellites), "%u", (unsigned)(rd16(Data, Size, 54) % 32U));
    snprintf(gps_date, sizeof(gps_date), "%04u:%02u:%02u",
             (unsigned)(2000U + (rd16(Data, Size, 56) % 50U)),
             (unsigned)((rd16(Data, Size, 58) % 12U) + 1U),
             (unsigned)((rd16(Data, Size, 60) % 28U) + 1U));
    snprintf(gps_mapdatum, sizeof(gps_mapdatum), "DATUM%u", (unsigned)(rd16(Data, Size, 62) % 1000U));

    gps_lat[0] = (double)(rd16(Data, Size, 64) % 90U);
    gps_lat[1] = (double)(rd16(Data, Size, 66) % 60U);
    gps_lat[2] = (double)(rd16(Data, Size, 68) % 60U);
    gps_lon[0] = (double)(rd16(Data, Size, 70) % 180U);
    gps_lon[1] = (double)(rd16(Data, Size, 72) % 60U);
    gps_lon[2] = (double)(rd16(Data, Size, 74) % 60U);
    gps_time[0] = (double)(rd16(Data, Size, 76) % 24U);
    gps_time[1] = (double)(rd16(Data, Size, 78) % 60U);
    gps_time[2] = (double)(rd16(Data, Size, 80) % 60U);

    gps_dop = rdfloat(Data, Size, 84);
    gps_speed = rdfloat(Data, Size, 88);
    gps_track = rdfloat(Data, Size, 92);
    gps_imgdir = rdfloat(Data, Size, 96);
    gps_altitude = rdfloat(Data, Size, 100);
    if (!(gps_dop >= 0.0f && gps_dop < 100000.0f)) gps_dop = 1.0f;
    if (!(gps_speed >= 0.0f && gps_speed < 100000.0f)) gps_speed = 0.0f;
    if (!(gps_track >= 0.0f && gps_track < 360.0f)) gps_track = 0.0f;
    if (!(gps_imgdir >= 0.0f && gps_imgdir < 360.0f)) gps_imgdir = 0.0f;
    if (!(gps_altitude >= 0.0f && gps_altitude < 100000.0f)) gps_altitude = 0.0f;
    gps_altitude_ref = (Size > 4 && (Data[4] & 1U)) ? 1U : 0U;
    gps_differential = (uint16_t)(rd16(Data, Size, 104) % 2U);
    (void)gps_differential;

    gps_version[0] = (Size > 0) ? Data[0] : 0;
    gps_version[1] = (Size > 1) ? Data[1] : 0;
    gps_version[2] = (Size > 2) ? Data[2] : 0;
    gps_version[3] = (Size > 3) ? Data[3] : 0;

    TIFFSetField(tif, GPSTAG_VERSIONID, gps_version);
    TIFFSetField(tif, GPSTAG_LATITUDEREF, gps_lat_ref);
    TIFFSetField(tif, GPSTAG_LATITUDE, gps_lat);
    TIFFSetField(tif, GPSTAG_LONGITUDEREF, gps_lon_ref);
    TIFFSetField(tif, GPSTAG_LONGITUDE, gps_lon);
    TIFFSetField(tif, GPSTAG_ALTITUDEREF, gps_altitude_ref);
    TIFFSetField(tif, GPSTAG_ALTITUDE, (double)gps_altitude);
    TIFFSetField(tif, GPSTAG_TIMESTAMP, gps_time);
    TIFFSetField(tif, GPSTAG_STATUS, gps_status);
    TIFFSetField(tif, GPSTAG_MEASUREMODE, gps_measure_mode);
    TIFFSetField(tif, GPSTAG_DOP, (double)gps_dop);
    TIFFSetField(tif, GPSTAG_SPEEDREF, "K");
    TIFFSetField(tif, GPSTAG_SPEED, (double)gps_speed);
    TIFFSetField(tif, GPSTAG_TRACKREF, "T");
    TIFFSetField(tif, GPSTAG_TRACK, (double)gps_track);
    TIFFSetField(tif, GPSTAG_IMGDIRECTIONREF, "T");
    TIFFSetField(tif, GPSTAG_IMGDIRECTION, (double)gps_imgdir);
    TIFFSetField(tif, GPSTAG_MAPDATUM, gps_mapdatum);
    TIFFSetField(tif, GPSTAG_SATELLITES, gps_satellites);
    TIFFSetField(tif, GPSTAG_DATESTAMP, gps_date);

    TIFFWriteCustomDirectory(tif, &custom_dir_offset);

    free(scanline);
    TIFFClose(tif);
    remove("./dummy_file");
    return 0;
}