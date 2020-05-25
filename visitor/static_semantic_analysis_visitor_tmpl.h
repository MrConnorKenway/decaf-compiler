#pragma once
#include "build/include/create_symbol_table_visitor.h"
#include "build/include/visitor.h"
#include "kern/error.h"
#include "kern/symbol_table.h"

// This visitor defines operations to analyse
// the static semantics of input source file
class Static_semantic_analysis_visitor : public Visitor {
 public:
  ///$decl

  class_entry current_class_entry;
  func_entry current_func_entry;
  interface_entry current_interface_entry;
  symbol_table global_symbol_table;
  string current_id;
  vector<node_type> call_trace;
  ast_node_ptr_t current_loop_node;

  Static_semantic_analysis_visitor(symbol_table& st)
      : global_symbol_table(st) {}
};
