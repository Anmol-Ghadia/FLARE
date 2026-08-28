#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "tiffio.h"

static uint16_t rd16(const uint8_t **p, size_t *n) {
    uint16_t v = 0;
    size_t m = (*n < 2) ? *n : 2;
    if (m >= 1) v |= (uint16_t)(*p)[0];
    if (m >= 2) v |= (uint16_t)((uint16_t)(*p)[1] << 8);
    *p += m;
    *n -= m;
    return v;
}

static uint32_t rd32(const uint8_t **p, size_t *n) {
    uint32_t v = 0;
    size_t i, m = (*n < 4) ? *n : 4;
    for (i = 0; i < m; i++) {
        v |= ((uint32_t)(*p)[i]) << (8 * i);
    }
    *p += m;
    *n -= m;
    return v;
}

static float rdfloat(const uint8_t **p, size_t *n) {
    uint32_t u = rd32(p, n);
    float f = 0.0f;
    memcpy(&f, &u, sizeof(f));
    return f;
}

static double rddouble(const uint8_t **p, size_t *n) {
    uint64_t u = 0;
    size_t i, m = (*n < 8) ? *n : 8;
    for (i = 0; i < m; i++) {
        u |= ((uint64_t)(*p)[i]) << (8 * i);
    }
    *p += m;
    *n -= m;
    double d = 0.0;
    memcpy(&d, &u, sizeof(d));
    return d;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    const uint8_t *p = Data;
    size_t n = Size;
    FILE *fp;
    TIFF *tif;
    uint32_t width, height;
    uint16_t spp, bps, photo, planar, compression, orientation, fillorder, resunit;
    float xres, yres;
    uint16_t rowsperstrip;
    char software[32], artist[32], datetime[32], make[32], hostcomputer[32];
    size_t copy;
    tmsize_t scanline;
    uint64_t dir_offset = 0;

    fp = fopen("./dummy_file", "wb");
    if (!fp) return 0;
    fclose(fp);

    tif = TIFFOpen("./dummy_file", "w");
    if (!tif) {
        remove("./dummy_file");
        return 0;
    }

    width = (rd32(&p, &n) % 64U) + 1U;
    height = (rd32(&p, &n) % 64U) + 1U;
    spp = (uint16_t)((rd16(&p, &n) % 4U) + 1U);

    {
        static const uint16_t bps_choices[] = {1, 2, 4, 8, 16};
        bps = bps_choices[rd16(&p, &n) % (sizeof(bps_choices) / sizeof(bps_choices[0]))];
    }

    photo = (spp == 1) ? PHOTOMETRIC_MINISBLACK : PHOTOMETRIC_RGB;
    planar = (rd16(&p, &n) & 1U) ? PLANARCONFIG_CONTIG : PLANARCONFIG_SEPARATE;

    {
        static const uint16_t compression_choices[] = {
            COMPRESSION_NONE, COMPRESSION_LZW, COMPRESSION_PACKBITS, COMPRESSION_DEFLATE
        };
        compression = compression_choices[rd16(&p, &n) % (sizeof(compression_choices) / sizeof(compression_choices[0]))];
    }

    orientation = (uint16_t)((rd16(&p, &n) % 8U) + 1U);
    fillorder = (rd16(&p, &n) & 1U) ? FILLORDER_MSB2LSB : FILLORDER_LSB2MSB;

    {
        static const uint16_t resunit_choices[] = {RESUNIT_NONE, RESUNIT_INCH, RESUNIT_CENTIMETER};
        resunit = resunit_choices[rd16(&p, &n) % 3U];
    }

    xres = rdfloat(&p, &n);
    yres = rdfloat(&p, &n);
    if (!(xres > 0.0f) || xres != xres) xres = 72.0f;
    if (!(yres > 0.0f) || yres != yres) yres = 72.0f;

    rowsperstrip = (uint16_t)(((rd32(&p, &n) % height) + 1U) & 0xFFFFU);
    if (rowsperstrip == 0) rowsperstrip = 1;

    memset(software, 0, sizeof(software));
    memset(artist, 0, sizeof(artist));
    memset(datetime, 0, sizeof(datetime));
    memset(make, 0, sizeof(make));
    memset(hostcomputer, 0, sizeof(hostcomputer));

    copy = n < sizeof(software) - 1 ? n : sizeof(software) - 1;
    memcpy(software, p, copy);
    copy = n < sizeof(artist) - 1 ? n : sizeof(artist) - 1;
    memcpy(artist, p, copy);
    copy = n < sizeof(make) - 1 ? n : sizeof(make) - 1;
    memcpy(make, p, copy);
    copy = n < sizeof(hostcomputer) - 1 ? n : sizeof(hostcomputer) - 1;
    memcpy(hostcomputer, p, copy);
    snprintf(datetime, sizeof(datetime), "2024:01:%02u 00:00:%02u",
             (unsigned)((width % 28U) + 1U), (unsigned)(height % 60U));

    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, bps);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, spp);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, photo);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, planar);
    TIFFSetField(tif, TIFFTAG_COMPRESSION, compression);
    TIFFSetField(tif, TIFFTAG_ORIENTATION, orientation);
    TIFFSetField(tif, TIFFTAG_FILLORDER, fillorder);
    TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, (uint32_t)rowsperstrip);
    TIFFSetField(tif, TIFFTAG_XRESOLUTION, xres);
    TIFFSetField(tif, TIFFTAG_YRESOLUTION, yres);
    TIFFSetField(tif, TIFFTAG_RESOLUTIONUNIT, resunit);
    TIFFSetField(tif, TIFFTAG_SOFTWARE, software);
    TIFFSetField(tif, TIFFTAG_ARTIST, artist);
    TIFFSetField(tif, TIFFTAG_DATETIME, datetime);

    (void)TIFFFindField(tif, TIFFTAG_IMAGEWIDTH, TIFF_ANY);

    TIFFSetField(tif, TIFFTAG_MAKE, make);
    TIFFSetField(tif, TIFFTAG_HOSTCOMPUTER, hostcomputer);

    scanline = TIFFScanlineSize(tif);
    if (scanline > 0 && scanline < (1 << 20)) {
        unsigned char *buf = (unsigned char *)_TIFFmalloc(scanline);
        if (buf) {
            if (n > 0) {
                size_t i;
                for (i = 0; i < (size_t)scanline; i++) {
                    buf[i] = p[i % n];
                }
            } else {
                memset(buf, 0, (size_t)scanline);
            }
            {
                uint16_t sample = 0;
                if (planar == PLANARCONFIG_SEPARATE && spp > 0) {
                    sample = (uint16_t)(rd16(&p, &n) % spp);
                }
                (void)TIFFWriteScanline(tif, buf, 0, sample);
            }
            _TIFFfree(buf);
        }
    }

    (void)TIFFWriteDirectory(tif);

    if (TIFFCreateGPSDirectory(tif)) {
        uint8_t gpsversion[4];
        char gpslatref[2];
        char gpslonref[2];
        double gpslatitude[3];
        double gpslongitude[3];
        double gpstimestamp[3];
        double gpsalt;
        double gpsspeed;
        double gpstrack;
        uint16_t gpsaltref;
        char gpsstatus[2];
        char gpsmeasuremode[2];
        char gpsspeedref[2];
        char gpstrackref[2];
        char gpsmapdatum[16];
        char gpsdatestamp[16];
        unsigned char gpsprocessing[16];
        uint16_t gpsdifferential;

        gpsversion[0] = (uint8_t)(rd16(&p, &n) & 0xFFU);
        gpsversion[1] = (uint8_t)(rd16(&p, &n) & 0xFFU);
        gpsversion[2] = (uint8_t)(rd16(&p, &n) & 0xFFU);
        gpsversion[3] = (uint8_t)(rd16(&p, &n) & 0xFFU);

        gpslatref[0] = (rd16(&p, &n) & 1U) ? 'N' : 'S';
        gpslatref[1] = '\0';
        gpslonref[0] = (rd16(&p, &n) & 1U) ? 'E' : 'W';
        gpslonref[1] = '\0';

        gpslatitude[0] = (double)(rd16(&p, &n) % 90U);
        gpslatitude[1] = (double)(rd16(&p, &n) % 60U);
        gpslatitude[2] = rddouble(&p, &n);
        if (!(gpslatitude[2] >= 0.0) || gpslatitude[2] != gpslatitude[2]) gpslatitude[2] = 0.0;
        while (gpslatitude[2] >= 60.0) gpslatitude[2] /= 2.0;

        gpslongitude[0] = (double)(rd16(&p, &n) % 180U);
        gpslongitude[1] = (double)(rd16(&p, &n) % 60U);
        gpslongitude[2] = rddouble(&p, &n);
        if (!(gpslongitude[2] >= 0.0) || gpslongitude[2] != gpslongitude[2]) gpslongitude[2] = 0.0;
        while (gpslongitude[2] >= 60.0) gpslongitude[2] /= 2.0;

        gpstimestamp[0] = (double)(rd16(&p, &n) % 24U);
        gpstimestamp[1] = (double)(rd16(&p, &n) % 60U);
        gpstimestamp[2] = (double)(rd16(&p, &n) % 60U);

        gpsalt = (double)rdfloat(&p, &n);
        if (gpsalt != gpsalt) gpsalt = 0.0;
        gpsaltref = (uint16_t)(rd16(&p, &n) & 1U);

        gpsstatus[0] = (rd16(&p, &n) & 1U) ? 'A' : 'V';
        gpsstatus[1] = '\0';
        gpsmeasuremode[0] = (rd16(&p, &n) & 1U) ? '2' : '3';
        gpsmeasuremode[1] = '\0';
        gpsspeedref[0] = 'K';
        gpsspeedref[1] = '\0';
        gpstrackref[0] = 'T';
        gpstrackref[1] = '\0';

        gpsspeed = (double)rdfloat(&p, &n);
        gpstrack = (double)rdfloat(&p, &n);
        if (gpsspeed != gpsspeed) gpsspeed = 0.0;
        if (gpstrack != gpstrack) gpstrack = 0.0;

        memset(gpsmapdatum, 0, sizeof(gpsmapdatum));
        memset(gpsdatestamp, 0, sizeof(gpsdatestamp));
        memset(gpsprocessing, 0, sizeof(gpsprocessing));

        snprintf(gpsmapdatum, sizeof(gpsmapdatum), "WGS-%u", (unsigned)(rd16(&p, &n) % 100U));
        snprintf(gpsdatestamp, sizeof(gpsdatestamp), "2024:%02u:%02u",
                 (unsigned)((rd16(&p, &n) % 12U) + 1U),
                 (unsigned)((rd16(&p, &n) % 28U) + 1U));

        copy = n < sizeof(gpsprocessing) ? n : sizeof(gpsprocessing);
        memcpy(gpsprocessing, p, copy);

        gpsdifferential = (uint16_t)(rd16(&p, &n) & 1U);

        TIFFSetField(tif, GPSTAG_VERSIONID, gpsversion);
        TIFFSetField(tif, GPSTAG_LATITUDEREF, gpslatref);
        TIFFSetField(tif, GPSTAG_LATITUDE, gpslatitude);
        TIFFSetField(tif, GPSTAG_LONGITUDEREF, gpslonref);
        TIFFSetField(tif, GPSTAG_LONGITUDE, gpslongitude);
        TIFFSetField(tif, GPSTAG_ALTITUDEREF, gpsaltref);
        TIFFSetField(tif, GPSTAG_ALTITUDE, gpsalt);
        TIFFSetField(tif, GPSTAG_TIMESTAMP, gpstimestamp);
        TIFFSetField(tif, GPSTAG_STATUS, gpsstatus);
        TIFFSetField(tif, GPSTAG_MEASUREMODE, gpsmeasuremode);
        TIFFSetField(tif, GPSTAG_SPEEDREF, gpsspeedref);
        TIFFSetField(tif, GPSTAG_SPEED, gpsspeed);
        TIFFSetField(tif, GPSTAG_TRACKREF, gpstrackref);
        TIFFSetField(tif, GPSTAG_TRACK, gpstrack);
        TIFFSetField(tif, GPSTAG_MAPDATUM, gpsmapdatum);
        TIFFSetField(tif, GPSTAG_PROCESSINGMETHOD, (uint32_t)copy, gpsprocessing);
        TIFFSetField(tif, GPSTAG_DATESTAMP, gpsdatestamp);
        TIFFSetField(tif, GPSTAG_DIFFERENTIAL, gpsdifferential);

        (void)TIFFWriteCustomDirectory(tif, &dir_offset);
    }

    TIFFClose(tif);
    remove("./dummy_file");
    return 0;
}