#pragma once

#define ss_assert(expr, fmt...) \
  if (!(expr)) {                \
    printf(fmt);                \
    exit(-1);                   \
  }
