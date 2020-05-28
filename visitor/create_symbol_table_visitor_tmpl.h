#pragma once
#include "build/include/parser_handwritten.tab.h"
#include "build/include/visitor.h"
#include "utils/error.h"
#include "kern/symbol_table.h"

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
  enum class node_type { CLASS, FUNC, PROTOTYPE };
  stack<node_type> call_trace;

  string visit_and_get_id_of(ast_node_ptr_t node_ptr) {
    current_id = "";
    node_ptr->accept(*this);
    return current_id;
  }
};
