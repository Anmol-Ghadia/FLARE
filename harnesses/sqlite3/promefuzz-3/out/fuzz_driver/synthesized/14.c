// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_open at sqlite3.c:155676:16 in sqlite3.h
// sqlite3_close at sqlite3.c:153535:16 in sqlite3.h
// sqlite3_mprintf at sqlite3.c:17463:18 in sqlite3.h
// sqlite3_free at sqlite3.c:15815:17 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_threadsafe at sqlite3.c:152376:16 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_enable_shared_cache at sqlite3.c:53207:16 in sqlite3.h
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

static int consume_int(const uint8_t *Data, size_t Size, size_t *Off) {
    int v = 0;
    size_t i;
    for (i = 0; i < sizeof(int) && *Off < Size; ++i, ++(*Off)) {
        v = (v << 8) | Data[*Off];
    }
    return v;
}

int LLVMFuzzerTestOneInput_14(const uint8_t *Data, size_t Size) {
    size_t off = 0;
    int rc;
    int shared_cache_flag;
    int config_choice1, config_choice2, config_choice3;
    sqlite3 *db = NULL;
    char *sqlite_mem = NULL;
    FILE *fp;

    fp = fopen("./dummy_file", "wb");
    if (fp != NULL) {
        if (Size > 0) {
            (void)fwrite(Data, 1, Size, fp);
        }
        fclose(fp);
    }

    config_choice1 = consume_int(Data, Size, &off) % 4;
    config_choice2 = consume_int(Data, Size, &off) % 4;
    config_choice3 = consume_int(Data, Size, &off) % 4;
    shared_cache_flag = consume_int(Data, Size, &off) & 1;

    switch (config_choice1) {
        case 0:
            (void)sqlite3_config(SQLITE_CONFIG_SINGLETHREAD);
            break;
        case 1:
            (void)sqlite3_config(SQLITE_CONFIG_MULTITHREAD);
            break;
        case 2:
            (void)sqlite3_config(SQLITE_CONFIG_SERIALIZED);
            break;
        default:
            (void)sqlite3_config(SQLITE_CONFIG_URI, shared_cache_flag);
            break;
    }

    switch (config_choice2) {
        case 0:
            (void)sqlite3_config(SQLITE_CONFIG_MEMSTATUS, shared_cache_flag);
            break;
        case 1:
            (void)sqlite3_config(SQLITE_CONFIG_URI, shared_cache_flag);
            break;
        case 2:
            (void)sqlite3_config(SQLITE_CONFIG_COVERING_INDEX_SCAN, shared_cache_flag);
            break;
        default:
            (void)sqlite3_config(SQLITE_CONFIG_SORTERREF_SIZE, consume_int(Data, Size, &off));
            break;
    }

    switch (config_choice3) {
        case 0:
            (void)sqlite3_config(SQLITE_CONFIG_STMTJRNL_SPILL, consume_int(Data, Size, &off));
            break;
        case 1:
            (void)sqlite3_config(SQLITE_CONFIG_MEMSTATUS, (consume_int(Data, Size, &off) & 1));
            break;
        case 2:
            (void)sqlite3_config(SQLITE_CONFIG_URI, (consume_int(Data, Size, &off) & 1));
            break;
        default:
            (void)sqlite3_config(SQLITE_CONFIG_PCACHE_HDRSZ, &rc);
            break;
    }

    (void)sqlite3_threadsafe();

    rc = sqlite3_initialize();
    (void)rc;

    (void)sqlite3_enable_shared_cache(shared_cache_flag);

    {
        const char *filename;
        int selector = consume_int(Data, Size, &off) % 5;
        switch (selector) {
            case 0:
                filename = ":memory:";
                break;
            case 1:
                filename = "";
                break;
            case 2:
                filename = "./dummy_file";
                break;
            case 3:
                filename = "file:./dummy_file?mode=ro";
                break;
            default:
                filename = "file:./dummy_file?mode=memory&cache=shared";
                break;
        }
        (void)sqlite3_open(filename, &db);
    }

    (void)sqlite3_close(db);

    sqlite_mem = sqlite3_mprintf("%.*s", (int)((Size > off) ? (Size - off) : 0), (const char *)(Data + off));
    sqlite3_free(sqlite_mem);

    return 0;
}