#pragma once
#include <llvm/IR/Value.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Constant.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/SourceMgr.h>
#include <memory>
#include <string_view>

#include "utils/common.h"
#include "kern/symbol_table.h"
#include "build/codegen_visitor.h"

class llvm_driver {
 public:
  llvm::LLVMContext current_context;
  std::unique_ptr<llvm::Module> current_module;
  std::unique_ptr<llvm::Module> extern_module;
  llvm::IRBuilder<> builder;
  llvm::SMDiagnostic err;

  llvm::StructType* v_entry_t;
  llvm::StructType* v_table_t;
  llvm::StructType* decaf_str_t;
  llvm::StructType* decaf_arr_t;

  unordered_map<var_type, llvm::Type*> builtin_types;
  unordered_map<var_type, llvm::StructType*> user_defined_types;
  unordered_map<func_id, llvm::Function*> builtin_funcs;
  // all function with same name has the same uid
  std::map<func_id, int> func_name_to_uid;
  unordered_map<class_id, unordered_map<func_id, llvm::Function*>> user_defined_func_table;
  unordered_map<class_id, llvm::GlobalVariable*> class_virtual_table;

  const symbol_table& global_symbol_table;

  explicit llvm_driver(const symbol_table& st);

  void gen_llvm_ir();

  void declare_func(const class_id& cid, const func_id& fid, const func_entry& fe);

  void define_func(const class_id& cid, const func_id& fid, const func_entry& fe);

  auto create_llvm_constant_signed_int(int val) {
    return llvm::ConstantInt::get(current_context, llvm::APInt(32, val, true));
  }

  void init_all_virtual_tables();

  llvm::Type* get_llvm_type(const var_type& type) {
    auto base_type = is_array_type(type);
    if (!base_type.has_value()) {
      if (builtin_types.count(type) != 0) {
        return builtin_types[type];
      }
      assert(user_defined_types.count(type) != 0);
      // all decaf object is accessed by reference
      return user_defined_types[type]->getPointerTo();
    } else {
      auto _base_type = base_type.value();
      assert(user_defined_types.count(_base_type) != 0 || builtin_types.count(_base_type) != 0);
      return builtin_types["array"];
    }
  }
};
