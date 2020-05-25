#pragma once
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>

#include "build/include/ast.h"

using std::string;
using std::unordered_map;

using var_id = string;
using interface_id = string;
using class_id = string;
using func_id = string;
using var_type = string;
using var_table = unordered_map<var_id, var_type>;

struct func_entry {
  string return_type;
  var_table formal_table;
  std::optional<StmtBlock_node*> func_body;

  inline bool operator==(const func_entry& that) {
    return (this->return_type == that.return_type) &&
           (this->formal_table == that.formal_table);
  }
  inline bool operator!=(const func_entry& that) { return !(*this == that); }
};

using interface_entry = unordered_map<func_id, func_entry>;

struct Inheritance {
  std::unordered_set<interface_id> interface_ids;
  // map from a function to the id of the class that declares it
  std::unordered_map<func_id, class_id> func_decl_class;
  var_table field_table;
};

struct class_entry {
  unordered_map<func_id, func_entry> func_table;
  var_table field_table;
  class_id parent_class_id;
  Inheritance inheritance;
  std::unordered_set<interface_id> implemented_interface_set;
};

using symbol_table_entry = std::variant<class_entry, interface_entry>;
using symbol_table = unordered_map<string, symbol_table_entry>;