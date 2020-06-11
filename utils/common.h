#pragma once
#include <iostream>
#include <optional>
#include <stack>
#include <string>
#include <variant>
#include <vector>

using std::cout;
using std::cerr;
using std::endl;
using std::optional;
using std::stack;
using std::string;
using std::variant;
using std::vector;

// check if type is array type, if so, return its base type
// else return nullopt
inline optional<string> is_array_type(const string& type) {
  auto pos = type.find('[');
  if (pos == string::npos) {
    return {};
  } else {
    return type.substr(0, pos);
  }
}
