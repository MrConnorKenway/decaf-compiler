#include <cassert>
#include <iostream>

#include "build/include/display_visitor.h"

int yyparse();

extern ast_node_ptr_t root;

using std::cout;

int main(int argc, char **argv) {
  ++argv, --argc; /* skip over program name */
  if (argc > 0) {
    assert(argc == 1);
    freopen(argv[0], "r", stdin);
  }

  yyparse();

  vector<bool> is_last_bools;
  bool is_last = true;
  Display_visitor v(is_last_bools, is_last);

  cout << "Program" << endl;
  root->accept(v);

  return 0;
}