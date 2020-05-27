#include <FlexLexer.h>

#include <fstream>
#include <iostream>

#include "build/include/create_symbol_table_visitor.h"
#include "build/include/display_visitor.h"
#include "build/include/static_semantic_analysis_visitor.h"
#include "kern/static_semantic_analyser.h"

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

  Create_symbol_table_visitor cv;
  root->accept(cv);

  static_semantic_analyser analyser(cv.global_symbol_table);
  analyser.analyse();

  if (false) {
    vector<bool> is_last_bools;
    bool is_last = true;
    Display_visitor dv(is_last_bools, is_last);

    cout << endl << "Program" << endl;
    root->accept(dv);
  }

  return 0;
}
