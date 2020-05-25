#include "build/include/static_semantic_analysis_visitor.h"

void Static_semantic_analysis_visitor::visit(List_node* list_node_ptr) {}

void Static_semantic_analysis_visitor::visit(This_node* this_node_ptr) {}

void Static_semantic_analysis_visitor::visit(
    Null_const_node* null_const_node_ptr) {}

void Static_semantic_analysis_visitor::visit(Empty_node* empty_node_ptr) {}

void Static_semantic_analysis_visitor::visit(
    Type_ident_node* type_ident_node_ptr) {}

void Static_semantic_analysis_visitor::visit(Ident_node* ident_node_ptr) {}

void Static_semantic_analysis_visitor::visit(
    Base_type_node* base_type_node_ptr) {}

void Static_semantic_analysis_visitor::visit(
    Array_type_node* array_type_node_ptr) {}

void Static_semantic_analysis_visitor::visit(
    User_defined_type_node* user_defined_type_node_ptr) {}

void Static_semantic_analysis_visitor::visit(
    Assignment_node* assignment_node_ptr) {}

void Static_semantic_analysis_visitor::visit(
    Binary_expr_node* binary_expr_node_ptr) {}

void Static_semantic_analysis_visitor::visit(
    Unary_expr_node* unary_expr_node_ptr) {}

void Static_semantic_analysis_visitor::visit(Read_op_node* read_op_node_ptr) {}

void Static_semantic_analysis_visitor::visit(New_op_node* new_op_node_ptr) {}

void Static_semantic_analysis_visitor::visit(
    New_array_op_node* new_array_op_node_ptr) {}

void Static_semantic_analysis_visitor::visit(Dot_op_node* dot_op_node_ptr) {}

void Static_semantic_analysis_visitor::visit(Index_op_node* index_op_node_ptr) {
}

void Static_semantic_analysis_visitor::visit(
    Int_const_node* int_const_node_ptr) {}

void Static_semantic_analysis_visitor::visit(
    Double_const_node* double_const_node_ptr) {}

void Static_semantic_analysis_visitor::visit(
    Str_const_node* str_const_node_ptr) {}

void Static_semantic_analysis_visitor::visit(
    Bool_const_node* bool_const_node_ptr) {}

void Static_semantic_analysis_visitor::visit(
    ClassDecl_node* classdecl_node_ptr) {}

void Static_semantic_analysis_visitor::visit(Variable_node* variable_node_ptr) {
}

void Static_semantic_analysis_visitor::visit(
    StmtBlock_node* stmtblock_node_ptr) {}

void Static_semantic_analysis_visitor::visit(
    FunctionDecl_node* functiondecl_node_ptr) {}

void Static_semantic_analysis_visitor::visit(
    Prototype_node* prototype_node_ptr) {}

void Static_semantic_analysis_visitor::visit(
    InterfaceDecl_node* interfacedecl_node_ptr) {}

void Static_semantic_analysis_visitor::visit(
    ReturnStmt_node* returnstmt_node_ptr) {}

void Static_semantic_analysis_visitor::visit(
    WhileStmt_node* whilestmt_node_ptr) {
  current_loop_node = whilestmt_node_ptr;
}

void Static_semantic_analysis_visitor::visit(Call_node* call_node_ptr) {}

void Static_semantic_analysis_visitor::visit(
    BreakStmt_node* breakstmt_node_ptr) {
  ss_assert(current_loop_node != nullptr,
            "Use break statement outside a loop\n");
  breakstmt_node_ptr->resident_loop_node = current_loop_node;
}

void Static_semantic_analysis_visitor::visit(
    ContinueStmt_node* continuestmt_node_ptr) {
  ss_assert(current_loop_node != nullptr,
            "Use continue statement outside a loop\n");
  continuestmt_node_ptr->resident_loop_node = current_loop_node;
}

void Static_semantic_analysis_visitor::visit(IfStmt_node* ifstmt_node_ptr) {}

void Static_semantic_analysis_visitor::visit(ForStmt_node* forstmt_node_ptr) {}

void Static_semantic_analysis_visitor::visit(
    PrintStmt_node* printstmt_node_ptr) {}
