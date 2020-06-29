#pragma once
#include <llvm/IR/Value.h>
#include <llvm/IR/DataLayout.h>
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
#include <fstream>

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
  llvm::StructType* obj_ref_t;

  unordered_map<var_type, llvm::Type*> builtin_types;
  unordered_map<var_type, llvm::StructType*> user_defined_types;
  unordered_map<func_id, llvm::Function*> builtin_funcs;
  // all function with same name has the same uid
  std::map<func_id, int> func_name_to_uid;
  unordered_map<class_id, unordered_map<func_id, llvm::Function*>> user_defined_func_table;
  unordered_map<class_id, llvm::GlobalVariable*> class_virtual_table;

  const symbol_table& global_symbol_table;
  const string& output_path;

  llvm_driver(const symbol_table& st, const string& output_path);

  void gen_llvm_ir();

  void define_user_type(const class_id& cid, const class_entry& ce);

  void declare_func(const class_id& cid, const func_id& fid, const func_entry& fe);

  void define_func(const class_id& cid, const func_id& fid, const func_entry& fe);

  // the member variable here does not include virtual table pointer
  int try_fetch_member_variable_uid(const class_id& cid, const var_id& vid) {
    auto& ce = global_symbol_table.try_fetch_class(cid);
    auto& field_table = ce.inheritance.field_table;
    int uid = 1;
    for (auto&[var_name, _] : field_table) {
      if (var_name == vid) {
        break;
      }
      ++uid;
    }
    assert(uid > 0);
    return uid;
  }

  llvm::AllocaInst* create_alloca_inst(const var_type& type, const var_id& vid) {
    return builder.CreateAlloca(get_llvm_type(type), nullptr, vid);
  }

  auto get_sizeof(const var_type& type) {
    // use llvm::DataLayout to get the size of a llvm type
    auto data_layout = new llvm::DataLayout(current_module.get());
    llvm::Type* obj_type;
    if (builtin_types.count(type) != 0) {
      obj_type = builtin_types[type];
    } else {
      // we don't use obj_type = get_llvm_type(type) because
      // here we want to get the size of the object rather than
      // the size of pointer to the object
      obj_type = get_llvm_type(type)->getPointerElementType();
    }
    return data_layout->getTypeAllocSize(obj_type);
  }

  auto create_llvm_constant_signed_int32(int val) {
    return llvm::ConstantInt::get(current_context, llvm::APInt(32, val, true));
  }

  auto create_llvm_constant_signed_int64(int val) {
    return llvm::ConstantInt::get(current_context, llvm::APInt(64, val, true));
  }

  auto create_llvm_signed_int64(llvm::TypeSize val) {
    return llvm::ConstantInt::get(current_context, llvm::APInt(64, val, true));
  }

  llvm::Value* alloc_object(const var_type& obj_type) {
    // make sure that we are not allocating an array type
    assert(!is_array_type(obj_type).has_value());
    auto obj_size = create_llvm_signed_int64(get_sizeof(obj_type));
    return builder.CreateCall(builtin_funcs["alloc_obj"], {obj_size});
  }

  llvm::Value* alloc_array(llvm::Value* array_size, const var_type& element_type) {
    auto element_size = create_llvm_signed_int64(get_sizeof(element_type));
    return builder.CreateCall(builtin_funcs["alloc_arr"], {array_size, element_size});
  }

  auto create_member_variable_gep(const class_id& cid,
                                  ssize_t var_uid,
                                  llvm::Value* class_ptr,
                                  const var_id& var_name, bool is_first_call = false) {
    auto& ce = global_symbol_table.try_fetch_class(cid);
    auto field_index = var_uid;
    // the uid of the last inherited member variable from parent class
    ssize_t last_uid_inherited = ce.inheritance.field_table.size() - ce.field_table.size() - 1;
    if (field_index > last_uid_inherited) {
      // this variable is defined in current class
      if (!is_first_call) {
        // if is not first call, then we need to cast the original pointer to
        // parent class pointer
        class_ptr = builder.CreatePointerCast(class_ptr, get_llvm_type(cid));
      }
      field_index -= last_uid_inherited;
      auto member_var_addr =
          builder.CreateGEP(class_ptr, {create_llvm_constant_signed_int32(0),
                                        create_llvm_constant_signed_int32(field_index)}, var_name);
      return member_var_addr;
    } else {
      // parent class variable
      // we first find out the class that define this variable
      assert(!ce.parent_class_id.empty());
      return create_member_variable_gep(ce.parent_class_id, var_uid, class_ptr, var_name);
    }
  }

  auto get_virtual_table_ptr(const class_id& cid) {
    auto const& v_table = class_virtual_table[cid];
    auto ptr = llvm::ConstantExpr::getBitCast(v_table, v_table_t->getPointerTo());
    return ptr;
  }

  auto get_pointer_value(llvm::Value* ptr) {
    return builder.CreatePtrToInt(ptr, builder.getInt64Ty());
  }

  void init_all_virtual_tables();

  llvm::Type* get_llvm_type(const var_type& type) {
    auto base_type = is_array_type(type);
    if (!base_type.has_value()) {
      if (builtin_types.count(type) != 0) {
        return builtin_types[type];
      }
      if (user_defined_types.count(type) == 0) {
        // we reference a type before its definition
        return llvm::StructType::create(current_context, type);
      } else {
        // all decaf object is accessed by reference
        return user_defined_types[type]->getPointerTo();
      }
    } else {
      auto _base_type = base_type.value();
      assert(user_defined_types.count(_base_type) != 0 || builtin_types.count(_base_type) != 0);
      return builtin_types["array"];
    }
  }
};
