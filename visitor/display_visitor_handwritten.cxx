#include "build/include/display_visitor.h"
#include "build/include/parser_handwritten.tab.h"

string Display_visitor::op_code_to_str(int op_code) {
  switch (op_code) {
    case '+':

    case '-':

    case '*':

    case '/':

    case '<':

    case '>':

    case '=':

    case '%':

    case '!':
      return string(1, op_code);

    case t_eq:
      return "==";

    case t_greater_eq:
      return ">=";

    case t_less_eq:
      return "<=";

    case t_not_eq:
      return "!=";

    case t_and:
      return "&&";

    case t_or:
      return "||";

    default:
      return "unknown op code";
  }
}

string Display_visitor::base_type_code_to_str(int type_code) {
  switch (type_code) {
    case t_int:
      return "int";

    case t_bool:
      return "bool";

    case t_double:
      return "double";

    case t_string:
      return "string";

    case t_void:
      return "void";

    default:
      return "unknown type code";
  }
}

void Display_visitor::visit(List_node* list_node_ptr) {
  Indent i(is_last_bools, is_last);
  i.indent(list_node_ptr->node_type);
  for (auto iter = list_node_ptr->list.cbegin();
       iter != list_node_ptr->list.cend(); ++iter) {
    is_last = iter == std::prev(list_node_ptr->list.cend());
    (*iter)->accept(*this);
  }
}

void Display_visitor::visit(This_node* this_node_ptr) {
  Indent i(is_last_bools, is_last);
  i.indent(this_node_ptr->node_type);
}

void Display_visitor::visit(Null_const_node* null_const_node_ptr) {
  Indent i(is_last_bools, is_last);
  i.indent(null_const_node_ptr->node_type);
}

void Display_visitor::visit(Empty_node* empty_node_ptr) {
  Indent i(is_last_bools, is_last);
  i.indent(empty_node_ptr->node_type);
}

void Display_visitor::visit(Ident_node* ident_node_ptr) {
  Indent i(is_last_bools, is_last);
  i.indent(ident_node_ptr->node_type + " " + ident_node_ptr->ident_name);
}

void Display_visitor::visit(Base_type_node* base_type_node_ptr) {
  Indent i(is_last_bools, is_last);
  i.indent(base_type_node_ptr->node_type +
           " type: " + base_type_code_to_str(base_type_node_ptr->type));
  ;
}

void Display_visitor::visit(Array_type_node* array_type_node_ptr) {
  Indent i(is_last_bools, is_last);
  i.indent(array_type_node_ptr->node_type);
  is_last = true;
  array_type_node_ptr->base_type->accept(*this);
}

void Display_visitor::visit(
    User_defined_type_node* user_defined_type_node_ptr) {
  Indent i(is_last_bools, is_last);
  i.indent(user_defined_type_node_ptr->node_type + " " +
           user_defined_type_node_ptr->type_ident_name);
}

void Display_visitor::visit(Assignment_node* assignment_node_ptr) {
  Indent i(is_last_bools, is_last);
  i.indent(assignment_node_ptr->node_type);
  assignment_node_ptr->LHS->accept(*this);
  is_last = true;
  assignment_node_ptr->RHS->accept(*this);
}

void Display_visitor::visit(Binary_expr_node* binary_expr_node_ptr) {
  Indent i(is_last_bools, is_last);
  i.indent(binary_expr_node_ptr->node_type +
           " op: " + op_code_to_str(binary_expr_node_ptr->op));
  binary_expr_node_ptr->left_operand->accept(*this);
  is_last = true;
  binary_expr_node_ptr->right_operand->accept(*this);
}

void Display_visitor::visit(Unary_expr_node* unary_expr_node_ptr) {
  Indent i(is_last_bools, is_last);
  i.indent(unary_expr_node_ptr->node_type +
           " op: " + op_code_to_str(unary_expr_node_ptr->op));
  is_last = true;
  unary_expr_node_ptr->operand->accept(*this);
}

void Display_visitor::visit(Read_op_node* read_op_node_ptr) {
  Indent i(is_last_bools, is_last);
  if (read_op_node_ptr->read_type == t_ReadInteger) {
    i.indent(read_op_node_ptr->node_type + " Read type: Read Integer");
  } else {
    i.indent(read_op_node_ptr->node_type + " Read type: Read Line");
  }
}

void Display_visitor::visit(New_op_node* new_op_node_ptr) {
  Indent i(is_last_bools, is_last);
  i.indent(new_op_node_ptr->node_type);
  is_last = true;
  new_op_node_ptr->alloc_obj_type->accept(*this);
}

void Display_visitor::visit(New_array_op_node* new_array_op_node_ptr) {
  Indent i(is_last_bools, is_last);
  i.indent(new_array_op_node_ptr->node_type);
  new_array_op_node_ptr->alloc_obj_type->accept(*this);
  is_last = true;
  new_array_op_node_ptr->array_size->accept(*this);
}

void Display_visitor::visit(Dot_op_node* dot_op_node_ptr) {
  Indent i(is_last_bools, is_last);
  i.indent(dot_op_node_ptr->node_type);
  dot_op_node_ptr->obj->node_type = "Object:";
  dot_op_node_ptr->obj->accept(*this);
  is_last = true;
  dot_op_node_ptr->member_id->node_type = "Member:";
  dot_op_node_ptr->member_id->accept(*this);
}

void Display_visitor::visit(Index_op_node* index_op_node_ptr) {
  Indent i(is_last_bools, is_last);
  i.indent(index_op_node_ptr->node_type);
  index_op_node_ptr->array->accept(*this);
  is_last = true;
  index_op_node_ptr->index_expr->accept(*this);
}

