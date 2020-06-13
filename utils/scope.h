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
  var_table local_symbol_table;
  unordered_map<int, llvm::Value*> var_uid_to_llvm_value;
  scope* parent_scope_ptr{};
  vector<scope*> children_scope_ptr;

  static ssize_t next_uid;

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

  // use uid to find the llvm value to access outside scope's variable
  llvm::Value* lookup_llvm_value(int uid) const {
    auto iter = this;
    while (iter) {
      if (iter->var_uid_to_llvm_value.count(uid)) {
        return iter->var_uid_to_llvm_value.at(uid);
      }
      iter = iter->parent_scope_ptr;
    }

    ss_assert(false);
  }

  void load_symbol_table_from_class(const class_entry& ce) {
    // the first member variable is virtual table pointer
    // the second member variable is whole parent class variable (if this class
    // inherits from some other class)
    // member variables defined in this class start after the member variables of parent class
    // the uid of a member variable is determined by its order
    // in the output of for loop of `ce.field_table'
    local_symbol_table = ce.inheritance.field_table;
    next_uid = local_symbol_table.size();
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

