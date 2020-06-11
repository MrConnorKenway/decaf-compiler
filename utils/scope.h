#pragma once
#include <llvm/IR/Value.h>
#include "kern/symbol_table.h"
#include "utils/common.h"
#include "utils/error.h"

// "scope" encapsulates necessary information for
// static semantic analysis and code generation procedure
class scope {
 public:
  // this symbol table only contains variable symbols
  using var_entry = std::pair<int, var_type>;
  unordered_map<var_id, var_entry> local_symbol_table;
  unordered_map<int, llvm::Value*> var_uid_to_llvm_value;
  scope* parent_scope_ptr{};
  vector<scope*> children_scope_ptr;

  static int next_uid;

  void display(vector<bool>& is_last_bools, bool& is_last) const {
    {
      for (auto iter = local_symbol_table.cbegin();
           iter != local_symbol_table.cend(); ++iter) {
        auto[vid, ve] = *iter;
        auto[uid, vt] = ve;
        is_last = (std::next(iter) == local_symbol_table.cend()) &&
            children_scope_ptr.empty();
        Indent i(is_last_bools, is_last);
        i.indent(vid, " ", std::to_string(uid), " ", vt);
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

  const var_entry& lookup(const var_id& vid) const {
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
    next_uid = 0;
    for (auto[vid, vt] : ce.inheritance.field_table) {
      try_insert(vid, vt);
    }
  }

  void load_symbol_table_from_func(const func_entry& fe) {
    for (auto[vid, vt] : fe.formal_table) {
      try_insert(vid, vt);
    }
  }

  void try_insert(const var_id& vid, var_type vt) {
    ss_assert(local_symbol_table.count(vid) == 0,
              "Multiple definition of variable ", vid);
    local_symbol_table.try_emplace(vid, next_uid++, vt);
  }

  scope() = default;

  explicit scope(const func_entry& fe) {
    load_symbol_table_from_func(fe);
  }

  explicit scope(const class_entry& ce) {
    load_symbol_table_from_class(ce);
  }
};

