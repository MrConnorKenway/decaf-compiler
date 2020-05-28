#pragma once
#include "build/include/parser_handwritten.tab.h"
#include "utils/common.h"

extern YYLTYPE* yylloc_ptr;
extern string src_file_name;

#define ss_assert(expr, args...)                                            \
  if (!(expr)) {                                                            \
    fprintf(stderr, "Compiling failed at:%s:%d:%s\n", __FILE__, __LINE__, \
            __PRETTY_FUNCTION__);                                           \
    ss_assert_helper(args);                                                 \
    exit(-1);                                                               \
  }

template <typename... Args>
inline void ss_assert_helper(Args&&... args) {
  (cerr << src_file_name << ":" << yylloc_ptr->first_line << ":"
        << yylloc_ptr->first_column << " " << ... << args)
      << endl;  // since c++ 17
}
