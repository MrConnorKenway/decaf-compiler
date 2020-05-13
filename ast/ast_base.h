#pragma once
#include <iostream>
#include <string>
#include <vector>

#include "../visitor/visitor.h"

using std::cout;
using std::endl;
using std::string;
using std::vector;

class AST_node_base {
 public:
  string node_type;

  virtual void accept(Visitor&) = 0;
};

using ast_node_ptr_t = AST_node_base*;
