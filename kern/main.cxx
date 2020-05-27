#include <FlexLexer.h>
#include <getopt.h>

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
  int opt;
  bool verbose = false;
  while ((opt = getopt(argc, argv, "hv")) != -1) {
    switch (opt) {
      case 'v':
        verbose = true;
        break;
      case 'h':
      default:
        exit(-1);
        fprintf(stderr, "Usage: %s [-hv] <decaf source file>\n", argv[0]);
    }
  }

  if (argv[optind] == nullptr) {
    fprintf(
        stderr,
        "Mandatory argument(s) missing\nUsage: %s [-hv] <decaf source file>\n",
        argv[0]);
    exit(-1);
  }

  std::ifstream f(argv[optind]);
  if (!f.is_open()) {
    std::cerr << "Failed to open file: " << argv[optind] << endl;
    exit(-1);
  }
  lexer_ptr = new yyFlexLexer(f, cout);

  yyparse();

  Create_symbol_table_visitor cv;
  root->accept(cv);

  static_semantic_analyser analyser(cv.global_symbol_table);
  analyser.analyse();

  if (verbose) {
    vector<bool> is_last_bools;
    bool is_last = true;
    Display_visitor dv(is_last_bools, is_last);

    cout << endl << "Program" << endl;
    root->accept(dv);
  }

  return 0;
}
