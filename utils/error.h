#pragma once
#include "build/parser.tab.h"
#include "utils/common.h"

extern YYLTYPE* yylloc_ptr;
extern string src_file_name;

#define ss_assert(expr, args...)                                          \
  if (!(expr)) {                                                          \
    fprintf(stderr, "Compiling failed at:%s:%d:%s\n", __FILE__, __LINE__, \
            __PRETTY_FUNCTION__);                                         \
    cerr << src_file_name << ":" << yylloc_ptr->first_line << ":"         \
         << yylloc_ptr->first_column << " ";                              \
    ss_assert_helper(args);                                               \
    cerr << endl;                                                         \
    abort();                                                              \
  } static_assert(true, "")

template <typename T, typename... Args>
inline void ss_assert_helper(T&& t, Args&&... args);

template <typename... Args>
inline void ss_assert_helper(const string& s, Args&&... args);

template <typename... Args>
inline void ss_assert_helper();

template <typename... Args>
inline void ss_assert_helper() {}

template <typename T, typename... Args>
inline void ss_assert_helper(T&& t);

template <typename... Args>
inline void ss_assert_helper(const string& s);

template <typename T, typename... Args>
inline void ss_assert_helper(T&& t) {
  cerr << t;
}

template <typename... Args>
inline void ss_assert_helper(const string& s) {
  cerr << "\"" << s << "\"";
}

template <typename T, typename... Args>
inline void ss_assert_helper(T&& t, Args&&... args) {
  cerr << t;
  ss_assert_helper(std::forward<Args>(args)...);
}

template <typename... Args>
inline void ss_assert_helper(const string& s, Args&&... args) {
  cerr << "\"" << s << "\"";
  ss_assert_helper(std::forward<Args>(args)...);
}
