#pragma once
#include <unordered_map>
#include <variant>

#include "build/static_semantic_analysis_visitor.h"
#include "kern/symbol_table.h"
#include "utils/error.h"

struct static_semantic_analyser {
  std::unordered_map<string, bool> is_visited;
  symbol_table& global_symbol_table;
  bool verbose;

  void analyse(const string& id, class_entry& entry);
  void analyse();

  explicit static_semantic_analyser(symbol_table& st, bool v)
      : global_symbol_table(st), verbose(v), is_visited() {}
};
