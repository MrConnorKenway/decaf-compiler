#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

typedef struct {
  unsigned len;
  char* c_str;
} decaf_str;

typedef struct {
  unsigned fid;
  void* f_ptr;
} v_entry;

typedef struct {
  unsigned len;
  v_entry* table;
} v_table;

void print_str(decaf_str* str) {
  printf("(%d)%s", str->len, str->c_str);
}

void print_bool(bool a_bool) {
  printf("%s", a_bool ? "true" : "false");
}

void print_int(int a_int) {
  printf("%d", a_int);
}

void print_double(double a_double) {
  printf("%lf", a_double);
}

int read_int() {
  int tmp;
  scanf("%d", &tmp);
  return tmp;
}

decaf_str* read_line() {
  size_t n = 100;
  char* buf = (char*) malloc(n);
  getline(&buf, &n, stdin);
  decaf_str* str = (decaf_str*) malloc(sizeof(decaf_str));
  str->c_str = buf;
  // len counts '\n'
  str->len = strlen(buf);
  return str;
}

void* lookup_fptr(v_table* vtbl, unsigned fid) {
  unsigned beg = 0, end = vtbl->len;
  while (beg != end) {
    unsigned mid = (beg + end) / 2, mid_fid = vtbl->table[mid].fid;
    if (fid == mid_fid) {
      return vtbl->table[mid].f_ptr;
    } else if (fid > mid_fid) {
      end = mid;
    } else {
      beg = mid + 1;
    }
  }

  assert(false);
}
