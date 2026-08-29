// This fuzz driver is generated for library sqlite3, aiming to fuzz the following functions:
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
// sqlite3_os_init at sqlite3.c:29704:16 in sqlite3.h
// sqlite3_initialize at sqlite3.c:152449:16 in sqlite3.h
// sqlite3_os_end at sqlite3.c:29823:16 in sqlite3.h
// sqlite3_shutdown at sqlite3.c:152624:16 in sqlite3.h
// sqlite3_shutdown at sqlite3.c:152624:16 in sqlite3.h
// sqlite3_randomness at sqlite3.c:18421:17 in sqlite3.h
// sqlite3_config at sqlite3.c:152678:16 in sqlite3.h
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "sqlite3.h"

static int read_u32(const uint8_t *Data, size_t Size, size_t *Off, uint32_t *Out){
  if (*Off + 4 > Size) return 0;
  *Out = ((uint32_t)Data[*Off]) |
         ((uint32_t)Data[*Off + 1] << 8) |
         ((uint32_t)Data[*Off + 2] << 16) |
         ((uint32_t)Data[*Off + 3] << 24);
  *Off += 4;
  return 1;
}

static int read_i32(const uint8_t *Data, size_t Size, size_t *Off, int *Out){
  uint32_t v = 0;
  if (!read_u32(Data, Size, Off, &v)) return 0;
  *Out = (int)v;
  return 1;
}

static void write_dummy_file(const uint8_t *Data, size_t Size){
  FILE *fp = fopen("./dummy_file", "wb");
  if (!fp) return;
  if (Size) fwrite(Data, 1, Size, fp);
  fclose(fp);
}

int LLVMFuzzerTestOneInput_103(const uint8_t *Data, size_t Size){
  size_t off = 0;
  uint32_t steps = 0;
  int initialized = 0;
  int i;

  write_dummy_file(Data, Size);

  if (!read_u32(Data, Size, &off, &steps)) {
    steps = (uint32_t)Size;
  }
  steps = (steps % 64U) + 1U;

  for (i = 0; i < (int)steps; i++) {
    uint32_t op = 0;
    if (!read_u32(Data, Size, &off, &op)) {
      op = (uint32_t)i;
    }

    switch (op % 8U) {
      case 0: {
        int n = 0;
        uint32_t use_null = 0;
        void *p = NULL;
        unsigned char *buf = NULL;

        if (!read_i32(Data, Size, &off, &n)) {
          n = (int)(Size & 0x7f);
        }
        if (!read_u32(Data, Size, &off, &use_null)) {
          use_null = 0;
        }

        if ((use_null & 1U) == 0U) {
          size_t alloc_sz = 0;
          if (n > 0) {
            alloc_sz = (size_t)n;
            if (alloc_sz > 4096U) {
              alloc_sz = 4096U;
              n = (int)alloc_sz;
            }
          } else {
            n = 0;
          }

          if (alloc_sz > 0) {
            buf = (unsigned char *)malloc(alloc_sz);
            if (buf) {
              memset(buf, 0, alloc_sz);
              p = buf;
            } else {
              p = NULL;
              n = 0;
            }
          }
        }

        sqlite3_randomness(n, p);
        free(buf);
        break;
      }

      case 1: {
        int rc = sqlite3_config(SQLITE_CONFIG_SINGLETHREAD);
        (void)rc;
        break;
      }

      case 2: {
        int rc = sqlite3_config(SQLITE_CONFIG_MULTITHREAD);
        (void)rc;
        break;
      }

      case 3: {
        int rc = sqlite3_config(SQLITE_CONFIG_SERIALIZED);
        (void)rc;
        break;
      }

      case 4: {
        int rc = sqlite3_os_init();
        if (rc == SQLITE_OK) initialized = 1;
        break;
      }

      case 5: {
        int rc = sqlite3_initialize();
        if (rc == SQLITE_OK) initialized = 1;
        break;
      }

      case 6: {
        int rc = sqlite3_os_end();
        (void)rc;
        break;
      }

      case 7: {
        int rc = sqlite3_shutdown();
        if (rc == SQLITE_OK) initialized = 0;
        break;
      }
    }
  }

  if (initialized) {
    sqlite3_shutdown();
  }

  return 0;
}