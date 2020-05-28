#include "ast/ast_base.h"

extern YYLTYPE* yylloc_ptr;

class yylloc_manager {
 public:
  ast_node_ptr_t node_ptr;
  YYLTYPE* old_yylloc_ptr;

  yylloc_manager(ast_node_ptr_t _node_ptr) : node_ptr(_node_ptr) {
    old_yylloc_ptr = yylloc_ptr;
    yylloc_ptr = node_ptr->yylloc_ptr;
  }

  ~yylloc_manager() { yylloc_ptr = old_yylloc_ptr; }
};