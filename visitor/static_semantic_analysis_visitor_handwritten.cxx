#include "build/include/static_semantic_analysis_visitor.h"

void Static_semantic_analysis_visitor::visit(List_node* list_node_ptr) {
  yylloc_manager y(list_node_ptr);
  if (!call_trace.empty() && call_trace.top() == action_type::GET_ARGS_TYPE) {
    for (auto node_ptr : list_node_ptr->list) {
      list_node_ptr->elements_type_list = vector<string>();
      list_node_ptr->elements_type_list.value().push_back(decl_type(node_ptr));
    }
  } else {
    for (auto node_ptr : list_node_ptr->list) {
      node_ptr->accept(*this);
    }
  }
}

void Static_semantic_analysis_visitor::visit(This_node* this_node_ptr) {
  this_node_ptr->expr_type = current_class_id;
}

void Static_semantic_analysis_visitor::visit(
    Null_const_node* null_const_node_ptr) {
  null_const_node_ptr->expr_type = "null";
}

void Static_semantic_analysis_visitor::visit(Empty_node* empty_node_ptr) {
  empty_node_ptr->expr_type = "";
}

void Static_semantic_analysis_visitor::visit(Ident_node* ident_node_ptr) {
  yylloc_manager y(ident_node_ptr);
  // identifier as an expression
  std::tie(ident_node_ptr->tid, ident_node_ptr->expr_type) =
      current_scope_ptr->lookup(ident_node_ptr->ident_name);
}

