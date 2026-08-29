// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_log at sqlite3.c:17539:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_log at sqlite3.c:17539:17 in sqlite3.h
// sqlite3_errstr at sqlite3.c:154877:24 in sqlite3.h
// sqlite3_errstr at sqlite3.c:154877:24 in sqlite3.h
// sqlite3_errstr at sqlite3.c:154877:24 in sqlite3.h
// sqlite3_errstr at sqlite3.c:154877:24 in sqlite3.h
// sqlite3_errstr at sqlite3.c:154877:24 in sqlite3.h
// sqlite3_errstr at sqlite3.c:154877:24 in sqlite3.h
// sqlite3_errstr at sqlite3.c:154877:24 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_snprintf at sqlite3.c:17503:18 in sqlite3.h
// sqlite3_snprintf at sqlite3.c:17503:18 in sqlite3.h
// sqlite3_snprintf at sqlite3.c:17503:18 in sqlite3.h
// sqlite3_snprintf at sqlite3.c:17503:18 in sqlite3.h
// sqlite3_snprintf at sqlite3.c:17503:18 in sqlite3.h
// sqlite3_snprintf at sqlite3.c:17503:18 in sqlite3.h
// sqlite3_log at sqlite3.c:17539:17 in sqlite3.h
// sqlite3_log at sqlite3.c:17539:17 in sqlite3.h
// sqlite3_log at sqlite3.c:17539:17 in sqlite3.h
// sqlite3_vmprintf at sqlite3.c:17439:18 in sqlite3.h
// sqlite3_vmprintf at sqlite3.c:17439:18 in sqlite3.h
// sqlite3_vsnprintf at sqlite3.c:17488:18 in sqlite3.h
// sqlite3_vsnprintf at sqlite3.c:17488:18 in sqlite3.h
// sqlite3_log at sqlite3.c:17539:17 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_log at sqlite3.c:17539:17 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "sqlite3.h"

static uint32_t read_u32(const uint8_t **p, size_t *n){
    uint32_t v = 0;
    size_t i, m = *n < 4 ? *n : 4;
    for(i = 0; i < m; i++){
        v = (v << 8) | (*p)[i];
    }
    *p += m;
    *n -= m;
    return v;
}

static int read_int(const uint8_t **p, size_t *n){
    return (int)read_u32(p, n);
}

static size_t read_size(const uint8_t **p, size_t *n, size_t maxv){
    if(*n == 0) return 0;
    return (size_t)(read_u32(p, n) % (maxv + 1));
}

static char *make_cstring(const uint8_t **p, size_t *n, size_t maxlen){
    size_t len = read_size(p, n, maxlen);
    char *s = (char *)malloc(len + 1);
    if(!s) return NULL;
    if(len > *n) len = *n;
    if(len) memcpy(s, *p, len);
    s[len] = '\0';
    *p += len;
    *n -= len;
    return s;
}

static char *make_buffer(const uint8_t **p, size_t *n, size_t maxlen, size_t *out_len){
    size_t len = read_size(p, n, maxlen);
    char *buf = (char *)malloc(len ? len : 1);
    if(!buf) return NULL;
    if(len){
        size_t copy = len;
        if(copy > *n) copy = *n;
        if(copy) memcpy(buf, *p, copy);
        if(copy < len) memset(buf + copy, 0x41, len - copy);
        *p += copy;
        *n -= copy;
    }
    *out_len = len;
    return buf;
}

static char *vmprintf_wrap_s3(const char *f, ...){
    char *r;
    va_list ap;
    va_start(ap, f);
    r = sqlite3_vmprintf(f, ap);
    va_end(ap);
    return r;
}

static char *vmprintf_wrap_si2(const char *f, ...){
    char *r;
    va_list ap;
    va_start(ap, f);
    r = sqlite3_vmprintf(f, ap);
    va_end(ap);
    return r;
}

static char *vsnprintf_wrap_s3(int nn, char *b, const char *f, ...){
    char *r;
    va_list ap;
    va_start(ap, f);
    r = sqlite3_vsnprintf(nn, b, f, ap);
    va_end(ap);
    return r;
}

static char *vsnprintf_wrap_si1(int nn, char *b, const char *f, ...){
    char *r;
    va_list ap;
    va_start(ap, f);
    r = sqlite3_vsnprintf(nn, b, f, ap);
    va_end(ap);
    return r;
}

static void log_wrap_s3(int rc, const char *fmt, const char *s1, const char *s2, const char *s3){
    char *msg = vmprintf_wrap_s3(fmt, s1, s2, s3);
    if(msg){
        sqlite3_log(rc, "%s", msg);
        sqlite3_free(msg);
    }else{
        sqlite3_log(rc, "%s", "");
    }
}

static void log_wrap_si2(int rc, const char *fmt, const char *s1, int i1, const char *s2, int i2){
    char *msg = vmprintf_wrap_si2(fmt, s1, i1, s2, i2);
    if(msg){
        sqlite3_log(rc, "%s", msg);
        sqlite3_free(msg);
    }else{
        sqlite3_log(rc, "%s", "");
    }
}

