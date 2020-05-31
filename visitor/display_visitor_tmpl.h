#pragma once
#include "build/visitor.h"
#include "build/parser.tab.h"
#include "utils/indent.h"

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
