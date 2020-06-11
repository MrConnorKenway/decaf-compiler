#pragma once
#include "utils/common.h"

// this class is used to control indentation when
// printing the AST
class Indent {
 public:
  const string empty_style = "    ";
  const string short_inter_style = "├── ";
  const string long_inter_style = "│   ";
  const string end_style = "└── ";

  // is_last_bools[k] represents whether node at
  // level k is the last node in the sub-tree of
  // its parent
  vector<bool> &is_last_bools;
  // is_last: global variable used to indicate if
  // current node is the last node
  bool &is_last;

  Indent(vector<bool> &v, bool &b) : is_last_bools(v), is_last(b) {
    is_last_bools.push_back(is_last);
    is_last = false;
  }
  ~Indent() {
    is_last_bools.pop_back();
    is_last = false;
  }

  template <typename...Args>
  void indent(Args&&... args) {
    static_assert(sizeof...(args) > 0);
    auto it = is_last_bools.cbegin();
    for (; it != --is_last_bools.cend(); ++it) {
      if (*it) {
        cout << empty_style;
      } else {
        cout << long_inter_style;
      }
    }
    if (*it) {
      cout << end_style;
    } else {
      cout << short_inter_style;
    }
    (cout << ... << args) << endl;
  }
};
