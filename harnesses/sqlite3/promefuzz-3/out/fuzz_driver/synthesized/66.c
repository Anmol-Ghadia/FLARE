// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_mutex_try at sqlite3.c:14089:16 in sqlite3.h
// sqlite3_mutex_held at sqlite3.c:14116:16 in sqlite3.h
// sqlite3_mutex_notheld at sqlite3.c:14120:16 in sqlite3.h
// sqlite3_mutex_alloc at sqlite3.c:14046:27 in sqlite3.h
// sqlite3_mutex_alloc at sqlite3.c:14046:27 in sqlite3.h
// sqlite3_mutex_enter at sqlite3.c:14078:17 in sqlite3.h
// sqlite3_mutex_try at sqlite3.c:14089:16 in sqlite3.h
// sqlite3_mutex_held at sqlite3.c:14116:16 in sqlite3.h
// sqlite3_mutex_notheld at sqlite3.c:14120:16 in sqlite3.h
// sqlite3_mutex_leave at sqlite3.c:14104:17 in sqlite3.h
// sqlite3_mutex_leave at sqlite3.c:14104:17 in sqlite3.h
// sqlite3_shutdown at sqlite3.c:152624:16 in sqlite3.h
// sqlite3_mutex_leave at sqlite3.c:14104:17 in sqlite3.h
// sqlite3_mutex_free at sqlite3.c:14067:17 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_mutex_alloc at sqlite3.c:14046:27 in sqlite3.h
// sqlite3_mutex_enter at sqlite3.c:14078:17 in sqlite3.h
// sqlite3_mutex_leave at sqlite3.c:14104:17 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <sqlite3.h>

static uint32_t read_u32(const uint8_t **p, size_t *n) {
    uint32_t v = 0;
    size_t take = *n < 4 ? *n : 4;
    for (size_t i = 0; i < take; i++) {
        v = (v << 8) | (*p)[i];
    }
    *p += take;
    *n -= take;
    return v;
}

static int choose_mutex_type(uint8_t b) {
    return (b & 1) ? SQLITE_MUTEX_RECURSIVE : SQLITE_MUTEX_FAST;
}

static int is_dynamic_type(int t) {
    return t == SQLITE_MUTEX_FAST || t == SQLITE_MUTEX_RECURSIVE;
}

static int can_reenter(int t) {
    return t == SQLITE_MUTEX_RECURSIVE;
}

static void release_slot(sqlite3_mutex **pm, unsigned char *entered, int is_dynamic) {
    if (!pm || !*pm) {
        return;
    }

    while (*entered > 0) {
        sqlite3_mutex_leave(*pm);
        (*entered)--;
    }

    if (is_dynamic) {
        sqlite3_mutex_free(*pm);
    }

    *pm = NULL;
    *entered = 0;
}

int LLVMFuzzerTestOneInput_66(const uint8_t *Data, size_t Size) {
    const uint8_t *p = Data;
    size_t n = Size;
    sqlite3_mutex *mutexes[16];
    unsigned char is_dynamic[16];
    unsigned char entered[16];
    int types[16];

    memset(mutexes, 0, sizeof(mutexes));
    memset(is_dynamic, 0, sizeof(is_dynamic));
    memset(entered, 0, sizeof(entered));
    memset(types, 0, sizeof(types));

    (void)sqlite3_initialize();

    {
        uint32_t steps = n ? (read_u32(&p, &n) % 128U) : 0U;

        for (uint32_t i = 0; i < steps; i++) {
            uint8_t op = n ? *p++ : 0;
            if (n) n--;

            uint8_t idx = n ? *p++ : 0;
            if (n) n--;
            idx %= 16;

            switch (op % 7) {
                case 0: {
                    int t = choose_mutex_type(op ^ idx);
                    release_slot(&mutexes[idx], &entered[idx], is_dynamic[idx]);
                    mutexes[idx] = sqlite3_mutex_alloc(t);
                    is_dynamic[idx] = (unsigned char)is_dynamic_type(t);
                    entered[idx] = 0;
                    types[idx] = t;
                    break;
                }

                case 1: {
                    if (mutexes[idx]) {
                        if (entered[idx] == 0 || can_reenter(types[idx])) {
                            sqlite3_mutex_enter(mutexes[idx]);
                            if (entered[idx] < 255) {
                                entered[idx]++;
                            }
                        }
                    }
                    break;
                }

                case 2: {
                    if (mutexes[idx] && entered[idx] > 0) {
                        sqlite3_mutex_leave(mutexes[idx]);
                        entered[idx]--;
                    }
                    break;
                }

                case 3: {
                    if (mutexes[idx]) {
                        if (entered[idx] == 0 || can_reenter(types[idx])) {
                            int rc = sqlite3_mutex_try(mutexes[idx]);
                            if (rc == SQLITE_OK && entered[idx] < 255) {
                                entered[idx]++;
                            }
                        }
                    }
                    break;
                }

                case 4: {
                    if (mutexes[idx]) {
                        (void)sqlite3_mutex_held(mutexes[idx]);
                        (void)sqlite3_mutex_notheld(mutexes[idx]);
                    }
                    break;
                }

                case 5: {
                    release_slot(&mutexes[idx], &entered[idx], is_dynamic[idx]);
                    is_dynamic[idx] = 0;
                    types[idx] = 0;
                    break;
                }

                case 6: {
                    uint8_t idx2 = n ? *p++ : 0;
                    if (n) n--;
                    idx2 %= 16;

                    if (!mutexes[idx]) {
                        int t1 = choose_mutex_type(idx);
                        mutexes[idx] = sqlite3_mutex_alloc(t1);
                        is_dynamic[idx] = (unsigned char)is_dynamic_type(t1);
                        entered[idx] = 0;
                        types[idx] = t1;
                    }
                    if (!mutexes[idx2]) {
                        int t2 = choose_mutex_type(idx2);
                        mutexes[idx2] = sqlite3_mutex_alloc(t2);
                        is_dynamic[idx2] = (unsigned char)is_dynamic_type(t2);
                        entered[idx2] = 0;
                        types[idx2] = t2;
                    }

                    if (idx != idx2) {
                        if (mutexes[idx] && (entered[idx] == 0 || can_reenter(types[idx]))) {
                            sqlite3_mutex_enter(mutexes[idx]);
                            if (entered[idx] < 255) {
                                entered[idx]++;
                            }
                        }
                        if (mutexes[idx2]) {
                            if (entered[idx2] == 0 || can_reenter(types[idx2])) {
                                int rc = sqlite3_mutex_try(mutexes[idx2]);
                                if (rc == SQLITE_OK && entered[idx2] < 255) {
                                    entered[idx2]++;
                                }
                            }
                            (void)sqlite3_mutex_held(mutexes[idx2]);
                            (void)sqlite3_mutex_notheld(mutexes[idx2]);
                            if (entered[idx2] > 0) {
                                sqlite3_mutex_leave(mutexes[idx2]);
                                entered[idx2]--;
                            }
                        }
                        if (mutexes[idx] && entered[idx] > 0) {
                            sqlite3_mutex_leave(mutexes[idx]);
                            entered[idx]--;
                        }
                    }
                    break;
                }
            }
        }
    }

    for (size_t i = 0; i < 16; i++) {
        release_slot(&mutexes[i], &entered[i], is_dynamic[i]);
        is_dynamic[i] = 0;
        types[i] = 0;
    }

    (void)sqlite3_shutdown();
    return 0;
}