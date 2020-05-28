#pragma once
#include <unordered_map>
#include <unordered_set>

#include "build/include/ast.h"
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
using var_type = string;
using var_table = unordered_map<var_id, var_type>;

struct func_entry {
  string return_type;
  var_table formal_table;
  std::optional<StmtBlock_node*> func_body;

  bool operator==(const func_entry& that) const {
    return (this->return_type == that.return_type) &&
           (this->formal_table == that.formal_table);
  }
  bool operator!=(const func_entry& that) const { return !(*this == that); }

  void display(string fid, vector<bool>& is_last_bools, bool& is_last) const;
};

struct interface_entry : unordered_map<func_id, func_entry> {
  void display(string iid, vector<bool>& is_last_bools, bool& is_last) const;
};

struct Inheritance {
  unordered_set<interface_id> interface_ids;
  // map from a function to the id of the class that declares it
  std::unordered_map<func_id, class_id> func_decl_class;
  var_table field_table;
};

struct symbol_table;

struct class_entry {
  unordered_map<func_id, func_entry> func_table;
  var_table field_table;
  class_id parent_class_id;
  Inheritance inheritance;
  unordered_set<interface_id> implemented_interface_set;

  var_type try_fetch_variable(var_id vid) {
    ss_assert(field_table.count(vid) != 0, "Undefined reference to variable ",
              vid);
    return field_table[vid];
  }

  void display(const symbol_table& global_symbol_table, string cid,
               vector<bool>& is_last_bools, bool& is_last) const;
};

using symbol_table_entry = std::variant<class_entry, interface_entry>;

struct symbol_table : unordered_map<string, symbol_table_entry> {
  void check_return_type(string rt) {
    auto pos = rt.find("[");
    if (pos != string::npos) {
      // this is an array type
      rt = rt.substr(0, pos);
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
    auto pos = vt.find("[");
    if (pos != string::npos) {
      // this is an array type
      vt = vt.substr(0, pos);
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
    auto pos = cid.find("[");
    if (pos != string::npos) {
      // this is an array type
      cid = cid.substr(0, pos);
    }
    if (count(cid) == 0 || std::holds_alternative<interface_entry>(at(cid))) {
      ss_assert(false, "Undefined reference to class ", cid);
    }
    return std::get<class_entry>(at(cid));
  }

  const class_entry& try_fetch_class(string cid) const {
    if (count(cid) == 0 || std::holds_alternative<interface_entry>(at(cid))) {
      ss_assert(false, "Undefined reference to class ", cid);
    }
    return std::get<class_entry>(at(cid));
  }

  func_entry& try_fetch_func(string cid, string fid) {
    auto& ce = try_fetch_class(cid);
    ss_assert(ce.func_table.count(fid), "Undefined reference to function ", fid,
              " in class ", cid);
    return ce.func_table[fid];
  }

  const func_entry& try_fetch_func(string cid, string fid) const {
    auto const& ce = try_fetch_class(cid);
    ss_assert(ce.func_table.count(fid), "Undefined reference to function ", fid,
              " in class ", cid);
    return ce.func_table.at(fid);
  }

  interface_entry& try_fetch_interface(string iid) {
    if (count(iid) == 0 || std::holds_alternative<class_entry>(at(iid))) {
      ss_assert(false, "Undefined reference to interface ", iid);
    }
    return std::get<interface_entry>(at(iid));
  }

  void display(vector<bool>& is_last_bools, bool& is_last) const;
};