#pragma once
#include <unordered_map>
#include <variant>

#include "build/include/static_semantic_analysis_visitor.h"
#include "kern/error.h"
#include "kern/symbol_table.h"

struct static_semantic_analyser {
  std::unordered_map<string, bool> is_visited;
  symbol_table& global_symbol_table;

  void visit(string id, symbol_table_entry& entry);
  void analyse(string id, interface_entry& entry);
  void analyse(string id, class_entry& entry);
  void analyse();

  class_entry& try_fetch_class(string cid);
  func_entry& try_fetch_func(string cid, string fid);
  interface_entry& try_fetch_interface(string iid);

  static_semantic_analyser(symbol_table& st) : global_symbol_table(st) {}
};
