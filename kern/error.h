#pragma once
#include <cassert>

#define ss_assert(expr, fmt...)                                             \
  if (!(expr)) {                                                            \
    fprintf(stderr, "Compiling failed at:%s:%d:\n%s: ", __FILE__, __LINE__, \
            __PRETTY_FUNCTION__);                                           \
    fprintf(stderr, fmt);                                                   \
    exit(-1);                                                               \
  }
