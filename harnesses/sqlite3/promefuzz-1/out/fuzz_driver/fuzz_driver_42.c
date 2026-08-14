// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_open_v2 at sqlite3.c:155683:16 in sqlite3.h
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_mutex_alloc at sqlite3.c:14046:27 in sqlite3.h
// sqlite3_mutex_alloc at sqlite3.c:14046:27 in sqlite3.h
// sqlite3_mutex_try at sqlite3.c:14089:16 in sqlite3.h
// sqlite3_mutex_held at sqlite3.c:14116:16 in sqlite3.h
// sqlite3_mutex_notheld at sqlite3.c:14120:16 in sqlite3.h
// sqlite3_mutex_leave at sqlite3.c:14104:17 in sqlite3.h
// sqlite3_db_mutex at sqlite3.c:153096:27 in sqlite3.h
// sqlite3_mutex_held at sqlite3.c:14116:16 in sqlite3.h
// sqlite3_mutex_notheld at sqlite3.c:14120:16 in sqlite3.h
// sqlite3_mutex_try at sqlite3.c:14089:16 in sqlite3.h
// sqlite3_mutex_held at sqlite3.c:14116:16 in sqlite3.h
// sqlite3_mutex_notheld at sqlite3.c:14120:16 in sqlite3.h
// sqlite3_mutex_held at sqlite3.c:14116:16 in sqlite3.h
// sqlite3_mutex_notheld at sqlite3.c:14120:16 in sqlite3.h
// sqlite3_mutex_try at sqlite3.c:14089:16 in sqlite3.h
// sqlite3_mutex_held at sqlite3.c:14116:16 in sqlite3.h
// sqlite3_mutex_notheld at sqlite3.c:14120:16 in sqlite3.h
// sqlite3_mutex_leave at sqlite3.c:14104:17 in sqlite3.h
// sqlite3_mutex_leave at sqlite3.c:14104:17 in sqlite3.h
// sqlite3_mutex_free at sqlite3.c:14067:17 in sqlite3.h
// sqlite3_mutex_free at sqlite3.c:14067:17 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <sqlite3.h>

static int pick_mutex_id(uint8_t v) {
    /* Only allocate dynamic mutex types that are safe to try/enter/free. */
    static const int ids[] = {
        SQLITE_MUTEX_FAST,
        SQLITE_MUTEX_RECURSIVE
    };
    return ids[v % (sizeof(ids) / sizeof(ids[0]))];
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    sqlite3 *db = NULL;
    sqlite3_mutex *m1 = NULL, *m2 = NULL;
    int rc;
    int entered_m1 = 0, entered_m2 = 0;
    int id1, id2;
    FILE *fp;

    fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Data && Size) {
            fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    sqlite3_initialize();

    if (Size > 0) {
        int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
        if (Data[0] & 1) flags |= SQLITE_OPEN_URI;
        if (Data[0] & 2) flags |= SQLITE_OPEN_MEMORY;
        if (Data[0] & 4) flags |= SQLITE_OPEN_NOMUTEX;
        if (Data[0] & 8) flags |= SQLITE_OPEN_FULLMUTEX;
        sqlite3_open_v2("./dummy_file", &db, flags, NULL);
    } else {
        sqlite3_open("./dummy_file", &db);
    }

    id1 = (Size > 1) ? pick_mutex_id(Data[1]) : SQLITE_MUTEX_FAST;
    id2 = (Size > 2) ? pick_mutex_id(Data[2]) : SQLITE_MUTEX_RECURSIVE;

    m1 = sqlite3_mutex_alloc(id1);
    m2 = sqlite3_mutex_alloc(id2);

    /* NULL mutex operations are explicitly allowed by SQLite. */
    (void)sqlite3_mutex_try(NULL);
    (void)sqlite3_mutex_held(NULL);
    (void)sqlite3_mutex_notheld(NULL);
    sqlite3_mutex_leave(NULL);

    /*
     * Do not call sqlite3_mutex_try()/enter()/leave() on sqlite3_db_mutex(db).
     * The DB mutex may already be held internally by SQLite, and probing it
     * with try() can trip debug assertions on non-recursive mutexes.
     * Only fetch it to exercise the accessor.
     */
    if (db) {
        (void)sqlite3_db_mutex(db);
    }

    if (m1) {
        (void)sqlite3_mutex_held(m1);
        (void)sqlite3_mutex_notheld(m1);
        rc = sqlite3_mutex_try(m1);
        if (rc == SQLITE_OK) {
            entered_m1 = 1;
            (void)sqlite3_mutex_held(m1);
            (void)sqlite3_mutex_notheld(m1);
        }
    }

    if (m2) {
        (void)sqlite3_mutex_held(m2);
        (void)sqlite3_mutex_notheld(m2);
        rc = sqlite3_mutex_try(m2);
        if (rc == SQLITE_OK) {
            entered_m2 = 1;
            (void)sqlite3_mutex_held(m2);
            (void)sqlite3_mutex_notheld(m2);
        }
    }

    if (entered_m1) {
        sqlite3_mutex_leave(m1);
    }
    if (entered_m2) {
        sqlite3_mutex_leave(m2);
    }

    if (m1) {
        sqlite3_mutex_free(m1);
    }
    if (m2) {
        sqlite3_mutex_free(m2);
    }

    if (db) {
        sqlite3_close(db);
    }

    return 0;
}