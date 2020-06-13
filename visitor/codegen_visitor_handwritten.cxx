#include "build/codegen_visitor.h"

// if get_value_flag is set, then this function will
// return the value instead of the address
llvm::Value* Codegen_visitor::get_llvm_value(ast_node_ptr_t node_ptr, bool get_value_flag) {
  yylloc_manager y(node_ptr);
  stack_manager s(frame);

  frame.get_value_flag = get_value_flag;
  node_ptr->accept(*this);
  ss_assert(return_llvm_value);
  return return_llvm_value;
}

void Codegen_visitor::visit(List_node* list_node_ptr) {
  yylloc_manager y(list_node_ptr);
  stack_manager s(frame);

  if (frame.args.has_value()) {
    for (auto node_ptr : list_node_ptr->list) {
      // operate on the old args vector
      s.old_frame.args->push_back(get_llvm_value(node_ptr));
    }
  } else {
    for (auto node_ptr : list_node_ptr->list) {
      node_ptr->accept(*this);
      if (unreachable) {
        break;
      }
    }
  }

  return_llvm_value = nullptr;
}

void Codegen_visitor::visit(This_node* this_node_ptr) {
  yylloc_manager y(this_node_ptr);

  assert(frame.get_value_flag);
  return_llvm_value = frame.current_scope_ptr->lookup_llvm_value(-1);
}

void Codegen_visitor::visit(Null_const_node* null_const_node_ptr) {
  yylloc_manager y(null_const_node_ptr);

  return_llvm_value = llvm_driver_.create_llvm_constant_signed_int64(0);
}

void Codegen_visitor::visit(Empty_node* empty_node_ptr) {
}

void Codegen_visitor::visit(Ident_node* ident_node_ptr) {
  yylloc_manager y(ident_node_ptr);

  auto value = frame.current_scope_ptr->lookup_llvm_value(ident_node_ptr->uid);
  return_llvm_value = value;
  if (frame.get_value_flag) {
    return_llvm_value =
        llvm_driver_.builder.CreateLoad(llvm_driver_.get_llvm_type(ident_node_ptr->expr_type.value()),
                                        return_llvm_value);
  }
}

void Codegen_visitor::visit(Base_type_node* base_type_node_ptr) {
}

void Codegen_visitor::visit(Array_type_node* array_type_node_ptr) {
}

void Codegen_visitor::visit(User_defined_type_node* user_defined_type_node_ptr) {
}

void Codegen_visitor::visit(Assignment_node* assignment_node_ptr) {
  yylloc_manager y(assignment_node_ptr);
  stack_manager s(frame);

  auto lhs = assignment_node_ptr->LHS;
  auto rhs = assignment_node_ptr->RHS;
  // we want address of the left operand
  auto left_addr = get_llvm_value(lhs, false);
  auto right_value = get_llvm_value(rhs);
  ss_assert(lhs->expr_type.has_value() && rhs->expr_type.has_value());
  auto left_type = lhs->expr_type.value();
  auto right_type = rhs->expr_type.value();

  if (left_type != right_type) {
    if (right_type == "null") {
      auto null_value =
          llvm_driver_.builder.CreateIntToPtr(right_value, llvm_driver_.get_llvm_type(left_type));
      llvm_driver_.builder.CreateStore(null_value, left_addr);
    } else {
      // convert from derived class to base class
      auto cast_value = llvm_driver_.builder.CreatePointerCast(right_value, llvm_driver_.get_llvm_type(left_type));
      llvm_driver_.builder.CreateStore(cast_value, left_addr);
    }
  } else {
    llvm_driver_.builder.CreateStore(right_value, left_addr);
  }
  return_llvm_value = get_llvm_value(lhs);
}

