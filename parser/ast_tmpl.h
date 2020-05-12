#pragma once
#include <iostream>
#include <string>
#include <vector>

using std::cout;
using std::endl;
using std::string;
using std::vector;

class StmtBlock_node;
class Extender_node;
class Implementor_node;

class AST_node_base {
 public:
  string node_type;
};

using ast_node_ptr_t = AST_node_base*;

class List_node : public AST_node_base {
 public:
  vector<ast_node_ptr_t> list;
};

class Type_ident_node : public AST_node_base {
 public:
  string type_ident_name;

  Type_ident_node(string _type_ident_name) : type_ident_name(_type_ident_name) {}
};

class Ident_node : public AST_node_base {
 public:
  string ident_name;

  Ident_node(string _ident_name) : ident_name(_ident_name) {}
};

// the code above can be copied directly into ast.h
class Extender_node : public AST_node_base {
 public:
  Type_ident_node* type_id;
};

class Implementor_node : public AST_node_base {
 public:
  List_node* type_ident_list;
};

class ClassDecl_node : public AST_node_base {
 public:
  string class_name;
  Type_ident_node* type_id;
  Extender_node* extender;
  Implementor_node* implementer;
};

class Type_node : public AST_node_base {
 public:
  string type_name;
};

class ArrayType_node : public AST_node_base {
 public:
  string type_name;
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
  List_node* formals;
  StmtBlock_node* block;
};

class Prototype_node : public AST_node_base {
 public:
  ast_node_ptr_t return_type;
  Ident_node* prototype_id;
  List_node* formals;
  StmtBlock_node* block;
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
  List_node* actuals;
  Ident_node* function_id;
  ast_node_ptr_t expr_optional;
};

class BreakStmt_node : public AST_node_base {
 public:
  ast_node_ptr_t expr_optional;
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
