#pragma once
#include "build/include/visitor.h"

// this class is used for display ast tree
class Display_visitor : public Visitor {
 public:
  vector<bool> &is_last_bools;
  bool &is_last;

  Display_visitor(vector<bool> &v, bool &b) : is_last_bools(v), is_last(b) {}
  static string op_code_to_str(int op_code);
  static string base_type_code_to_str(int type_code);
  ///$decl
};

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

  template <typename T>
  void indent(T t) {
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
    cout << t << endl;
  }
};