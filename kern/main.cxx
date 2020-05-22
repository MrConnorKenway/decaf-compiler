#include <FlexLexer.h>

#include <fstream>
#include <iostream>

#include "build/include/create_symbol_table_visitor.h"
#include "build/include/display_visitor.h"

extern ast_node_ptr_t root;
yyFlexLexer* lexer_ptr = nullptr;

int yyparse();
int yylex() { return lexer_ptr->yylex(); }
string get_yytext() { return lexer_ptr->YYText(); }

using std::cout;

int main(int argc, char** argv) {
  std::ifstream f;
  ++argv, --argc; /* skip over program name */
  if (argc > 0) {
    if (argc > 1) {
      std::cerr << "Warning: ignore extra arguments" << endl;
    }
    f.open(argv[0]);
    lexer_ptr = new yyFlexLexer(f, cout);
  } else {
    lexer_ptr = new yyFlexLexer();
  }

  yyparse();

  vector<bool> is_last_bools;
  bool is_last = true;
  Display_visitor display_visitor(is_last_bools, is_last);

  cout << "Program" << endl;
  root->accept(display_visitor);

  Create_symbol_table_visitor create_symbol_table_visitor;
  root->accept(create_symbol_table_visitor);

  return 0;
}
