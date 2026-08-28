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

static uint16_t rd16(const uint8_t **p, size_t *n) {
    uint16_t v = 0;
    if (*n >= 2) {
        v = (uint16_t)((*p)[0] | ((uint16_t)(*p)[1] << 8));
        *p += 2;
        *n -= 2;
    } else if (*n == 1) {
        v = (*p)[0];
        *p += 1;
        *n -= 1;
    }
    return v;
}

static uint32_t rd32(const uint8_t **p, size_t *n) {
    uint32_t v = 0;
    size_t i, m = *n < 4 ? *n : 4;
    for (i = 0; i < m; i++) {
        v |= ((uint32_t)(*p)[i]) << (8 * i);
    }
    *p += m;
    *n -= m;
    return v;
}

static float rdfloat(const uint8_t **p, size_t *n) {
    uint32_t u = rd32(p, n);
    float f;
    memcpy(&f, &u, sizeof(f));
    return f;
}

static double rddouble(const uint8_t **p, size_t *n) {
    uint64_t u = 0;
    size_t i, m = *n < 8 ? *n : 8;
    for (i = 0; i < m; i++) {
        u |= ((uint64_t)(*p)[i]) << (8 * i);
    }
    *p += m;
    *n -= m;
    double d;
    memcpy(&d, &u, sizeof(d));
    return d;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    const uint8_t *p = Data;
    size_t n = Size;

    FILE *fp = fopen("./dummy_file", "wb");
    if (!fp) {
        return 0;
    }

    TIFF *tif = TIFFOpen("./dummy_file", "w");
    if (!tif) {
        fclose(fp);
        remove("./dummy_file");
        return 0;
    }

    uint32_t width = (rd32(&p, &n) % 64U) + 1U;
    uint32_t height = (rd32(&p, &n) % 64U) + 1U;
    uint16_t spp = (uint16_t)((rd16(&p, &n) % 4U) + 1U);
    uint16_t bps_choices[] = {1, 2, 4, 8, 16};
    uint16_t bps = bps_choices[rd16(&p, &n) % (sizeof(bps_choices) / sizeof(bps_choices[0]))];
    uint16_t photo = (spp == 1) ? PHOTOMETRIC_MINISBLACK : PHOTOMETRIC_RGB;
    uint16_t planar = (rd16(&p, &n) & 1U) ? PLANARCONFIG_CONTIG : PLANARCONFIG_SEPARATE;
    uint16_t compression_choices[] = {COMPRESSION_NONE, COMPRESSION_LZW, COMPRESSION_PACKBITS, COMPRESSION_DEFLATE};
    uint16_t compression = compression_choices[rd16(&p, &n) % (sizeof(compression_choices) / sizeof(compression_choices[0]))];
    uint16_t orientation = (uint16_t)((rd16(&p, &n) % 8U) + 1U);
    uint16_t fillorder = (rd16(&p, &n) & 1U) ? FILLORDER_MSB2LSB : FILLORDER_LSB2MSB;
    uint16_t resunit_choices[] = {RESUNIT_NONE, RESUNIT_INCH, RESUNIT_CENTIMETER};
    uint16_t resunit = resunit_choices[rd16(&p, &n) % 3U];
    float xres = rdfloat(&p, &n);
    float yres = rdfloat(&p, &n);
    if (!(xres > 0.0f) || xres != xres) xres = 72.0f;
    if (!(yres > 0.0f) || yres != yres) yres = 72.0f;
    uint16_t rowsperstrip = (uint16_t)(((rd32(&p, &n) % height) + 1U) & 0xFFFFU);
    if (rowsperstrip == 0) rowsperstrip = 1;

    char software[32];
    char artist[32];
    char datetime[32];
    char make[32];
    char model[32];
    char hostcomputer[32];
    memset(software, 0, sizeof(software));
    memset(artist, 0, sizeof(artist));
    memset(datetime, 0, sizeof(datetime));
    memset(make, 0, sizeof(make));
    memset(model, 0, sizeof(model));
    memset(hostcomputer, 0, sizeof(hostcomputer));

    size_t copy = n < sizeof(software) - 1 ? n : sizeof(software) - 1;
    memcpy(software, p, copy);
    copy = n < sizeof(artist) - 1 ? n : sizeof(artist) - 1;
    memcpy(artist, p, copy);
    copy = n < sizeof(make) - 1 ? n : sizeof(make) - 1;
    memcpy(make, p, copy);
    copy = n < sizeof(model) - 1 ? n : sizeof(model) - 1;
    memcpy(model, p, copy);
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

    tmsize_t scanline = TIFFScanlineSize(tif);
    if (scanline > 0 && scanline < (1 << 20)) {
        unsigned char *buf = (unsigned char *)_TIFFmalloc((tmsize_t)scanline);
        if (buf) {
            if (n > 0) {
                size_t i;
                for (i = 0; i < (size_t)scanline; i++) {
                    buf[i] = p[i % n];
                }
            } else {
                memset(buf, 0, (size_t)scanline);
            }
            uint16_t sample = 0;
            if (planar == PLANARCONFIG_SEPARATE && spp > 0) {
                sample = (uint16_t)(rd16(&p, &n) % spp);
            }
            (void)TIFFWriteScanline(tif, buf, 0, sample);
            _TIFFfree(buf);
        }
    }

    (void)TIFFWriteDirectory(tif);

    (void)TIFFCreateGPSDirectory(tif);

    uint16_t gpsversion[4] = {
        (uint16_t)(rd16(&p, &n) & 0xFFU),
        (uint16_t)(rd16(&p, &n) & 0xFFU),
        (uint16_t)(rd16(&p, &n) & 0xFFU),
        (uint16_t)(rd16(&p, &n) & 0xFFU)
    };
    char gpslatref[2] = { (rd16(&p, &n) & 1U) ? 'N' : 'S', '\0' };
    char gpslonref[2] = { (rd16(&p, &n) & 1U) ? 'E' : 'W', '\0' };
    float gpsalt = rdfloat(&p, &n);
    if (gpsalt != gpsalt) gpsalt = 0.0f;
    uint16_t gpsaltref = (uint16_t)(rd16(&p, &n) & 1U);
    char gpsmapdatum[16];
    char gpsdatestamp[16];
    char gpsprocessing[16];
    memset(gpsmapdatum, 0, sizeof(gpsmapdatum));
    memset(gpsdatestamp, 0, sizeof(gpsdatestamp));
    memset(gpsprocessing, 0, sizeof(gpsprocessing));
    snprintf(gpsmapdatum, sizeof(gpsmapdatum), "WGS-%u", (unsigned)(rd16(&p, &n) % 100U));
    snprintf(gpsdatestamp, sizeof(gpsdatestamp), "2024:%02u:%02u",
             (unsigned)((rd16(&p, &n) % 12U) + 1U),
             (unsigned)((rd16(&p, &n) % 28U) + 1U));
    copy = n < sizeof(gpsprocessing) - 1 ? n : sizeof(gpsprocessing) - 1;
    memcpy(gpsprocessing, p, copy);

    double lat0 = (double)(rd16(&p, &n) % 90U);
    double lat1 = (double)(rd16(&p, &n) % 60U);
    double lat2 = rddouble(&p, &n);
    if (!(lat2 >= 0.0) || lat2 != lat2) lat2 = 0.0;
    while (lat2 >= 60.0) lat2 /= 2.0;

    double lon0 = (double)(rd16(&p, &n) % 180U);
    double lon1 = (double)(rd16(&p, &n) % 60U);
    double lon2 = rddouble(&p, &n);
    if (!(lon2 >= 0.0) || lon2 != lon2) lon2 = 0.0;
    while (lon2 >= 60.0) lon2 /= 2.0;

    double gpslatitude[3] = {lat0, lat1, lat2};
    double gpslongitude[3] = {lon0, lon1, lon2};
    double gpstimestamp[3] = {
        (double)(rd16(&p, &n) % 24U),
        (double)(rd16(&p, &n) % 60U),
        (double)(rd16(&p, &n) % 60U)
    };
    uint16_t gpsstatus_char = (rd16(&p, &n) & 1U) ? 'A' : 'V';
    uint16_t gpsmeasuremode_char = (rd16(&p, &n) & 1U) ? '2' : '3';
    uint16_t gpsspeedref_char = 'K';
    uint16_t gpstrackref_char = 'T';
    float gpsspeed = rdfloat(&p, &n);
    float gpstrack = rdfloat(&p, &n);
    if (gpsspeed != gpsspeed) gpsspeed = 0.0f;
    if (gpstrack != gpstrack) gpstrack = 0.0f;

    TIFFSetField(tif, GPSTAG_VERSIONID, 4, gpsversion);
    TIFFSetField(tif, GPSTAG_LATITUDEREF, gpslatref);
    TIFFSetField(tif, GPSTAG_LATITUDE, gpslatitude);
    TIFFSetField(tif, GPSTAG_LONGITUDEREF, gpslonref);
    TIFFSetField(tif, GPSTAG_LONGITUDE, gpslongitude);
    TIFFSetField(tif, GPSTAG_ALTITUDEREF, gpsaltref);
    TIFFSetField(tif, GPSTAG_ALTITUDE, (double)gpsalt);
    TIFFSetField(tif, GPSTAG_TIMESTAMP, gpstimestamp);
    TIFFSetField(tif, GPSTAG_STATUS, &gpsstatus_char);
    TIFFSetField(tif, GPSTAG_MEASUREMODE, &gpsmeasuremode_char);
    TIFFSetField(tif, GPSTAG_SPEEDREF, &gpsspeedref_char);
    TIFFSetField(tif, GPSTAG_SPEED, (double)gpsspeed);
    TIFFSetField(tif, GPSTAG_TRACKREF, &gpstrackref_char);
    TIFFSetField(tif, GPSTAG_TRACK, (double)gpstrack);
    TIFFSetField(tif, GPSTAG_MAPDATUM, gpsmapdatum);
    TIFFSetField(tif, GPSTAG_PROCESSINGMETHOD, (uint32_t)strlen(gpsprocessing), gpsprocessing);
    TIFFSetField(tif, GPSTAG_DATESTAMP, gpsdatestamp);
    TIFFSetField(tif, GPSTAG_DIFFERENTIAL, (uint16_t)(rd16(&p, &n) & 1U));

    uint64_t dir_offset = 0;
    (void)TIFFWriteCustomDirectory(tif, &dir_offset);

    TIFFClose(tif);
    fclose(fp);
    remove("./dummy_file");
    return 0;
}