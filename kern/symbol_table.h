#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>

#include "build/include/ast.h"

using std::string;
using std::unordered_map;

using var_id = string;
using var_type = string;
using var_table = unordered_map<var_id, var_type>;

struct func_entry {
  string return_type;
  var_table formal_table;
};

using interface_entry = unordered_map<string, func_entry>;

struct class_entry {
  unordered_map<string, func_entry> func_table;
  var_table field_table;
  string parent_class;
  std::unordered_set<string> implemented_interface_set;
};

using symbol_table_entry = std::variant<class_entry, interface_entry>;
using symbol_table = unordered_map<string, symbol_table_entry>;