void Display_visitor::visit(Int_const_node* int_const_node_ptr) {
  Indent i(is_last_bools, is_last);
  i.indent(int_const_node_ptr->node_type +
           " val: " + std::to_string(int_const_node_ptr->val));
}

void Display_visitor::visit(Double_const_node* double_const_node_ptr) {
  Indent i(is_last_bools, is_last);
  i.indent(double_const_node_ptr->node_type +
           " val: " + std::to_string(double_const_node_ptr->val));
}

void Display_visitor::visit(Str_const_node* str_const_node_ptr) {
  Indent i(is_last_bools, is_last);
  i.indent(str_const_node_ptr->node_type + " val: " + str_const_node_ptr->val);
}

void Display_visitor::visit(Bool_const_node* bool_const_node_ptr) {
  Indent i(is_last_bools, is_last);
  i.indent(bool_const_node_ptr->node_type +
           " val: " + std::to_string(bool_const_node_ptr->val));
}

void Display_visitor::visit(ClassDecl_node* classdecl_node_ptr) {
  Indent i(is_last_bools, is_last);
  i.indent(classdecl_node_ptr->node_type);
  classdecl_node_ptr->type_id->accept(*this);
  classdecl_node_ptr->extender_optional->node_type = "Extender node: ";
  classdecl_node_ptr->extender_optional->accept(*this);
  classdecl_node_ptr->implementer_optional->node_type = "Implementer node: ";
  classdecl_node_ptr->implementer_optional->accept(*this);
  is_last = true;
  classdecl_node_ptr->field_list_optional->node_type = "FieldList_node";
  classdecl_node_ptr->field_list_optional->accept(*this);
}

void Display_visitor::visit(Variable_node* variable_node_ptr) {
  Indent i(is_last_bools, is_last);
  i.indent(variable_node_ptr->node_type);
  variable_node_ptr->type_node->accept(*this);
  is_last = true;
  variable_node_ptr->id->accept(*this);
}

void Display_visitor::visit(StmtBlock_node* stmtblock_node_ptr) {
  Indent i(is_last_bools, is_last);
  i.indent(stmtblock_node_ptr->node_type);
  is_last = true;
  stmtblock_node_ptr->stmt_list_optional->accept(*this);
}

void Display_visitor::visit(FunctionDecl_node* functiondecl_node_ptr) {
  Indent i(is_last_bools, is_last);
  i.indent(functiondecl_node_ptr->node_type);
  functiondecl_node_ptr->return_type->accept(*this);
  functiondecl_node_ptr->function_id->accept(*this);
  functiondecl_node_ptr->formals->accept(*this);
  is_last = true;
  functiondecl_node_ptr->block->accept(*this);
}

void Display_visitor::visit(Prototype_node* prototype_node_ptr) {
  Indent i(is_last_bools, is_last);
  i.indent(prototype_node_ptr->node_type);
  prototype_node_ptr->return_type->accept(*this);
  prototype_node_ptr->prototype_id->accept(*this);
  is_last = true;
  prototype_node_ptr->formals->accept(*this);
}

void Display_visitor::visit(InterfaceDecl_node* interfacedecl_node_ptr) {
  Indent i(is_last_bools, is_last);
  i.indent(interfacedecl_node_ptr->node_type);
  interfacedecl_node_ptr->type_id->accept(*this);
  is_last = true;
  interfacedecl_node_ptr->prototype_list_optional->accept(*this);
}

void Display_visitor::visit(ReturnStmt_node* returnstmt_node_ptr) {
  Indent i(is_last_bools, is_last);
  i.indent(returnstmt_node_ptr->node_type);
}

void Display_visitor::visit(WhileStmt_node* whilestmt_node_ptr) {
  Indent i(is_last_bools, is_last);
  i.indent(whilestmt_node_ptr->node_type);
  whilestmt_node_ptr->condition_expr->accept(*this);
  is_last = true;
  whilestmt_node_ptr->stmt->accept(*this);
}

void Display_visitor::visit(Call_node* call_node_ptr) {
  Indent i(is_last_bools, is_last);
  i.indent(call_node_ptr->node_type);
  call_node_ptr->function_id->accept(*this);
  call_node_ptr->obj_optional->accept(*this);
  is_last = true;
  call_node_ptr->actuals->accept(*this);
}

void Display_visitor::visit(BreakStmt_node* breakstmt_node_ptr) {
  Indent i(is_last_bools, is_last);
  i.indent(breakstmt_node_ptr->node_type);
}

void Display_visitor::visit(ContinueStmt_node* continuestmt_node_ptr) {
  Indent i(is_last_bools, is_last);
  i.indent(continuestmt_node_ptr->node_type);
}

void Display_visitor::visit(IfStmt_node* ifstmt_node_ptr) {
  Indent i(is_last_bools, is_last);
  i.indent(ifstmt_node_ptr->node_type);
  ifstmt_node_ptr->condition_expr->accept(*this);
  ifstmt_node_ptr->stmt->accept(*this);
  is_last = true;
  ifstmt_node_ptr->else_stmt_optional->accept(*this);
}

void Display_visitor::visit(ForStmt_node* forstmt_node_ptr) {
  Indent i(is_last_bools, is_last);
  i.indent(forstmt_node_ptr->node_type);
  forstmt_node_ptr->init_expr_optional->accept(*this);
  forstmt_node_ptr->condition_expr->accept(*this);
  forstmt_node_ptr->step_expr_optional->accept(*this);
  is_last = true;
  forstmt_node_ptr->stmt->accept(*this);
}

void Display_visitor::visit(PrintStmt_node* printstmt_node_ptr) {
  Indent i(is_last_bools, is_last);
  i.indent(printstmt_node_ptr->node_type);
  is_last = true;
  printstmt_node_ptr->expr_list->accept(*this);
}
