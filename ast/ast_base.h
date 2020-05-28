#pragma once
#include "utils/common.h"

class Visitor;

class AST_node_base {
 public:
  string node_type = "";
  optional<string> expr_type = {};

  virtual void accept(Visitor&) = 0;
};

using ast_node_ptr_t = AST_node_base*;
