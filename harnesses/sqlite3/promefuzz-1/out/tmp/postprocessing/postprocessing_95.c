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

#include "sqlite3.h"

static uint32_t read_u32(const uint8_t **data, size_t *size) {
  uint32_t v = 0;
  size_t n = *size < 4 ? *size : 4;
  for (size_t i = 0; i < n; ++i) {
    v = (v << 8) | (*data)[i];
  }
  *data += n;
  *size -= n;
  return v;
}

static int read_bool(const uint8_t **data, size_t *size) {
  if (*size == 0) return 0;
  int v = (**data) & 1;
  (*data)++;
  (*size)--;
  return v;
}

static char *read_string(const uint8_t **data, size_t *size, size_t max_len) {
  size_t len = 0;
  if (*size > 0) {
    len = (**data) % (max_len + 1);
    (*data)++;
    (*size)--;
  }
  if (len > *size) len = *size;

  char *out = (char *)malloc(len + 1);
  if (!out) return NULL;

  for (size_t i = 0; i < len; ++i) {
    unsigned char c = (*data)[i];
    out[i] = (char)((c % 94) + 32);
  }
  out[len] = '\0';

  *data += len;
  *size -= len;
  return out;
}

static int choose_config_option(uint32_t x) {
  switch (x % 6) {
    case 0: return SQLITE_CONFIG_SINGLETHREAD;
    case 1: return SQLITE_CONFIG_MULTITHREAD;
    case 2: return SQLITE_CONFIG_SERIALIZED;
    case 3: return SQLITE_CONFIG_MEMSTATUS;
    case 4: return SQLITE_CONFIG_URI;
    default: return 0x7fffffff;
  }
}

static int choose_open_flags(uint32_t x) {
  int flags = 0;
  if (x & 1) flags |= SQLITE_OPEN_READONLY;
  if (x & 2) flags |= SQLITE_OPEN_READWRITE;
  if (x & 4) flags |= SQLITE_OPEN_CREATE;
  if (x & 8) flags |= SQLITE_OPEN_URI;
  if (x & 16) flags |= SQLITE_OPEN_MEMORY;
  if (x & 32) flags |= SQLITE_OPEN_NOMUTEX;
  if (x & 64) flags |= SQLITE_OPEN_FULLMUTEX;
  if (x & 128) flags |= SQLITE_OPEN_SHAREDCACHE;
  if (x & 256) flags |= SQLITE_OPEN_PRIVATECACHE;

  if ((flags & (SQLITE_OPEN_READONLY | SQLITE_OPEN_READWRITE)) == 0) {
    flags |= SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
  }
  return flags;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  const uint8_t *p = Data;
  size_t remaining = Size;
  sqlite3 *db = NULL;
  char *vfs_name1 = NULL;
  char *vfs_name2 = NULL;
  char *open_vfs_name = NULL;
  int initialized = 0;

  (void)sqlite3_threadsafe();

  if (read_bool(&p, &remaining)) {
    sqlite3_shutdown();
  }

  {
    int cfgopt = choose_config_option(read_u32(&p, &remaining));
    int rc;
    if (cfgopt == SQLITE_CONFIG_MEMSTATUS || cfgopt == SQLITE_CONFIG_URI) {
      int onoff = read_bool(&p, &remaining);
      rc = sqlite3_config(cfgopt, onoff);
      (void)rc;
    } else {
      rc = sqlite3_config(cfgopt);
      (void)rc;
    }
  }

  if (read_bool(&p, &remaining)) {
    if (sqlite3_initialize() == SQLITE_OK) {
      initialized = 1;
    }
  }

  vfs_name1 = read_string(&p, &remaining, 32);
  vfs_name2 = read_string(&p, &remaining, 32);
  open_vfs_name = read_string(&p, &remaining, 32);

  {
    sqlite3_vfs *default_vfs = sqlite3_vfs_find(NULL);
    sqlite3_vfs *named_vfs1 = sqlite3_vfs_find(vfs_name1 && vfs_name1[0] ? vfs_name1 : NULL);
    sqlite3_vfs *named_vfs2 = sqlite3_vfs_find(vfs_name2 && vfs_name2[0] ? vfs_name2 : NULL);

    if (default_vfs) {
      int make_default = read_bool(&p, &remaining);
      (void)sqlite3_vfs_register(default_vfs, make_default);
    }
    if (named_vfs1) {
      int make_default = read_bool(&p, &remaining);
      (void)sqlite3_vfs_register(named_vfs1, make_default);
    }
    if (named_vfs2) {
      int make_default = read_bool(&p, &remaining);
      (void)sqlite3_vfs_register(named_vfs2, make_default);
    }
  }

  {
    FILE *fp = fopen("./dummy_file", "wb");
    if (fp) {
      if (remaining > 0) {
        fwrite(p, 1, remaining, fp);
      }
      fclose(fp);
    }
  }

  {
    int flags = choose_open_flags(read_u32(&p, &remaining));
    const char *filename;
    const char *zvfs = NULL;

    switch (read_u32(&p, &remaining) % 4) {
      case 0:
        filename = "./dummy_file";
        break;
      case 1:
        filename = ":memory:";
        break;
      case 2:
        filename = "";
        break;
      default:
        filename = "./dummy_file";
        break;
    }

    switch (read_u32(&p, &remaining) % 4) {
      case 0:
        zvfs = NULL;
        break;
      case 1:
        zvfs = (open_vfs_name && open_vfs_name[0]) ? open_vfs_name : NULL;
        break;
      case 2:
        zvfs = (vfs_name1 && vfs_name1[0]) ? vfs_name1 : NULL;
        break;
      default:
        zvfs = (vfs_name2 && vfs_name2[0]) ? vfs_name2 : NULL;
        break;
    }

    (void)sqlite3_open_v2(filename, &db, flags, zvfs);
  }

  if (db) {
    sqlite3_close(db);
    db = NULL;
  }

  if (initialized && read_bool(&p, &remaining)) {
    sqlite3_shutdown();
  }

  free(vfs_name1);
  free(vfs_name2);
  free(open_vfs_name);
  return 0;
}