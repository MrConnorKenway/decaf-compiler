#pragma once
#include "utils/common.h"
#include "utils/scope.h"
#include "llvm/IR/Value.h"

struct Frame {
  scope* current_scope_ptr{};
  optional<llvm::BasicBlock*> break_dest_bb{};
  optional<llvm::BasicBlock*> cont_dest_bb{};
  optional<vector<llvm::Value*>> args{};
  // true if we want to get the value
  // rather than address
  bool get_value_flag = true;
};

class stack_manager {
 public:
  Frame old_frame{};
  Frame& frame;

  explicit stack_manager(Frame& _frame) : frame(_frame) {
    old_frame = frame;
  }

  ~stack_manager() {
    std::swap(frame, old_frame);
  }
};
