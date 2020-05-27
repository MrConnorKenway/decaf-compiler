#pragma once
#include <iostream>
#include <optional>
#include <stack>
#include <string>
#include <variant>
#include <vector>

using std::cout;
using std::endl;
using std::optional;
using std::stack;
using std::string;
using std::variant;
using std::vector;

class Visitor;

class AST_node_base {
 public:
  string node_type = "";
  optional<string> expr_type = {};

  virtual void accept(Visitor&) = 0;
};

using ast_node_ptr_t = AST_node_base*;
