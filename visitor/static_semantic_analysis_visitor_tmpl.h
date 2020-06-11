#pragma once
#include "build/parser.tab.h"
#include "build/visitor.h"
#include "kern/symbol_table.h"
#include "utils/common.h"
#include "utils/error.h"
#include "utils/scope.h"

// This visitor defines operations to analyse
// the static semantics of input source file
class Static_semantic_analysis_visitor : public Visitor {
 public:
  ///$decl

  symbol_table& global_symbol_table;
  string current_id;
  string current_func_id;
  string current_class_id;
  enum class action_type { GET_ARGS_TYPE };
  stack<action_type> call_trace;
  ast_node_ptr_t current_loop_node;
  scope* current_scope_ptr;
  // this scope contains only the variable symbols
  // of current class
  scope class_scope;

  string visit_and_get_id_of(ast_node_ptr_t node_ptr) {
    current_id = "";
    node_ptr->accept(*this);
    return current_id;
  }

  // report error if node_ptr has no expr type
  string decl_type(ast_node_ptr_t node_ptr) {
    node_ptr->accept(*this);
    ss_assert(node_ptr->expr_type.has_value(), "Current node ",
              node_ptr->node_type, " has no expr type\n");
    return node_ptr->expr_type.value();
  }

  vector<string>& decl_type_list(List_node* list_node_ptr) {
    call_trace.push(action_type::GET_ARGS_TYPE);
    list_node_ptr->accept(*this);
    call_trace.pop();
    if (!list_node_ptr->elements_type_list.has_value()) {
      list_node_ptr->elements_type_list = vector<string>();
    }
    return list_node_ptr->elements_type_list.value();
  }

  Static_semantic_analysis_visitor(symbol_table& st, const class_entry& ce,
                                   string cid)
      : global_symbol_table(st),
        class_scope(ce),
        current_scope_ptr(&class_scope),
        current_class_id(std::move(cid)),
        current_func_id(),
        current_id(),
        call_trace(),
        current_loop_node() {}
};
