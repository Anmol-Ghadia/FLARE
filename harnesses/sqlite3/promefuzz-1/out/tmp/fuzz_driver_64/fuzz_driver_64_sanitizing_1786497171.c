#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sqlite3.h>

static uint32_t read_u32(const uint8_t **Data, size_t *Size) {
    uint32_t v = 0;
    size_t n = *Size < 4 ? *Size : 4;
    for (size_t i = 0; i < n; i++) {
        v = (v << 8) | (*Data)[i];
    }
    *Data += n;
    *Size -= n;
    return v;
}

static int read_int(const uint8_t **Data, size_t *Size) {
    return (int)read_u32(Data, Size);
}

static size_t take_bytes(const uint8_t **Data, size_t *Size, size_t max_take) {
    if (*Size == 0) return 0;
    size_t n = read_u32(Data, Size);
    if (max_take > *Size) max_take = *Size;
    if (max_take == 0) return 0;
    return n % (max_take + 1);
}

static void exec_sql(sqlite3 *db, const char *sql) {
    char *errmsg = NULL;
    sqlite3_exec(db, sql, NULL, NULL, &errmsg);
    if (errmsg) sqlite3_free(errmsg);
}

static void exec_blob_insert(sqlite3 *db, const uint8_t *data, size_t size) {
    sqlite3_stmt *stmt = NULL;
    if (sqlite3_prepare_v2(db, "INSERT INTO t1(x,y) VALUES(?1,?2);", -1, &stmt, NULL) != SQLITE_OK) {
        if (stmt) sqlite3_finalize(stmt);
        return;
    }
    sqlite3_bind_int(stmt, 1, (int)size);
    sqlite3_bind_blob(stmt, 2, data, (int)size, SQLITE_STATIC);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

static void setup_db(sqlite3 *db, const uint8_t *data, size_t size, int use_wal) {
    exec_sql(db, "PRAGMA synchronous=OFF;");
    exec_sql(db, "PRAGMA journal_mode=DELETE;");
    if (use_wal) {
        exec_sql(db, "PRAGMA journal_mode=WAL;");
    }
    exec_sql(db, "CREATE TABLE IF NOT EXISTS t1(x INTEGER, y BLOB);");
    exec_sql(db, "CREATE TABLE IF NOT EXISTS t2(a TEXT);");
    exec_blob_insert(db, data, size);
    exec_sql(db, "INSERT INTO t2(a) VALUES('alpha'),('beta'),('gamma');");
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    sqlite3 *src = NULL;
    sqlite3 *dst = NULL;
    sqlite3_backup *bk = NULL;
    FILE *fp;
    int rc;
    int pnLog = -1, pnCkpt = -1;

    fp = fopen("./dummy_file", "wb");
    if (fp) {
        if (Size) fwrite(Data, 1, Size, fp);
        fclose(fp);
    }

    if (sqlite3_open("./dummy_file", &src) != SQLITE_OK) {
        if (src) sqlite3_close(src);
        return 0;
    }

    if (sqlite3_open(":memory:", &dst) != SQLITE_OK) {
        sqlite3_close(src);
        if (dst) sqlite3_close(dst);
        return 0;
    }

    sqlite3_busy_timeout(src, 1);
    sqlite3_busy_timeout(dst, 1);

    {
        const uint8_t *p = Data;
        size_t remaining = Size;
        int use_wal = remaining ? (p[0] & 1) : 0;
        if (remaining) {
            p++;
            remaining--;
        }

        setup_db(src, p, remaining, use_wal);

        if (remaining > 0) {
            size_t chunk = take_bytes(&p, &remaining, remaining);
            if (chunk > 0) {
                exec_blob_insert(src, p, chunk);
                p += chunk;
                remaining -= chunk;
            }
        }

        {
            const char *zDbChoices[] = { "main", "temp", "", "no_such_db", NULL };
            int modeChoices[] = {
                SQLITE_CHECKPOINT_PASSIVE,
                SQLITE_CHECKPOINT_FULL,
                SQLITE_CHECKPOINT_RESTART,
                SQLITE_CHECKPOINT_TRUNCATE
            };
            const char *zDb = zDbChoices[read_u32(&p, &remaining) % 5];
            int mode = modeChoices[read_u32(&p, &remaining) % 4];
            sqlite3_wal_checkpoint_v2(src, zDb, mode, &pnLog, &pnCkpt);
            sqlite3_wal_checkpoint_v2(src, NULL, modeChoices[read_u32(&p, &remaining) % 4], NULL, NULL);
        }

        {
            const char *srcNameChoices[] = { "main", "temp", "no_such_db" };
            const char *dstNameChoices[] = { "main", "temp", "no_such_db" };
            const char *srcName = srcNameChoices[read_u32(&p, &remaining) % 3];
            const char *dstName = dstNameChoices[read_u32(&p, &remaining) % 3];
            int same_handle = remaining ? (p[0] & 1) : 0;
            if (remaining) {
                p++;
                remaining--;
            }

            if (same_handle) {
                bk = sqlite3_backup_init(src, dstName, src, srcName);
            } else {
                bk = sqlite3_backup_init(dst, dstName, src, srcName);
            }
        }

        if (bk) {
            int iterations = 0;
            while (iterations < 32) {
                int nPage;
                int action = read_u32(&p, &remaining) % 6;

                (void)sqlite3_backup_pagecount(bk);
                (void)sqlite3_backup_remaining(bk);

                switch (action) {
                    case 0: nPage = -1; break;
                    case 1: nPage = 0; break;
                    case 2: nPage = 1; break;
                    case 3: nPage = 2; break;
                    case 4: nPage = 8; break;
                    default: nPage = read_int(&p, &remaining); break;
                }

                rc = sqlite3_backup_step(bk, nPage);

                (void)sqlite3_backup_pagecount(bk);
                (void)sqlite3_backup_remaining(bk);

                if ((remaining > 0) && (p[0] & 1)) {
                    exec_sql(src, "INSERT INTO t1(x,y) VALUES(1234, X'414243');");
                }
                if (remaining > 0) {
                    p++;
                    remaining--;
                }

                if (rc == SQLITE_DONE) {
                    break;
                }
                if (rc != SQLITE_OK && rc != SQLITE_BUSY && rc != SQLITE_LOCKED) {
                    break;
                }
                iterations++;
            }
        }

        sqlite3_backup_finish(bk);
        bk = NULL;

        sqlite3_wal_checkpoint_v2(src, "main",
                                  SQLITE_CHECKPOINT_PASSIVE,
                                  &pnLog, &pnCkpt);
        sqlite3_wal_checkpoint_v2(dst, "main",
                                  SQLITE_CHECKPOINT_FULL,
                                  &pnLog, &pnCkpt);
    }

    sqlite3_close(dst);
    sqlite3_close(src);
    return 0;
}