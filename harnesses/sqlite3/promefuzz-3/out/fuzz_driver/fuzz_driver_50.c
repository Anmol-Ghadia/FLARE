// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_randomness at sqlite3.c:18421:17 in sqlite3.h
// sqlite3_status at sqlite3.c:9586:16 in sqlite3.h
// sqlite3_test_control at sqlite3.c:156127:16 in sqlite3.h
// sqlite3_test_control at sqlite3.c:156127:16 in sqlite3.h
// sqlite3_test_control at sqlite3.c:156127:16 in sqlite3.h
// sqlite3_test_control at sqlite3.c:156127:16 in sqlite3.h
// sqlite3_shutdown at sqlite3.c:152624:16 in sqlite3.h
// sqlite3_shutdown at sqlite3.c:152624:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_test_control at sqlite3.c:156127:16 in sqlite3.h
// sqlite3_randomness at sqlite3.c:18421:17 in sqlite3.h
// sqlite3_test_control at sqlite3.c:156127:16 in sqlite3.h
// sqlite3_randomness at sqlite3.c:18421:17 in sqlite3.h
// sqlite3_shutdown at sqlite3.c:152624:16 in sqlite3.h
// sqlite3_shutdown at sqlite3.c:152624:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
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

static int read_int_range(const uint8_t **p, size_t *n, int minv, int maxv) {
  if (maxv <= minv) return minv;
  uint32_t v = read_u32(p, n);
  uint32_t span = (uint32_t)(maxv - minv + 1);
  return minv + (int)(v % span);
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  const uint8_t *p = Data;
  size_t n = Size;

  FILE *fp = fopen("./dummy_file", "wb");
  if (fp) {
    if (Size > 0) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  (void)sqlite3_shutdown();

  if (n > 0) {
    int pre_cfg = (int)(*p % 4);
    p++;
    n--;

    switch (pre_cfg) {
      case 0: {
        int onoff = read_int_range(&p, &n, 0, 1);
        (void)sqlite3_config(SQLITE_CONFIG_MEMSTATUS, onoff);
        break;
      }
      case 1: {
        int onoff = read_int_range(&p, &n, 0, 1);
        (void)sqlite3_config(SQLITE_CONFIG_COVERING_INDEX_SCAN, onoff);
        break;
      }
      case 2: {
        int onoff = read_int_range(&p, &n, 0, 1);
        (void)sqlite3_config(SQLITE_CONFIG_URI, onoff);
        break;
      }
      case 3: {
        int small = read_int_range(&p, &n, 0, 1);
        (void)sqlite3_config(SQLITE_CONFIG_SMALL_MALLOC, small);
        break;
      }
    }
  }

  (void)sqlite3_initialize();

  int steps = (n > 0) ? (int)(*p % 32) + 1 : 8;
  if (n > 0) {
    p++;
    n--;
  }

  unsigned char randbuf[256];
  int current = 0;
  int highwater = 0;

  for (int i = 0; i < steps; i++) {
    int action = read_int_range(&p, &n, 0, 9);

    switch (action) {
      case 0: {
        (void)sqlite3_initialize();
        break;
      }

      case 1: {
        int len = read_int_range(&p, &n, -16, 512);
        void *out = randbuf;
        if ((read_u32(&p, &n) & 1U) != 0) {
          out = NULL;
        }
        if (len > (int)sizeof(randbuf)) len = (int)sizeof(randbuf);
        sqlite3_randomness(len, out);
        break;
      }

      case 2: {
        int ops[] = {
#ifdef SQLITE_STATUS_MEMORY_USED
          SQLITE_STATUS_MEMORY_USED,
#endif
#ifdef SQLITE_STATUS_PAGECACHE_USED
          SQLITE_STATUS_PAGECACHE_USED,
#endif
#ifdef SQLITE_STATUS_PAGECACHE_OVERFLOW
          SQLITE_STATUS_PAGECACHE_OVERFLOW,
#endif
#ifdef SQLITE_STATUS_SCRATCH_USED
          SQLITE_STATUS_SCRATCH_USED,
#endif
#ifdef SQLITE_STATUS_SCRATCH_OVERFLOW
          SQLITE_STATUS_SCRATCH_OVERFLOW,
#endif
#ifdef SQLITE_STATUS_MALLOC_SIZE
          SQLITE_STATUS_MALLOC_SIZE,
#endif
#ifdef SQLITE_STATUS_PARSER_STACK
          SQLITE_STATUS_PARSER_STACK,
#endif
#ifdef SQLITE_STATUS_PAGECACHE_SIZE
          SQLITE_STATUS_PAGECACHE_SIZE,
#endif
#ifdef SQLITE_STATUS_SCRATCH_SIZE
          SQLITE_STATUS_SCRATCH_SIZE,
#endif
#ifdef SQLITE_STATUS_MALLOC_COUNT
          SQLITE_STATUS_MALLOC_COUNT,
#endif
        };
        int opcount = (int)(sizeof(ops) / sizeof(ops[0]));
        if (opcount > 0) {
          int op = ops[read_int_range(&p, &n, 0, opcount - 1)];
          int reset = read_int_range(&p, &n, 0, 1);
          int *pc = ((read_u32(&p, &n) & 1U) != 0) ? &current : NULL;
          int *ph = ((read_u32(&p, &n) & 1U) != 0) ? &highwater : NULL;
          (void)sqlite3_status(op, pc, ph, reset);
        }
        break;
      }

      case 3: {
#ifdef SQLITE_TESTCTRL_PRNG_SAVE
        (void)sqlite3_test_control(SQLITE_TESTCTRL_PRNG_SAVE);
#endif
        break;
      }

      case 4: {
#ifdef SQLITE_TESTCTRL_PRNG_RESTORE
        (void)sqlite3_test_control(SQLITE_TESTCTRL_PRNG_RESTORE);
#endif
        break;
      }

      case 5: {
#ifdef SQLITE_TESTCTRL_PRNG_RESET
        (void)sqlite3_test_control(SQLITE_TESTCTRL_PRNG_RESET);
#endif
        break;
      }

      case 6: {
#ifdef SQLITE_TESTCTRL_BYTEORDER
        (void)sqlite3_test_control(SQLITE_TESTCTRL_BYTEORDER);
#endif
        break;
      }

      case 7: {
        (void)sqlite3_shutdown();
        break;
      }

      case 8: {
        (void)sqlite3_shutdown();
        int cfg = read_int_range(&p, &n, 0, 3);
        switch (cfg) {
          case 0: {
            int onoff = read_int_range(&p, &n, 0, 1);
            (void)sqlite3_config(SQLITE_CONFIG_MEMSTATUS, onoff);
            break;
          }
          case 1: {
            int onoff = read_int_range(&p, &n, 0, 1);
            (void)sqlite3_config(SQLITE_CONFIG_COVERING_INDEX_SCAN, onoff);
            break;
          }
          case 2: {
            int onoff = read_int_range(&p, &n, 0, 1);
            (void)sqlite3_config(SQLITE_CONFIG_URI, onoff);
            break;
          }
          case 3: {
            int small = read_int_range(&p, &n, 0, 1);
            (void)sqlite3_config(SQLITE_CONFIG_SMALL_MALLOC, small);
            break;
          }
        }
        (void)sqlite3_initialize();
        break;
      }

      case 9: {
        int len1 = read_int_range(&p, &n, 0, (int)sizeof(randbuf));
        int len2 = read_int_range(&p, &n, -8, (int)sizeof(randbuf));
#ifdef SQLITE_TESTCTRL_PRNG_SAVE
        (void)sqlite3_test_control(SQLITE_TESTCTRL_PRNG_SAVE);
#endif
        sqlite3_randomness(len1, randbuf);
#ifdef SQLITE_TESTCTRL_PRNG_RESTORE
        (void)sqlite3_test_control(SQLITE_TESTCTRL_PRNG_RESTORE);
#endif
        sqlite3_randomness(len2, randbuf);
        break;
      }
    }
  }

  (void)sqlite3_shutdown();
  return 0;
}