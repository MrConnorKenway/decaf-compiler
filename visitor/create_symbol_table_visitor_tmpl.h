#pragma once
#include <iostream>
#include <vector>

#include "build/include/visitor.h"
#include "kern/symbol_table.h"

using std::vector;

enum class node_type { CLASS, FUNC, PROTOTYPE };

// This visitor defines operations to build
// global symbol table in the first pass of
// traverse of AST
class Create_symbol_table_visitor : public Visitor {
 public:
  ///$decl

  class_entry current_class_entry;
  func_entry current_func_entry;
  interface_entry current_interface_entry;
  symbol_table global_symbol_table;
  string current_id;
  vector<node_type> call_trace;

  string visit_and_get_id_of(ast_node_ptr_t node_ptr);
};