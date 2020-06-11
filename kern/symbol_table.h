#pragma once
#include <unordered_map>
#include <map>
#include <unordered_set>

#include "build/ast.h"
#include "utils/error.h"
#include "utils/indent.h"

using std::ostream;
using std::string;
using std::unordered_map;
using std::unordered_set;

using var_id = string;
using interface_id = string;
using class_id = string;
using func_id = string;
using proto_id = string;
using var_type = string;
using var_table = unordered_map<var_id, var_type>;

struct func_entry {
  string return_type;
  vector<std::pair<var_id, var_type>> formal_table;
  std::optional<StmtBlock_node*> func_body;

  bool operator==(const func_entry& that) const {
    return (this->return_type == that.return_type) &&
           (this->formal_table == that.formal_table);
  }
  bool operator!=(const func_entry& that) const { return !(*this == that); }

  void display(const string& fid, vector<bool>& is_last_bools, bool& is_last) const;
};

struct interface_entry : unordered_map<func_id, func_entry> {
  void display(const string& iid, vector<bool>& is_last_bools, bool& is_last) const;
};

struct Inheritance {
  unordered_set<interface_id> interface_ids;
  // map from a prototype to the id of the interface that declares it
  std::unordered_map<proto_id, interface_id> proto_decl_interface;
  // map from a function to the id of the class that declares it
  std::map<func_id, class_id> func_decl_class;
  var_table field_table;
};

struct symbol_table;

struct class_entry {
  unordered_map<func_id, func_entry> func_table;
  var_table field_table;
  class_id parent_class_id;
  Inheritance inheritance;
  unordered_set<interface_id> implemented_interface_set;
  ClassDecl_node* classdecl_node_ptr;

  var_type try_fetch_variable(const var_id& vid) {
    ss_assert(field_table.count(vid) != 0, "Undefined reference to variable ",
              vid);
    return field_table[vid];
  }

  void display(const symbol_table& global_symbol_table, const string& cid,
               vector<bool>& is_last_bools, bool& is_last) const;
};

using symbol_table_entry = std::variant<class_entry, interface_entry>;

struct symbol_table : unordered_map<string, symbol_table_entry> {
  void check_return_type(string rt) {
    auto base_type = is_array_type(rt);
    if (base_type.has_value()) {
      // this is an array type
      rt = base_type.value();
    }
    if (unordered_set<string>({"int", "double", "string", "bool", "void"})
            .count(rt) != 0) {
      // is base type
      return;
    } else {
      try_fetch_class(rt);
    }
  }

  void check_var_type(string vt) {
    auto base_type = is_array_type(vt);
    if (base_type.has_value()) {
      // this is an array type
      vt = base_type.value();
    }
    if (unordered_set<string>({"int", "double", "string", "bool"}).count(vt) !=
        0) {
      // is base type
      return;
    } else {
      try_fetch_class(vt);
    }
  }

  class_entry& try_fetch_class(string cid) {
    auto base_type = is_array_type(cid);
    if (base_type.has_value()) {
      // this is an array type
      cid = base_type.value();
    }
    if (count(cid) == 0 || std::holds_alternative<interface_entry>(at(cid))) {
      ss_assert(false, "Undefined reference to class ", cid);
    }
    return std::get<class_entry>(at(cid));
  }

  const class_entry& try_fetch_class(const string& cid) const {
    if (count(cid) == 0 || std::holds_alternative<interface_entry>(at(cid))) {
      ss_assert(false, "Undefined reference to class ", cid);
    }
    return std::get<class_entry>(at(cid));
  }

  func_entry& try_fetch_func(const string& cid, const string& fid) {
    auto& ce = try_fetch_class(cid);
    ss_assert(ce.func_table.count(fid), "Undefined reference to function ", fid,
              " in class ", cid);
    return ce.func_table[fid];
  }

  const func_entry& try_fetch_func(const string& cid, const string& fid) const {
    auto const& ce = try_fetch_class(cid);
    ss_assert(ce.func_table.count(fid), "Undefined reference to function ", fid,
              " in class ", cid);
    return ce.func_table.at(fid);
  }

  interface_entry& try_fetch_interface(const string& iid) {
    if (count(iid) == 0 || std::holds_alternative<class_entry>(at(iid))) {
      ss_assert(false, "Undefined reference to interface ", iid);
    }
    return std::get<interface_entry>(at(iid));
  }

  void display(vector<bool>& is_last_bools, bool& is_last) const;
};