void Codegen_visitor::visit(Binary_expr_node* binary_expr_node_ptr) {
  yylloc_manager y(binary_expr_node_ptr);
  stack_manager s(frame);

  auto lhs = binary_expr_node_ptr->left_operand;
  auto rhs = binary_expr_node_ptr->right_operand;
  auto left_value = get_llvm_value(lhs);
  auto right_value = get_llvm_value(rhs);
  ss_assert(lhs->expr_type.has_value() && rhs->expr_type.has_value());
  auto left_type = llvm_driver_.get_llvm_type(lhs->expr_type.value());
  auto right_type = llvm_driver_.get_llvm_type(rhs->expr_type.value());
  decltype(return_llvm_value) rt_value;
  switch (binary_expr_node_ptr->op) {
    case '+': {
      if (lhs->expr_type.value() == "string") {
        // TODO: implement string cat
        auto func = llvm_driver_.builtin_funcs["string_cat"];
        assert(func);
        rt_value = llvm_driver_.builder.CreateCall(func, {left_value, right_value});
      } else {
        rt_value = llvm_driver_.builder.CreateAdd(left_value, right_value);
      }
      break;
    }
    case '%': {
      rt_value = llvm_driver_.builder.CreateSRem(left_value, right_value);
      break;
    }
    case '-': {
      rt_value = llvm_driver_.builder.CreateSub(left_value, right_value);
    }
    case '*': {
      rt_value = llvm_driver_.builder.CreateMul(left_value, right_value);
    }
    case '/': {
      rt_value = llvm_driver_.builder.CreateSDiv(left_value, right_value);
      break;
    }
    case '<': {
      rt_value = llvm_driver_.builder.CreateICmpSLE(left_value, right_value);
      break;
    }
    case '>': {
      rt_value = llvm_driver_.builder.CreateICmpSGT(left_value, right_value);
      break;
    }
    case t_greater_eq: {
      rt_value = llvm_driver_.builder.CreateICmpSGE(left_value, right_value);
      break;
    }
    case t_less_eq: {
      rt_value = llvm_driver_.builder.CreateICmpSLE(left_value, right_value);
      break;
    }

    case t_not_eq:
    case t_eq: {
      if (llvm_driver_.user_defined_types.count(lhs->expr_type.value()) != 0) {
        // if is user defined type, then we check if they are pointing to
        // the same address
        left_value = llvm_driver_.get_pointer_value(left_value);
        right_value = llvm_driver_.get_pointer_value(right_value);
      }
      if (binary_expr_node_ptr->op == t_eq) {
        rt_value = llvm_driver_.builder.CreateICmpEQ(left_value, right_value);
      } else {
        rt_value = llvm_driver_.builder.CreateICmpNE(left_value, right_value);
      }
      break;
    }
    case t_and: {
      rt_value = llvm_driver_.builder.CreateAnd(left_value, right_value);
      break;
    }
    case t_or: {
      rt_value = llvm_driver_.builder.CreateOr(left_value, right_value);
      break;
    }
  }

  return_llvm_value = rt_value;
}

void Codegen_visitor::visit(Unary_expr_node* unary_expr_node_ptr) {
  yylloc_manager y(unary_expr_node_ptr);
  stack_manager s(frame);

  auto value = get_llvm_value(unary_expr_node_ptr->operand);
  switch (unary_expr_node_ptr->op) {
    case '-': {
      return_llvm_value = llvm_driver_.builder.CreateNeg(value);
      break;
    }

    case '!': {
      return_llvm_value = llvm_driver_.builder.CreateNot(value);
      break;
    }

    default:ss_assert(false, "Unknown operator");
  }
}

void Codegen_visitor::visit(Read_op_node* read_op_node_ptr) {
  if (read_op_node_ptr->read_type == t_ReadInteger) {
    auto func = llvm_driver_.builtin_funcs["read_int"];
    return_llvm_value = llvm_driver_.builder.CreateCall(func);
  } else {
    ss_assert(read_op_node_ptr->read_type = t_ReadLine);
    auto func = llvm_driver_.builtin_funcs["read_line"];
    return_llvm_value = llvm_driver_.builder.CreateCall(func);
  }
}

