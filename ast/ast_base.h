#pragma once
#include "utils/common.h"

struct YYLTYPE;
class Visitor;

class AST_node_base {
 public:
  string node_type = "";
  optional<string> expr_type = {};
  YYLTYPE* yylloc_ptr;

  virtual void accept(Visitor&) = 0;
};

using ast_node_ptr_t = AST_node_base*;
