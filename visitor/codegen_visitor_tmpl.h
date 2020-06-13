#pragma once
#include "build/visitor.h"
#include "utils/scope.h"
#include "kern/llvm_driver.h"
#include "utils/stack_manager.h"

class llvm_driver;

// This visitor defines operations to generate llvm IR
class Codegen_visitor : public Visitor {
 public:
  ///$decl

  llvm_driver& llvm_driver_;
  const class_id& current_cid;
  Frame frame{};

  Codegen_visitor(llvm_driver& ld, Frame& _frame, const class_id& cid)
      : llvm_driver_(ld), frame(_frame), current_cid(cid) {}

  llvm::Value* get_llvm_value(ast_node_ptr_t node_ptr, bool is_rval = true);
};