// decaf grammar guarantees that only user defined struct can
// use the new operator
void Codegen_visitor::visit(New_op_node* new_op_node_ptr) {
  yylloc_manager y(new_op_node_ptr);

  auto type = new_op_node_ptr->expr_type.value();
  // now the llvm type of value is int8* (void *)
  auto raw_obj_addr = llvm_driver_.alloc_object(type);
  // cast it to corresponding type
  auto cast_obj_addr = llvm_driver_.builder.CreatePointerCast(raw_obj_addr, llvm_driver_.get_llvm_type(type));

  // pass virtual table pointer to allocated object
  // the first member variable of any object is v_ptr
  auto v_ptr_addr =
      llvm_driver_.builder.CreatePointerCast(cast_obj_addr, llvm_driver_.v_table_t->getPointerTo()->getPointerTo());
  llvm_driver_.builder.CreateStore(llvm_driver_.get_virtual_table_ptr(type), v_ptr_addr);
  return_llvm_value = cast_obj_addr;
}

void Codegen_visitor::visit(New_array_op_node* new_array_op_node_ptr) {
  yylloc_manager y(new_array_op_node_ptr);
  stack_manager s(frame);

  new_array_op_node_ptr->array_size->accept(*this);
  auto array_size = return_llvm_value;
  auto element_type = new_array_op_node_ptr->alloc_obj_type->expr_type.value();
  // we need to cast i32 arr_index to i64
  auto cast_arr_size = llvm_driver_.builder.CreateIntCast(array_size, llvm_driver_.builder.getInt64Ty(), false);
  auto arr_addr = llvm_driver_.alloc_array(cast_arr_size, element_type);
  return_llvm_value = arr_addr;
}

void Codegen_visitor::visit(Dot_op_node* dot_op_node_ptr) {
  yylloc_manager y(dot_op_node_ptr);
  stack_manager s(frame);

  llvm::Value* member_addr;
  if (dynamic_cast<This_node*>(dot_op_node_ptr->obj) != nullptr) {
    ss_assert(dot_op_node_ptr->member_id->uid > 0);
    member_addr = frame.current_scope_ptr->lookup_llvm_value(dot_op_node_ptr->member_id->uid);
  } else {
    auto obj_addr = get_llvm_value(dot_op_node_ptr->obj);
    auto cid = dot_op_node_ptr->obj->expr_type.value();
    auto vid = dot_op_node_ptr->member_id->ident_name;
    auto uid = dot_op_node_ptr->member_id->uid;
    ss_assert(uid >= 0);
    string var_name = cid + ".";
    var_name += vid;
    member_addr = llvm_driver_.create_member_variable_gep(cid, uid, obj_addr, var_name, true);
  }

  if (frame.get_value_flag) {
    return_llvm_value =
        llvm_driver_.builder.CreateLoad(llvm_driver_.get_llvm_type(dot_op_node_ptr->expr_type.value()), member_addr);
  } else {
    return_llvm_value = member_addr;
  }
}

void Codegen_visitor::visit(Index_op_node* index_op_node_ptr) {
  yylloc_manager y(index_op_node_ptr);
  stack_manager s(frame);

  auto array_type = index_op_node_ptr->array->expr_type.value();
  auto element_type = llvm_driver_.get_llvm_type(array_type.substr(0, array_type.size() - 2));
  auto element_pointer_type = element_type->getPointerTo();
  auto array_obj_addr = get_llvm_value(index_op_node_ptr->array);
  auto arr_index = get_llvm_value(index_op_node_ptr->index_expr);
  // we need to cast i32 arr_index to i64
  auto cast_arr_index = llvm_driver_.builder.CreateIntCast(arr_index, llvm_driver_.builder.getInt64Ty(), false);
  // dynamic array access sanity check
  auto validate_index =
      llvm_driver_.builder.CreateCall(llvm_driver_.builtin_funcs["validate_access"], {array_obj_addr, cast_arr_index});
  // get the address of the underlying array pointer
  auto underlying_arr_addr_addr = llvm_driver_.builder.CreateStructGEP(array_obj_addr, 2);
  // get the address of first element in array
  auto underlying_arr_addr_val =
      llvm_driver_.builder.CreateLoad(llvm_driver_.obj_ref_t->getPointerTo(), underlying_arr_addr_addr);
  auto element_addr_addr = llvm_driver_.builder.CreateGEP(underlying_arr_addr_val, cast_arr_index);
  return_llvm_value = llvm_driver_.builder.CreatePointerCast(element_addr_addr, element_pointer_type);
  if (frame.get_value_flag) {
    return_llvm_value = llvm_driver_.builder.CreateLoad(element_type, return_llvm_value);
  }
}

