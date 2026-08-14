#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <sqlite3.h>

static uint32_t read_u32(const uint8_t *Data, size_t Size, size_t *Offset) {
  uint32_t v = 0;
  size_t i;
  for (i = 0; i < 4 && *Offset < Size; ++i, ++(*Offset)) {
    v = (v << 8) | Data[*Offset];
  }
  return v;
}

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  sqlite3 *db = NULL;
  sqlite3_str *s1 = NULL;
  sqlite3_str *s2 = NULL;
  char *finished = NULL;
  size_t off = 0;
  int rc;

  FILE *fp = fopen("./dummy_file", "wb");
  if (fp != NULL) {
    if (Size > 0) {
      fwrite(Data, 1, Size, fp);
    }
    fclose(fp);
  }

  rc = sqlite3_open("./dummy_file", &db);
  if (rc != SQLITE_OK) {
    if (db) {
      sqlite3_close(db);
    }
    db = NULL;
  }

  s1 = sqlite3_str_new(db);
  s2 = sqlite3_str_new(NULL);

  while (off < Size) {
    uint8_t op = Data[off++];

    switch (op % 12) {
      case 0: {
        sqlite3_str_appendchar(s1, 0, 'A');
        break;
      }
      case 1: {
        int n = (int)(read_u32(Data, Size, &off) % 1024);
        char c = (off < Size) ? (char)Data[off++] : '\0';
        sqlite3_str_appendchar(s1, n, c);
        break;
      }
      case 2: {
        int n = -(int)(read_u32(Data, Size, &off) % 1024);
        char c = (off < Size) ? (char)Data[off++] : '\0';
        sqlite3_str_appendchar(s1, n, c);
        break;
      }
      case 3: {
        (void)sqlite3_str_value(s1);
        (void)sqlite3_str_length(s1);
        break;
      }
      case 4: {
        sqlite3_str_reset(s1);
        break;
      }
      case 5: {
        finished = sqlite3_str_finish(s1);
        sqlite3_free(finished);
        s1 = sqlite3_str_new(db);
        break;
      }
      case 6: {
        int n = (int)(read_u32(Data, Size, &off) % 256);
        char c = (off < Size) ? (char)Data[off++] : 'Z';
        sqlite3_str_appendchar(s2, n, c);
        (void)sqlite3_str_value(s2);
        (void)sqlite3_str_length(s2);
        break;
      }
      case 7: {
        sqlite3_str_reset(s2);
        break;
      }
      case 8: {
        finished = sqlite3_str_finish(s2);
        sqlite3_free(finished);
        s2 = sqlite3_str_new(NULL);
        break;
      }
      case 9: {
        (void)sqlite3_str_value(NULL);
        (void)sqlite3_str_length(NULL);
        finished = sqlite3_str_finish(NULL);
        sqlite3_free(finished);
        break;
      }
      case 10: {
        int n1 = (int)(read_u32(Data, Size, &off) % 4096);
        int n2 = (int)(read_u32(Data, Size, &off) % 4096);
        char c1 = (off < Size) ? (char)Data[off++] : 'x';
        char c2 = (off < Size) ? (char)Data[off++] : 'y';
        sqlite3_str_appendchar(s1, n1, c1);
        sqlite3_str_appendchar(s1, n2, c2);
        (void)sqlite3_str_value(s1);
        break;
      }
      case 11: {
        int n = (int)(read_u32(Data, Size, &off) % 8192);
        char c = (off < Size) ? (char)Data[off++] : (char)0xff;
        sqlite3_str_appendchar(s1, n, c);
        sqlite3_str_reset(s1);
        sqlite3_str_appendchar(s1, n / 2, (char)(c ^ 0x5a));
        (void)sqlite3_str_length(s1);
        break;
      }
    }
  }

  finished = sqlite3_str_finish(s1);
  sqlite3_free(finished);

  finished = sqlite3_str_finish(s2);
  sqlite3_free(finished);

  if (db) {
    sqlite3_close(db);
  }

  return 0;
}