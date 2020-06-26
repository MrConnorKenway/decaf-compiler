#include <FlexLexer.h>
#include <getopt.h>

#include <fstream>
#include <iostream>

#include "build/create_symbol_table_visitor.h"
#include "build/display_visitor.h"
#include "kern/static_semantic_analyser.h"
#include "kern/llvm_driver.h"

extern ast_node_ptr_t root;
yyFlexLexer* lexer_ptr = nullptr;
YYLTYPE* yylloc_ptr;
string src_file_name;

int yylex() { return lexer_ptr->yylex(); }

using std::cout;

int main(int argc, char** argv) {
  int opt;
  bool verbose = false;
  string output_path;
  while ((opt = getopt(argc, argv, "hvo:")) != -1) {
    switch (opt) {
      case 'v':
        verbose = true;
        break;
      case 'o':
        assert(optarg);
        output_path = optarg;
        break;
      case 'h':
      default:
        fprintf(stderr, "Usage: %s [-hv] <decaf source file>\n", argv[0]);
        exit(-1);
    }
  }

  if (argv[optind] == nullptr) {
    fprintf(
        stderr,
        "Mandatory argument(s) missing\nUsage: %s [-hv] <decaf source file>\n",
        argv[0]);
    exit(-1);
  }

  if (output_path.empty()) {
    output_path = "a.out";
  }

  std::ifstream f(argv[optind]);
  src_file_name = argv[optind];
  if (!f.is_open()) {
    std::cerr << "Failed to open file: " << argv[optind] << endl;
    exit(-1);
  }
  lexer_ptr = new yyFlexLexer(f, cout);

  yyparse();

  Create_symbol_table_visitor cv;
  root->accept(cv);

  static_semantic_analyser analyser(cv.global_symbol_table, verbose);
  analyser.analyse();

  llvm_driver ld(cv.global_symbol_table, output_path);
  ld.gen_llvm_ir();

  if (verbose) {
    vector<bool> is_last_bools;
    bool is_last = true;
    Display_visitor dv(is_last_bools, is_last);

    cout << endl << "Program" << endl;
    root->accept(dv);
  }

  return 0;
}