void Codegen_visitor::visit(Int_const_node* int_const_node_ptr) {
  ss_assert(frame.get_value_flag);
  return_llvm_value = llvm_driver_.create_llvm_constant_signed_int32(int_const_node_ptr->val);
}

void Codegen_visitor::visit(Double_const_node* double_const_node_ptr) {
  ss_assert(frame.get_value_flag);
  return_llvm_value = llvm::ConstantFP::get(llvm_driver_.builder.getDoubleTy(), double_const_node_ptr->val);
}

void Codegen_visitor::visit(Str_const_node* str_const_node_ptr) {
}

void Codegen_visitor::visit(Bool_const_node* bool_const_node_ptr) {
  ss_assert(frame.get_value_flag);
  return_llvm_value = llvm::ConstantInt::get(llvm_driver_.builder.getInt1Ty(), bool_const_node_ptr->val);
}

void Codegen_visitor::visit(ClassDecl_node* classdecl_node_ptr) {
}

void Codegen_visitor::visit(Variable_node* variable_node_ptr) {
  return_llvm_value = nullptr;
}

void Codegen_visitor::visit(StmtBlock_node* stmtblock_node_ptr) {
  yylloc_manager y(stmtblock_node_ptr);
  stack_manager s(frame);

  auto scope_ptr = stmtblock_node_ptr->scope_ptr;
  frame.current_scope_ptr = scope_ptr;
  // at the beginning of each statement block, define and allocate all
  // local variables
  for (auto&[vid, ve] : scope_ptr->local_symbol_table) {
    auto&[uid, type] = ve;
    if (scope_ptr->var_uid_to_llvm_value.count(uid) == 0) {
      scope_ptr->var_uid_to_llvm_value[uid] = llvm_driver_.create_alloca_inst(type, vid);
    }
  }
  stmtblock_node_ptr->stmt_list_optional->accept(*this);

  return_llvm_value = nullptr;
}

void Codegen_visitor::visit(FunctionDecl_node* functiondecl_node_ptr) {
}

void Codegen_visitor::visit(Prototype_node* prototype_node_ptr) {
}

void Codegen_visitor::visit(InterfaceDecl_node* interfacedecl_node_ptr) {
}

void Codegen_visitor::visit(ReturnStmt_node* returnstmt_node_ptr) {
  yylloc_manager y(returnstmt_node_ptr);
  stack_manager s(frame);

  auto return_type = returnstmt_node_ptr->expr_optional->expr_type.value();
  if (return_type.empty() && dynamic_cast<Empty_node*>(returnstmt_node_ptr->expr_optional) != nullptr) {
    llvm_driver_.builder.CreateRetVoid();
  } else {
    llvm_driver_.builder.CreateRet(get_llvm_value(returnstmt_node_ptr->expr_optional));
  }
  unreachable = true;

  returnstmt_node_ptr = nullptr;
}

void Codegen_visitor::visit(WhileStmt_node* whilestmt_node_ptr) {
  yylloc_manager y(whilestmt_node_ptr);
  stack_manager s(frame);

  auto reside_func = llvm_driver_.builder.GetInsertBlock()->getParent();
  auto cond_bb = llvm::BasicBlock::Create(llvm_driver_.current_context, "while_cond", reside_func);
  auto loop_bb = llvm::BasicBlock::Create(llvm_driver_.current_context, "while_body");
  auto next_bb = llvm::BasicBlock::Create(llvm_driver_.current_context, "while_next");
  // used by continue and break statement
  frame.break_dest_bb = next_bb;
  frame.cont_dest_bb = cond_bb;

  llvm_driver_.builder.CreateBr(cond_bb);

  // condition block
  llvm_driver_.builder.SetInsertPoint(cond_bb);
  auto condition = get_llvm_value(whilestmt_node_ptr->condition_expr);
  llvm_driver_.builder.CreateCondBr(condition, loop_bb, next_bb);

  // loop block
  reside_func->getBasicBlockList().push_back(loop_bb);
  llvm_driver_.builder.SetInsertPoint(loop_bb);
  // emit for body code
  whilestmt_node_ptr->stmt->accept(*this);
  if (unreachable) {
    unreachable = false;
    // break or continue or return statement has already created
    // correct branch command for us
  } else {
    llvm_driver_.builder.CreateBr(cond_bb);
  }

  // next block
  reside_func->getBasicBlockList().push_back(next_bb);
  llvm_driver_.builder.SetInsertPoint(next_bb);

  return_llvm_value = nullptr;
}

