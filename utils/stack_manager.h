#pragma once
#include "utils/common.h"
#include "utils/scope.h"
#include "llvm/IR/Value.h"

struct Frame {
  llvm::Value* return_llvm_value{};
  scope* current_scope_ptr{};
  optional<llvm::BasicBlock*> current_next_bb{};
  optional<llvm::BasicBlock*> current_bb{};
  optional<vector<llvm::Value*>> args{};
  bool is_rval = true;
};

class stack_manager {
 public:
  Frame old_frame{};
  Frame& frame;

  explicit stack_manager(Frame& _frame) : frame(_frame) {
    old_frame = frame;
  }

  ~stack_manager() {
    // handle return value and args specifically
    old_frame.return_llvm_value = frame.return_llvm_value;
    old_frame.args = frame.args;
    frame = old_frame;
  }
};
