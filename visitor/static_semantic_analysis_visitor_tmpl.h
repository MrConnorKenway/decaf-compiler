#pragma once
#include "build/parser.tab.h"
#include "build/visitor.h"
#include "kern/symbol_table.h"
#include "utils/common.h"
#include "utils/error.h"

// "scope" encapsulates necessary information for
// static semantic analysis
class scope {
 public:
  // this symbol table only contains variable symbols
  using var_entry = std::pair<int, var_type>;
  std::unordered_map<var_id, var_entry> local_symbol_table;
  scope* parent_scope_ptr{};
  vector<scope*> children_scope_ptr;

  static int next_tid;

  void display(vector<bool>& is_last_bools, bool& is_last) const {
    {
      for (auto iter = local_symbol_table.cbegin();
           iter != local_symbol_table.cend(); ++iter) {
        auto [vid, ve] = *iter;
        auto [tid, vt] = ve;
        is_last = (std::next(iter) == local_symbol_table.cend()) &&
                  children_scope_ptr.empty();
        Indent i(is_last_bools, is_last);
        i.indent(vid + " " + std::to_string(tid) + " " + vt);
      }
    }
    if (children_scope_ptr.empty()) {
      return;
    }
    for (auto iter = children_scope_ptr.cbegin();
         iter != children_scope_ptr.cend(); ++iter) {
      is_last = std::next(iter) == children_scope_ptr.cend();
      Indent i(is_last_bools, is_last);
      i.indent("Scope");
      (*iter)->display(is_last_bools, is_last);
    }
  }

  var_entry lookup(const var_id& vid) const {
    auto iter = this;
    while (iter) {
      if (iter->local_symbol_table.count(vid)) {
        return iter->local_symbol_table.at(vid);
      }
      iter = iter->parent_scope_ptr;
    }

    ss_assert(false, "Undefined reference to variable ", vid);
  }

  void load_symbol_table_from_class(const class_entry& ce) {
    next_tid = 0;
    for (auto [vid, vt] : ce.inheritance.field_table) {
      try_insert(vid, vt);
    }
  }

  void load_symbol_table_from_func(const func_entry& fe) {
    for (auto [vid, vt] : fe.formal_table) {
      try_insert(vid, vt);
    }
  }

  void try_insert(const var_id& vid, var_type vt) {
    ss_assert(local_symbol_table.count(vid) == 0,
              "Multiple definition of variable ", vid);
    local_symbol_table.try_emplace(vid, next_tid++, vt);
  }

  scope() = default;

  explicit scope(const func_entry& fe) {
    load_symbol_table_from_func(fe);
  }

  explicit scope(const class_entry& ce) {
    load_symbol_table_from_class(ce);
  }
};

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