void Codegen_visitor::visit(Call_node* call_node_ptr) {
  yylloc_manager y(call_node_ptr);
  stack_manager s(frame);

  auto func_name = call_node_ptr->function_id->ident_name;
  llvm::Value* caller;
  auto obj_type = call_node_ptr->obj_optional->expr_type.value();
  if (obj_type.empty()) {
    obj_type = current_cid;
  }
  if (dynamic_cast<This_node*>(call_node_ptr->obj_optional) != nullptr
      || dynamic_cast<Empty_node*>(call_node_ptr->obj_optional) != nullptr) {
    caller = frame.current_scope_ptr->lookup_llvm_value(-1);
  } else {
    caller = get_llvm_value(call_node_ptr->obj_optional);
  }
  auto v_ptr_addr =
      llvm_driver_.builder.CreateBitCast(caller, llvm_driver_.v_table_t->getPointerTo()->getPointerTo());
  auto v_ptr_val = llvm_driver_.builder.CreateLoad(llvm_driver_.v_table_t->getPointerTo(), v_ptr_addr);
  auto f_uid = llvm_driver_.func_name_to_uid[func_name];
  auto f_ptr =
      llvm_driver_.builder.CreateCall(llvm_driver_.builtin_funcs["lookup_fptr"],
                                      {v_ptr_val, llvm::ConstantInt::get(llvm_driver_.current_context,
                                                                         llvm::APInt(32, f_uid, false))});
  auto func = llvm_driver_.user_defined_func_table[obj_type][func_name];
  auto func_type = func->getFunctionType();
  // cast function pointer to the correct function type
  auto cast_f_ptr = llvm_driver_.builder.CreateBitCast(f_ptr, func_type->getPointerTo());
  // the first arg is pointer of caller
  frame.args = vector<llvm::Value*>{caller};
  call_node_ptr->actuals->accept(*this);
  return_llvm_value = llvm_driver_.builder.CreateCall(cast_f_ptr, frame.args.value());
}

void Codegen_visitor::visit(BreakStmt_node* breakstmt_node_ptr) {
  yylloc_manager y(breakstmt_node_ptr);

  ss_assert(frame.break_dest_bb.has_value());
  llvm_driver_.builder.CreateBr(frame.break_dest_bb.value());
  unreachable = true;

  return_llvm_value = nullptr;
}

void Codegen_visitor::visit(ContinueStmt_node* continuestmt_node_ptr) {
  yylloc_manager y(continuestmt_node_ptr);

  ss_assert(frame.cont_dest_bb.has_value());
  llvm_driver_.builder.CreateBr(frame.cont_dest_bb.value());
  unreachable = true;

  return_llvm_value = nullptr;
}

