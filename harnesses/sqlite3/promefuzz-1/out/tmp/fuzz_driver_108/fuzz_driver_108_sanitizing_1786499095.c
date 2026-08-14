#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "sqlite3.h"

static char *dup_nul_terminated(const uint8_t *data, size_t len) {
    char *s = (char *)malloc(len + 1);
    if (!s) return NULL;
    if (len) memcpy(s, data, len);
    s[len] = '\0';
    return s;
}

static int consume_u8(const uint8_t **pData, size_t *pSize, uint8_t *out) {
    if (*pSize < 1) return 0;
    *out = **pData;
    (*pData)++;
    (*pSize)--;
    return 1;
}

static size_t consume_len(const uint8_t **pData, size_t *pSize, size_t max_len) {
    uint8_t v = 0;
    if (!consume_u8(pData, pSize, &v)) return 0;
    return (size_t)(v % (max_len + 1));
}

static char *consume_string(const uint8_t **pData, size_t *pSize, size_t max_len) {
    size_t len = consume_len(pData, pSize, max_len);
    if (len > *pSize) len = *pSize;
    char *s = dup_nul_terminated(*pData, len);
    if (!s) return NULL;
    *pData += len;
    *pSize -= len;
    return s;
}

static void touch_string(const char *s) {
    volatile unsigned char sink = 0;
    if (s) sink ^= (unsigned char)s[0];
    (void)sink;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    const uint8_t *p = Data;
    size_t n = Size;
    uint8_t mode = 0;
    uint8_t nParamByte = 0;
    int i;

    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size) fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    if (!consume_u8(&p, &n, &mode)) mode = 0;
    if (!consume_u8(&p, &n, &nParamByte)) nParamByte = 0;

    char *db = consume_string(&p, &n, 64);
    char *jrnl = consume_string(&p, &n, 64);
    char *wal = consume_string(&p, &n, 64);
    if (!db || !jrnl || !wal) {
        free(db);
        free(jrnl);
        free(wal);
        return 0;
    }

    if ((mode & 1) == 0) {
        free(db);
        db = dup_nul_terminated((const uint8_t *)"./dummy_file", strlen("./dummy_file"));
        if (!db) {
            free(jrnl);
            free(wal);
            return 0;
        }
    }

    int nParam = (int)(nParamByte % 8);
    const char **azParam = NULL;
    char **owned = NULL;

    if (nParam > 0) {
        azParam = (const char **)calloc((size_t)nParam * 2, sizeof(const char *));
        owned = (char **)calloc((size_t)nParam * 2, sizeof(char *));
        if (!azParam || !owned) {
            free(db);
            free(jrnl);
            free(wal);
            free(azParam);
            free(owned);
            return 0;
        }

        for (i = 0; i < nParam * 2; i += 2) {
            char *k = consume_string(&p, &n, 32);
            char *v = consume_string(&p, &n, 32);
            if (!k || !v) {
                free(k);
                free(v);
                for (int j = 0; j < i; ++j) free(owned[j]);
                free(azParam);
                free(owned);
                free(db);
                free(jrnl);
                free(wal);
                return 0;
            }
            if (k[0] == '\0') {
                free(k);
                k = dup_nul_terminated((const uint8_t *)"k", 1);
                if (!k) {
                    free(v);
                    for (int j = 0; j < i; ++j) free(owned[j]);
                    free(azParam);
                    free(owned);
                    free(db);
                    free(jrnl);
                    free(wal);
                    return 0;
                }
            }
            owned[i] = k;
            owned[i + 1] = v;
            azParam[i] = k;
            azParam[i + 1] = v;
        }
    }

    char *fname = sqlite3_create_filename(db, jrnl, wal, nParam, azParam);
    if (fname) {
        const char *f_db = sqlite3_filename_database(fname);
        const char *f_j = sqlite3_filename_journal(fname);
        const char *f_w = sqlite3_filename_wal(fname);

        touch_string(f_db);
        touch_string(f_j);
        touch_string(f_w);

        if (nParam > 0 && (mode & 2)) {
            const char *param_name = azParam[(mode % (uint8_t)nParam) * 2];
            const char *pv = sqlite3_uri_parameter(fname, param_name);
            touch_string(pv);
        } else {
            char *tmp = consume_string(&p, &n, 32);
            if (tmp) {
                const char *pv = sqlite3_uri_parameter(fname, tmp);
                touch_string(pv);
                free(tmp);
            } else {
                const char *pv = sqlite3_uri_parameter(fname, "mode");
                touch_string(pv);
            }
        }

        if (f_db) {
            touch_string(sqlite3_filename_database(f_db));
            touch_string(sqlite3_filename_journal(f_db));
            touch_string(sqlite3_filename_wal(f_db));
            if (nParam > 0) {
                const char *pv = sqlite3_uri_parameter(f_db, azParam[0]);
                touch_string(pv);
            }
        }

        if (f_j) {
            touch_string(sqlite3_filename_database(f_j));
            touch_string(sqlite3_filename_journal(f_j));
            touch_string(sqlite3_filename_wal(f_j));
            if (nParam > 0) {
                const char *pv = sqlite3_uri_parameter(f_j, azParam[0]);
                touch_string(pv);
            }
        }

        if (f_w) {
            touch_string(sqlite3_filename_database(f_w));
            touch_string(sqlite3_filename_journal(f_w));
            touch_string(sqlite3_filename_wal(f_w));
            if (nParam > 0) {
                const char *pv = sqlite3_uri_parameter(f_w, azParam[0]);
                touch_string(pv);
            }
        }

        touch_string(sqlite3_uri_parameter(NULL, "x"));
        touch_string(sqlite3_uri_parameter(fname, NULL));

        sqlite3_free_filename(fname);
    }

    for (i = 0; i < nParam * 2; ++i) {
        free(owned ? owned[i] : NULL);
    }
    free(owned);
    free(azParam);
    free(db);
    free(jrnl);
    free(wal);
    return 0;
}