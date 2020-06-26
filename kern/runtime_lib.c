#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

typedef struct {
  size_t len;
  char* c_str;
} decaf_str;

typedef union {
  int64_t placeholder;
} obj_ref;

typedef struct {
  size_t len;
  size_t element_size;
  obj_ref* elements;
} decaf_arr;

typedef struct {
  unsigned fid;
  void* f_ptr;
} v_entry;

typedef struct {
  size_t len;
  v_entry* table;
} v_table;

void print_str(decaf_str* str) {
  printf("%s", str->c_str);
}

void print_str_literal(char *str) {
  printf("%s", str);
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

void validate_access(decaf_arr* arr, size_t i) {
  if (i >= arr->len) {
    fprintf(stderr, "Array index out of bounds\n");
    exit(-1);
  }
}

void* alloc_obj(size_t obj_size) {
  return malloc(obj_size);
}

decaf_str* create_str_from_literal(char* c_str) {
  decaf_str* str = (decaf_str*) malloc(sizeof(decaf_str));
  str->len = strlen(c_str);
  str->c_str = (char*) malloc(str->len);
  str->c_str = strcpy(str->c_str, c_str);
  return str;
}

decaf_arr* alloc_arr(size_t len, size_t element_size) {
  decaf_arr* arr = (decaf_arr*) malloc(sizeof(decaf_arr));
  arr->len = len;
  arr->element_size = element_size;
  arr->elements = (obj_ref*) malloc(len * sizeof(obj_ref));
  return arr;
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