void Codegen_visitor::visit(IfStmt_node* ifstmt_node_ptr) {
  yylloc_manager y(ifstmt_node_ptr);

  auto parent_bb = llvm_driver_.builder.GetInsertBlock()->getParent();
  auto then_bb = llvm::BasicBlock::Create(llvm_driver_.current_context, "if_then", parent_bb);
  auto else_bb = llvm::BasicBlock::Create(llvm_driver_.current_context, "else");
  auto next_bb = llvm::BasicBlock::Create(llvm_driver_.current_context, "if_next");
  auto condition = get_llvm_value(ifstmt_node_ptr->condition_expr);
  llvm_driver_.builder.CreateCondBr(condition, then_bb, else_bb);

  // then block
  llvm_driver_.builder.SetInsertPoint(then_bb);
  ifstmt_node_ptr->stmt->accept(*this);
  if (unreachable) {
    unreachable = false;
  } else {
    llvm_driver_.builder.CreateBr(next_bb);
  }

  // else block
  parent_bb->getBasicBlockList().push_back(else_bb);
  llvm_driver_.builder.SetInsertPoint(else_bb);
  ifstmt_node_ptr->else_stmt_optional->accept(*this);
  if (unreachable) {
    unreachable = false;
  } else {
    llvm_driver_.builder.CreateBr(next_bb);
  }

  // next block
  parent_bb->getBasicBlockList().push_back(next_bb);
  llvm_driver_.builder.SetInsertPoint(next_bb);

  return_llvm_value = nullptr;
}

void Codegen_visitor::visit(ForStmt_node* forstmt_node_ptr) {
  yylloc_manager y(forstmt_node_ptr);
  stack_manager s(frame);

  // emit init code
  forstmt_node_ptr->init_expr_optional->accept(*this);

  auto reside_func = llvm_driver_.builder.GetInsertBlock()->getParent();
  auto cond_bb = llvm::BasicBlock::Create(llvm_driver_.current_context, "for_cond", reside_func);
  auto loop_bb = llvm::BasicBlock::Create(llvm_driver_.current_context, "for_body");
  auto next_bb = llvm::BasicBlock::Create(llvm_driver_.current_context, "for_next");
  auto step_bb = llvm::BasicBlock::Create(llvm_driver_.current_context, "for_step");
  // used by continue and break statement
  frame.break_dest_bb = next_bb;
  frame.cont_dest_bb = step_bb;

  llvm_driver_.builder.CreateBr(cond_bb);

  // condition block
  llvm_driver_.builder.SetInsertPoint(cond_bb);
  auto condition = get_llvm_value(forstmt_node_ptr->condition_expr);
  llvm_driver_.builder.CreateCondBr(condition, loop_bb, next_bb);

  // loop block
  reside_func->getBasicBlockList().push_back(loop_bb);
  llvm_driver_.builder.SetInsertPoint(loop_bb);
  // emit for body code
  forstmt_node_ptr->stmt->accept(*this);
  if (unreachable) {
    unreachable = false;
    // break or continue or return statement has already created
    // correct branch command for us
  } else {
    llvm_driver_.builder.CreateBr(step_bb);
  }

  // emit for step code
  reside_func->getBasicBlockList().push_back(step_bb);
  llvm_driver_.builder.SetInsertPoint(step_bb);
  forstmt_node_ptr->step_expr_optional->accept(*this);
  llvm_driver_.builder.CreateBr(cond_bb);

  // next block
  reside_func->getBasicBlockList().push_back(next_bb);
  llvm_driver_.builder.SetInsertPoint(next_bb);

  return_llvm_value = nullptr;
}

void Codegen_visitor::visit(PrintStmt_node* printstmt_node_ptr) {
  yylloc_manager y(printstmt_node_ptr);
  stack_manager s(frame);

  frame.args = vector<llvm::Value*>();
  printstmt_node_ptr->expr_list->accept(*this);
  auto& type_list = printstmt_node_ptr->expr_list->elements_type_list.value();
  auto& args = frame.args.value();
  for (size_t i = 0; i < type_list.size(); ++i) {
    auto arg_type = type_list[i];
    auto arg = args[i];
    if (arg_type == "double") {
      llvm_driver_.builder.CreateCall(llvm_driver_.builtin_funcs["print_double"], arg);
    } else if (arg_type == "int") {
      llvm_driver_.builder.CreateCall(llvm_driver_.builtin_funcs["print_int"], arg);
    } else if (arg_type == "bool") {
      llvm_driver_.builder.CreateCall(llvm_driver_.builtin_funcs["print_bool"], arg);
    } else if (arg_type == "string") {
      llvm_driver_.builder.CreateCall(llvm_driver_.builtin_funcs["print_str"], arg);
    } else {
      ss_assert(false);
    }
  }
  return_llvm_value = nullptr;
}
