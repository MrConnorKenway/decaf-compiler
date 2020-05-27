#pragma once
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>

#include "build/include/ast.h"

using std::string;
using std::unordered_map;
using std::unordered_set;

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

  inline bool operator==(const func_entry& that) const {
    return (this->return_type == that.return_type) &&
           (this->formal_table == that.formal_table);
  }
  inline bool operator!=(const func_entry& that) const {
    return !(*this == that);
  }
};

using interface_entry = unordered_map<func_id, func_entry>;

struct Inheritance {
  unordered_set<interface_id> interface_ids;
  // map from a function to the id of the class that declares it
  std::unordered_map<func_id, class_id> func_decl_class;
  var_table field_table;
};

struct class_entry {
  unordered_map<func_id, func_entry> func_table;
  var_table field_table;
  class_id parent_class_id;
  Inheritance inheritance;
  unordered_set<interface_id> implemented_interface_set;

  var_type try_fetch_variable(var_id vid) {
    ss_assert(field_table.count(vid) != 0,
              "Undefined reference to variable \"%s\"\n", vid.c_str());
    return field_table[vid];
  }
};

using symbol_table_entry = std::variant<class_entry, interface_entry>;

struct symbol_table : unordered_map<string, symbol_table_entry> {
  class_entry& try_fetch_class(string cid) {
    if (count(cid) == 0 || std::holds_alternative<interface_entry>(at(cid))) {
      ss_assert(false, "Undefined reference to class \"%s\"\n", cid.c_str());
    }
    return std::get<class_entry>(at(cid));
  }

  func_entry& try_fetch_func(string cid, string fid) {
    auto& ce = try_fetch_class(cid);
    ss_assert(ce.func_table.count(fid),
              "Undefined reference to function \"%s\" in class \"%s\"\n",
              fid.c_str(), cid.c_str());
    return ce.func_table[fid];
  }

  interface_entry& try_fetch_interface(string iid) {
    if (count(iid) == 0 || std::holds_alternative<class_entry>(at(iid))) {
      ss_assert(false, "Undefined reference to interface \"%s\"\n",
                iid.c_str());
    }
    return std::get<interface_entry>(at(iid));
  }
};