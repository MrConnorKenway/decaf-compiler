#include <cassert>

#include "build/include/create_symbol_table_visitor.h"
#include "build/include/parser_handwritten.tab.h"

string Create_symbol_table_visitor::visit_and_get_id_of(
    ast_node_ptr_t node_ptr) {
  current_id = "";
  node_ptr->accept(*this);
  return current_id;
}

void Create_symbol_table_visitor::visit(List_node* list_node_ptr) {
  for (auto node_ptr : list_node_ptr->list) {
    node_ptr->accept(*this);
  }
}

void Create_symbol_table_visitor::visit(This_node* this_node_ptr) {}

void Create_symbol_table_visitor::visit(Null_const_node* null_const_node_ptr) {}

void Create_symbol_table_visitor::visit(Empty_node* empty_node_ptr) {}

void Create_symbol_table_visitor::visit(Type_ident_node* type_ident_node_ptr) {
  current_id = type_ident_node_ptr->type_ident_name;
}

void Create_symbol_table_visitor::visit(Ident_node* ident_node_ptr) {
  current_id = ident_node_ptr->ident_name;
}

void Create_symbol_table_visitor::visit(Base_type_node* base_type_node_ptr) {
  switch (base_type_node_ptr->type) {
    case t_int:
      current_id = "int";
      return;

    case t_bool:
      current_id = "bool";
      return;

    case t_double:
      current_id = "double";
      return;

    case t_string:
      current_id = "string";
      return;

    case t_void:
      current_id = "void";
      return;

    default:
      current_id = "unknown type code";
      return;
  }
}

void Create_symbol_table_visitor::visit(Array_type_node* array_type_node_ptr) {
  visit_and_get_id_of(array_type_node_ptr->base_type);
  current_id = "array of " + current_id;
}

void Create_symbol_table_visitor::visit(
    User_defined_type_node* user_defined_type_node_ptr) {
  current_id = user_defined_type_node_ptr->type_ident_name;
}

void Create_symbol_table_visitor::visit(Assignment_node* assignment_node_ptr) {}

void Create_symbol_table_visitor::visit(
    Binary_expr_node* binary_expr_node_ptr) {}

void Create_symbol_table_visitor::visit(Unary_expr_node* unary_expr_node_ptr) {}

void Create_symbol_table_visitor::visit(Read_op_node* read_op_node_ptr) {}

void Create_symbol_table_visitor::visit(New_op_node* new_op_node_ptr) {}

void Create_symbol_table_visitor::visit(
    New_array_op_node* new_array_op_node_ptr) {}

void Create_symbol_table_visitor::visit(Dot_op_node* dot_op_node_ptr) {}

void Create_symbol_table_visitor::visit(Index_op_node* index_op_node_ptr) {}

void Create_symbol_table_visitor::visit(Int_const_node* int_const_node_ptr) {}

void Create_symbol_table_visitor::visit(
    Double_const_node* double_const_node_ptr) {}

void Create_symbol_table_visitor::visit(Str_const_node* str_const_node_ptr) {}

void Create_symbol_table_visitor::visit(Bool_const_node* bool_const_node_ptr) {}

void Create_symbol_table_visitor::visit(ClassDecl_node* classdecl_node_ptr) {
  call_trace.push_back(node_type::CLASS);

  current_class_entry.parent_class =
      visit_and_get_id_of(classdecl_node_ptr->extender_optional);

  for (auto node_ptr : classdecl_node_ptr->implementer_optional->list) {
    current_class_entry.implemented_interface_set.insert(
        visit_and_get_id_of(node_ptr));
  }

  classdecl_node_ptr->field_list_optional->accept(*this);

  global_symbol_table.emplace(classdecl_node_ptr->type_id->type_ident_name,
                              std::move(current_class_entry));
  current_class_entry = class_entry();
}

void Create_symbol_table_visitor::visit(Variable_node* variable_node_ptr) {
  string tmp_id = variable_node_ptr->id->ident_name;
  // note that current_id has become "children's id" after the following line
  visit_and_get_id_of(variable_node_ptr->type_node);

  // this function may be called by a classdecl node or a functiondecl node
  switch (*call_trace.end()) {
    case node_type::CLASS:
      current_class_entry.field_table.emplace(tmp_id, current_id);
      break;

    case node_type::FUNC:
    case node_type::PROTOTYPE:
      current_func_entry.formal_table.emplace(tmp_id, current_id);
      break;

    default:
      // there shouldn't be other node type
      assert(0);
  }

  current_id = tmp_id;
}

void Create_symbol_table_visitor::visit(StmtBlock_node* stmtblock_node_ptr) {}

void Create_symbol_table_visitor::visit(
    FunctionDecl_node* functiondecl_node_ptr) {
  call_trace.push_back(node_type::FUNC);

  current_func_entry.return_type =
      visit_and_get_id_of(functiondecl_node_ptr->return_type);

  functiondecl_node_ptr->formals->accept(*this);

  current_class_entry.func_table.emplace(
      functiondecl_node_ptr->function_id->ident_name,
      std::move(current_func_entry));
  current_func_entry = func_entry();
}

void Create_symbol_table_visitor::visit(Prototype_node* prototype_node_ptr) {
  call_trace.push_back(node_type::PROTOTYPE);

  current_func_entry.return_type =
      visit_and_get_id_of(prototype_node_ptr->return_type);

  prototype_node_ptr->formals->accept(*this);

  current_interface_entry.emplace(prototype_node_ptr->prototype_id->ident_name,
                                  std::move(current_func_entry));
  current_func_entry = func_entry();
}

void Create_symbol_table_visitor::visit(
    InterfaceDecl_node* interfacedecl_node_ptr) {
  interfacedecl_node_ptr->prototype_list_optional->accept(*this);

  global_symbol_table.emplace(interfacedecl_node_ptr->type_id->type_ident_name,
                              std::move(current_interface_entry));
  current_interface_entry = interface_entry();
}

void Create_symbol_table_visitor::visit(ReturnStmt_node* returnstmt_node_ptr) {}

void Create_symbol_table_visitor::visit(WhileStmt_node* whilestmt_node_ptr) {}

void Create_symbol_table_visitor::visit(Call_node* call_node_ptr) {}

void Create_symbol_table_visitor::visit(BreakStmt_node* breakstmt_node_ptr) {}

void Create_symbol_table_visitor::visit(
    ContinueStmt_node* continuestmt_node_ptr) {}

void Create_symbol_table_visitor::visit(IfStmt_node* ifstmt_node_ptr) {}

void Create_symbol_table_visitor::visit(ForStmt_node* forstmt_node_ptr) {}

void Create_symbol_table_visitor::visit(PrintStmt_node* printstmt_node_ptr) {}