int LLVMFuzzerTestOneInput_41(const uint8_t *Data, size_t Size){
    const uint8_t *p = Data;
    size_t n = Size;

    int rc1 = read_int(&p, &n);
    int rc2 = read_int(&p, &n);
    int rc3 = read_int(&p, &n);

    char *fmt1 = make_cstring(&p, &n, 256);
    char *fmt2 = make_cstring(&p, &n, 256);
    char *arg1 = make_cstring(&p, &n, 256);
    char *arg2 = make_cstring(&p, &n, 256);
    char *arg3 = make_cstring(&p, &n, 256);

    size_t buf1_len = 0, buf2_len = 0;
    char *buf1 = make_buffer(&p, &n, 512, &buf1_len);
    char *buf2 = make_buffer(&p, &n, 512, &buf2_len);

    if(!fmt1 || !fmt2 || !arg1 || !arg2 || !arg3 || !buf1 || !buf2){
        free(fmt1);
        free(fmt2);
        free(arg1);
        free(arg2);
        free(arg3);
        free(buf1);
        free(buf2);
        return 0;
    }

    (void)sqlite3_errstr(rc1);
    (void)sqlite3_errstr(rc2);
    (void)sqlite3_errstr(rc3);
    (void)sqlite3_errstr(SQLITE_OK);
    (void)sqlite3_errstr(SQLITE_ERROR);
    (void)sqlite3_errstr(SQLITE_NOMEM);
    (void)sqlite3_errstr(SQLITE_MISUSE);

    {
        char *m1 = sqlite3_mprintf("%s|%s|%s", arg1, arg2, arg3);
        if(m1) sqlite3_free(m1);

        char *m2 = sqlite3_mprintf("%s|%d|%s|%d|%s|%d", arg1, rc1, arg2, rc2, arg3, rc3);
        if(m2) sqlite3_free(m2);

        char *m3 = sqlite3_mprintf("%q %Q %w", arg1, arg2, arg3);
        if(m3) sqlite3_free(m3);

        char *m4 = sqlite3_mprintf("%s", fmt1);
        if(m4) sqlite3_free(m4);

        char *m5 = sqlite3_mprintf("%s", fmt2);
        if(m5) sqlite3_free(m5);
    }

    {
        char *v1 = vmprintf_wrap_s3("%s %s %s", arg1, arg2, arg3);
        if(v1) sqlite3_free(v1);

        char *v2 = vmprintf_wrap_si2("%s %d %s %d", arg1, rc1, arg2, rc2);
        if(v2) sqlite3_free(v2);

        char *v3 = vmprintf_wrap_s3("%q:%Q:%w", arg1, arg2, arg3);
        if(v3) sqlite3_free(v3);

        char *v4 = vmprintf_wrap_s3("%s", fmt1, "", "");
        if(v4) sqlite3_free(v4);
    }

    {
        int n1 = (int)buf1_len;
        int n2 = (int)buf2_len;

        if(buf1_len > 0){
            sqlite3_snprintf(n1, buf1, "%s|%s|%s", arg1, arg2, arg3);
            vsnprintf_wrap_si1(n1, buf1, "%s-%d-%s", arg1, rc1, arg2);
            vsnprintf_wrap_s3(n1, buf1, "%q|%Q|%w", arg1, arg2, arg3);
            sqlite3_snprintf(n1, buf1, "%s", fmt1);
        }else{
            sqlite3_snprintf(0, buf1, "%s", arg1);
            vsnprintf_wrap_s3(0, buf1, "%s", arg1, "", "");
        }

        if(buf2_len > 0){
            sqlite3_snprintf(n2, buf2, "%s %s %d %d", arg1, arg2, rc1, rc2);
            vsnprintf_wrap_s3(n2, buf2, "%s", fmt2, "", "");
            sqlite3_snprintf(n2, buf2, "%q:%Q:%w", arg1, arg2, arg3);
        }else{
            sqlite3_snprintf(0, buf2, "%s", arg2);
            vsnprintf_wrap_s3(0, buf2, "%s", arg3, "", "");
        }
    }

    sqlite3_log(rc1, "%s", arg1);
    sqlite3_log(rc2, "%s %d %s", arg2, rc2, arg3);
    log_wrap_s3(rc3, "%s|%s|%s", arg1, arg2, arg3);
    log_wrap_s3(SQLITE_ERROR, "%q %Q %w", arg1, arg2, arg3);
    log_wrap_si2(SQLITE_MISUSE, "%s %d %s %d", arg1, rc1, arg2, rc2);
    sqlite3_log(SQLITE_OK, "%s", fmt1);

    free(fmt1);
    free(fmt2);
    free(arg1);
    free(arg2);
    free(arg3);
    free(buf1);
    free(buf2);
    return 0;
}