void Static_semantic_analysis_visitor::visit(
    Base_type_node* base_type_node_ptr) {
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

void Static_semantic_analysis_visitor::visit(
    Array_type_node* array_type_node_ptr) {
  yylloc_manager y(array_type_node_ptr);
  visit_and_get_id_of(array_type_node_ptr->base_type);
  current_id += "[]";
  array_type_node_ptr->expr_type = current_id;
}

void Static_semantic_analysis_visitor::visit(
    User_defined_type_node* user_defined_type_node_ptr) {
  current_id = user_defined_type_node_ptr->type_ident_name;
  user_defined_type_node_ptr->expr_type = current_id;
}

void Static_semantic_analysis_visitor::visit(
    Assignment_node* assignment_node_ptr) {
  yylloc_manager y(assignment_node_ptr);
  auto left_type = decl_type(assignment_node_ptr->LHS);
  auto right_type = decl_type(assignment_node_ptr->RHS);
  if (left_type != right_type) {
    global_symbol_table.try_fetch_class(left_type);
    ss_assert(left_type == "null" || right_type == "null", "LHS type \"",
              left_type, "\" and RHS type \"", right_type, "\" mismatch\n");
  }
  assignment_node_ptr->expr_type = left_type;
}

void Static_semantic_analysis_visitor::visit(
    Binary_expr_node* binary_expr_node_ptr) {
  yylloc_manager y(binary_expr_node_ptr);
  auto left_type = decl_type(binary_expr_node_ptr->left_operand);
  auto right_type = decl_type(binary_expr_node_ptr->right_operand);
  switch (binary_expr_node_ptr->op) {
    case '+': {
      ss_assert(left_type == right_type,
                "LHS type \"%s\" and RHS type \"%s\" mismatch\n",
                left_type.c_str(), right_type.c_str());
      ss_assert(
          left_type == "double" || left_type == "int" || left_type == "string",
          "The type of left operand \"%s\" does not support \"%c\" operation\n",
          left_type.c_str(), binary_expr_node_ptr->op);
      binary_expr_node_ptr->expr_type = left_type;
      break;
    }
    case '%':
    case '-':
    case '*':
    case '/': {
      ss_assert(left_type == right_type,
                "LHS type \"%s\" and RHS type \"%s\" mismatch\n",
                left_type.c_str(), right_type.c_str());
      ss_assert(
          left_type == "double" || left_type == "int",
          "The type of left operand \"%s\" does not support \"%c\" operation\n",
          left_type.c_str(), binary_expr_node_ptr->op);
      binary_expr_node_ptr->expr_type = left_type;
      break;
    }
    case '<':
    case '>':
    case t_greater_eq:
    case t_less_eq: {
      ss_assert(left_type == right_type,
                "LHS type \"%s\" and RHS type \"%s\" mismatch\n",
                left_type.c_str(), right_type.c_str());
      ss_assert(
          left_type == "double" || left_type == "int",
          "The type of left operand \"%s\" does not support comparation\n",
          left_type.c_str());
      binary_expr_node_ptr->expr_type = "bool";
      break;
    }

    case t_not_eq:
    case t_eq: {
      if (left_type != right_type) {
        ss_assert(left_type == "null" || right_type == "null",
                  "LHS type \"%s\" and RHS type \"%s\" mismatch\n",
                  left_type.c_str(), right_type.c_str());
      }
      binary_expr_node_ptr->expr_type = "bool";
      break;
    }
    case t_and:
    case t_or: {
      ss_assert(left_type == right_type,
                "LHS type \"%s\" and RHS type \"%s\" mismatch\n",
                left_type.c_str(), right_type.c_str());
      ss_assert(
          left_type == "double" || left_type == "int",
          "The type of left operand \"%s\" does not support comparation\n",
          left_type.c_str());
      binary_expr_node_ptr->expr_type = "bool";
      break;
    }
  }
}

void Static_semantic_analysis_visitor::visit(
    Unary_expr_node* unary_expr_node_ptr) {
  yylloc_manager y(unary_expr_node_ptr);
  auto operand_type = decl_type(unary_expr_node_ptr->operand);
  switch (unary_expr_node_ptr->op) {
    case '-': {
      ss_assert(
          operand_type == "double" || operand_type == "int",
          "The type of operand \"%s\" does not support unary minus operation\n",
          operand_type.c_str());
      unary_expr_node_ptr->expr_type = operand_type;
      break;
    }

    case '!': {
      ss_assert(operand_type == "bool",
                "The type of operand \"%s\" does not support logical negation "
                "operation\n",
                operand_type.c_str());
      unary_expr_node_ptr->expr_type = "bool";
      break;
    }

    default:
      ss_assert(false, "Unknown operator\n");
  }
}

void Static_semantic_analysis_visitor::visit(Read_op_node* read_op_node_ptr) {
  read_op_node_ptr->expr_type = "void";
}

void Static_semantic_analysis_visitor::visit(New_op_node* new_op_node_ptr) {
  yylloc_manager y(new_op_node_ptr);
  new_op_node_ptr->expr_type = decl_type(new_op_node_ptr->alloc_obj_type);
}

void Static_semantic_analysis_visitor::visit(
    New_array_op_node* new_array_op_node_ptr) {
  yylloc_manager y(new_array_op_node_ptr);
  new_array_op_node_ptr->expr_type =
      decl_type(new_array_op_node_ptr->alloc_obj_type) + "[]";
}

void Static_semantic_analysis_visitor::visit(Dot_op_node* dot_op_node_ptr) {
  yylloc_manager y(dot_op_node_ptr);
  auto cid = decl_type(dot_op_node_ptr->obj);
  auto& ce = global_symbol_table.try_fetch_class(cid);
  dot_op_node_ptr->expr_type =
      ce.try_fetch_variable(dot_op_node_ptr->member_id->ident_name);
}

void Static_semantic_analysis_visitor::visit(Index_op_node* index_op_node_ptr) {
  yylloc_manager y(index_op_node_ptr);
  auto array_type = decl_type(index_op_node_ptr->array);
  auto pos = array_type.find("[");
  ss_assert(pos != string::npos, "The left of dot is not an array type\n");
  auto index_type = decl_type(index_op_node_ptr->index_expr);
  ss_assert(index_type == "int",
            "Index of an array should be \"int\" rather than \"%s\"\n",
            index_type);
  // array_type must be "int[][][]...[]" form
  index_op_node_ptr->expr_type = array_type.substr(0, array_type.size() - 2);
}

void Static_semantic_analysis_visitor::visit(
    Int_const_node* int_const_node_ptr) {
  int_const_node_ptr->expr_type = "int";
}

void Static_semantic_analysis_visitor::visit(
    Double_const_node* double_const_node_ptr) {
  double_const_node_ptr->expr_type = "double";
}

void Static_semantic_analysis_visitor::visit(
    Str_const_node* str_const_node_ptr) {
  str_const_node_ptr->expr_type = "string";
}

void Static_semantic_analysis_visitor::visit(
    Bool_const_node* bool_const_node_ptr) {
  bool_const_node_ptr->expr_type = "bool";
}

void Static_semantic_analysis_visitor::visit(
    ClassDecl_node* classdecl_node_ptr) {}

void Static_semantic_analysis_visitor::visit(Variable_node* variable_node_ptr) {
  yylloc_manager y(variable_node_ptr);
  current_scope_ptr->try_insert(
      variable_node_ptr->id->ident_name,
      visit_and_get_id_of(variable_node_ptr->type_node));
}

void Static_semantic_analysis_visitor::visit(
    StmtBlock_node* stmtblock_node_ptr) {
  yylloc_manager y(stmtblock_node_ptr);
  if (stmtblock_node_ptr->scope_ptr == nullptr) {
    // this block is not a function body block
    stmtblock_node_ptr->scope_ptr = new scope();
  }
  stmtblock_node_ptr->scope_ptr->parent_scope_ptr = current_scope_ptr;
  current_scope_ptr = stmtblock_node_ptr->scope_ptr;
  stmtblock_node_ptr->stmt_list_optional->accept(*this);
  current_scope_ptr = stmtblock_node_ptr->scope_ptr->parent_scope_ptr;
}

void Static_semantic_analysis_visitor::visit(
    FunctionDecl_node* functiondecl_node_ptr) {}

void Static_semantic_analysis_visitor::visit(
    Prototype_node* prototype_node_ptr) {}

void Static_semantic_analysis_visitor::visit(
    InterfaceDecl_node* interfacedecl_node_ptr) {}

void Static_semantic_analysis_visitor::visit(
    ReturnStmt_node* returnstmt_node_ptr) {
  yylloc_manager y(returnstmt_node_ptr);
  ss_assert(
      decl_type(returnstmt_node_ptr->expr_optional) ==
          global_symbol_table.try_fetch_func(current_class_id, current_func_id)
              .return_type,
      "Return type mismatch function \"%s\" in class \"%s\"\n",
      current_func_id.c_str(), current_class_id.c_str());
}

void Static_semantic_analysis_visitor::visit(
    WhileStmt_node* whilestmt_node_ptr) {
  yylloc_manager y(whilestmt_node_ptr);
  current_loop_node = whilestmt_node_ptr;
}

// examine whether the call obey the function prototype
// and lookup the return type
void Static_semantic_analysis_visitor::visit(Call_node* call_node_ptr) {
  yylloc_manager y(call_node_ptr);
  auto obj_type = decl_type(call_node_ptr->obj_optional);
  char mismatch_info[] = "Function call mismatches its definition\n";

  string cid;
  if (obj_type.empty()) {
    // the call is direct function call
    // we think the function implicitly refers to this.function
    cid = current_class_id;
  } else {
    // member function call
    cid = obj_type;
  }
  auto const& fetched_fe = global_symbol_table.try_fetch_func(
      cid, call_node_ptr->function_id->ident_name);
  auto actual_list = dynamic_cast<List_node*>(call_node_ptr->actuals);
  ss_assert(actual_list != nullptr, "Cast failed\n");
  auto const& type_list = decl_type_list(actual_list);

  ss_assert(type_list.size() == fetched_fe.formal_table.size(), "%s",
            mismatch_info);
  bool is_matched = true;
  int cnt = 0;
  for (auto [_, vt] : fetched_fe.formal_table) {
    if (type_list[cnt] != vt) {
      is_matched = false;
      break;
    }
    ++cnt;
  }
  ss_assert(is_matched, "%s", mismatch_info);
  call_node_ptr->expr_type = fetched_fe.return_type;
}

void Static_semantic_analysis_visitor::visit(
    BreakStmt_node* breakstmt_node_ptr) {
  yylloc_manager y(breakstmt_node_ptr);
  ss_assert(current_loop_node != nullptr,
            "Use break statement outside a loop\n");
  breakstmt_node_ptr->resident_loop_node = current_loop_node;
}

void Static_semantic_analysis_visitor::visit(
    ContinueStmt_node* continuestmt_node_ptr) {
  yylloc_manager y(continuestmt_node_ptr);
  ss_assert(current_loop_node != nullptr,
            "Use continue statement outside a loop\n");
  continuestmt_node_ptr->resident_loop_node = current_loop_node;
}

void Static_semantic_analysis_visitor::visit(IfStmt_node* ifstmt_node_ptr) {
  yylloc_manager y(ifstmt_node_ptr);
  ss_assert(decl_type(ifstmt_node_ptr->condition_expr) == "bool",
            "Cannot convert if condition_expr to bool\n");
  ifstmt_node_ptr->stmt->accept(*this);
  ifstmt_node_ptr->else_stmt_optional->accept(*this);
}

void Static_semantic_analysis_visitor::visit(ForStmt_node* forstmt_node_ptr) {
  yylloc_manager y(forstmt_node_ptr);
  auto parent_loop_node = current_loop_node;
  current_loop_node = forstmt_node_ptr;

  forstmt_node_ptr->init_expr_optional->accept(*this);
  ss_assert(decl_type(forstmt_node_ptr->condition_expr) == "bool",
            "Cannot convert for condition_expr to bool\n");

  forstmt_node_ptr->step_expr_optional->accept(*this);
  forstmt_node_ptr->stmt->accept(*this);

  current_loop_node = parent_loop_node;
}

void Static_semantic_analysis_visitor::visit(
    PrintStmt_node* printstmt_node_ptr) {}
