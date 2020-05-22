#pragma once
#include "ast/ast_base.h"

// $ special mark for python to locate
// the code above can be copied directly into ast.h
class List_node : public AST_node_base {
 public:
  vector<ast_node_ptr_t> list;
};

// special node to handle 'this' reference
class This_node : public AST_node_base {};

class Null_const_node : public AST_node_base {};

// this node is used to handle the case when
// an non-terminal produce a empty production
class Empty_node : public AST_node_base {};

class Type_ident_node : public AST_node_base {
 public:
  string type_ident_name;
};

class Ident_node : public AST_node_base {
 public:
  string ident_name;
};

// we declare type_node as ast_node_base in bison union
// so we have to introduce 3 new class to provide same functionality
class Base_type_node : public AST_node_base {
 public:
  int type;
};

class Array_type_node : public AST_node_base {
 public:
  ast_node_ptr_t base_type;
};

class User_defined_type_node : public AST_node_base {
 public:
  string type_ident_name;
};

// special node to handle assignment
class Assignment_node : public AST_node_base {
 public:
  ast_node_ptr_t LHS;
  ast_node_ptr_t RHS;
};

class Binary_expr_node : public AST_node_base {
 public:
  ast_node_ptr_t left_operand;
  int op;
  ast_node_ptr_t right_operand;
};

class Unary_expr_node : public AST_node_base {
 public:
  int op;
  ast_node_ptr_t operand;
};

// special node to handle 'ReadInteger' and 'ReadLine'
class Read_op_node : public AST_node_base {
 public:
  int read_type;
};

class New_op_node : public AST_node_base {
 public:
  ast_node_ptr_t alloc_obj_type;
};

class New_array_op_node : public AST_node_base {
 public:
  ast_node_ptr_t alloc_obj_type;
  ast_node_ptr_t array_size;
};

class Dot_op_node : public AST_node_base {
 public:
  ast_node_ptr_t obj;
  Ident_node* member_id;
};

class Index_op_node : public AST_node_base {
 public:
  ast_node_ptr_t array;
  ast_node_ptr_t index_expr;
};

// node that represents an int literal
class Int_const_node : public AST_node_base {
 public:
  int val;
};

// node that represents a double literal
class Double_const_node : public AST_node_base {
 public:
  double val;
};

// node that represents a string literal
class Str_const_node : public AST_node_base {
 public:
  string val;
};

// node that represents a boolean literal
class Bool_const_node : public AST_node_base {
 public:
  int val;
};

class ClassDecl_node : public AST_node_base {
 public:
  Type_ident_node* type_id;
  ast_node_ptr_t extender_optional;
  List_node* implementer_optional;
  List_node* field_list_optional;
};

class Variable_node : public AST_node_base {
 public:
  ast_node_ptr_t type_node;
  Ident_node* id;
};

class StmtBlock_node : public AST_node_base {
 public:
  List_node* stmt_list_optional;
};

class FunctionDecl_node : public AST_node_base {
 public:
  ast_node_ptr_t return_type;
  Ident_node* function_id;
  ast_node_ptr_t formals;
  StmtBlock_node* block;
};

class Prototype_node : public AST_node_base {
 public:
  ast_node_ptr_t return_type;
  Ident_node* prototype_id;
  ast_node_ptr_t formals;
};

class InterfaceDecl_node : public AST_node_base {
 public:
  Type_ident_node* type_id;
  List_node* prototype_list_optional;
};

class ReturnStmt_node : public AST_node_base {
 public:
  ast_node_ptr_t expr_optional;
};

class WhileStmt_node : public AST_node_base {
 public:
  ast_node_ptr_t condition;
  ast_node_ptr_t stmt;
};

class Call_node : public AST_node_base {
 public:
  ast_node_ptr_t actuals;
  Ident_node* function_id;
  ast_node_ptr_t obj_optional;
};

class BreakStmt_node : public AST_node_base {
 public:
  ast_node_ptr_t resident_loop_node;
};

class ContinueStmt_node : public AST_node_base {
 public:
  ast_node_ptr_t resident_loop_node;
};

class IfStmt_node : public AST_node_base {
 public:
  ast_node_ptr_t condition;
  ast_node_ptr_t stmt;
  ast_node_ptr_t else_stmt_optional;
};

class ForStmt_node : public AST_node_base {
 public:
  ast_node_ptr_t init_expr_optional;
  ast_node_ptr_t condition;
  ast_node_ptr_t step_expr_optional;
  ast_node_ptr_t stmt;
};

class PrintStmt_node : public AST_node_base {
 public:
  List_node* expr_list;
};
