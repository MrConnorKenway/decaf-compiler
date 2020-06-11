#include "llvm_driver.h"

void llvm_driver::gen_llvm_ir() {
  // unique identifier for function
  int uid = 0;

  for (auto&[eid, entry] : global_symbol_table) {
    if (eid == "Main") {
      continue;
    }
    if (std::holds_alternative<class_entry>(entry)) {
      auto& ce = std::get<class_entry>(entry);
      auto& cid = eid;

      // define llvm IR of user defined type
      assert(user_defined_types.count(cid) == 0);
      auto user_defined_type = llvm::StructType::create(current_context, cid);
      user_defined_types[cid] = user_defined_type;

      vector<llvm::Type*> struct_body;
      // every class contains a virtual table pointer
      struct_body.push_back(v_table_t->getPointerTo());

      // also put inherited fields into struct
      for (auto&[vid, vt] : ce.inheritance.field_table) {
        struct_body.push_back(get_llvm_type(vt));
      }
      user_defined_type->setBody(struct_body);

      // we don't have to handle the inherited function here, since
      // we will depend on virtual table to handle function call
      for (auto&[fid, decl_class_id] : ce.inheritance.func_decl_class) {
        func_name_to_uid[fid];
        if (decl_class_id == cid) {
          auto& fe = ce.func_table.at(fid);
          declare_func(cid, fid, fe);
        }
      }
    }
  }

  // make sure that the order of uid correspond to the order
  // of function name
  for (auto&[_, i] : func_name_to_uid) {
    i = uid++;
  }

  init_all_virtual_tables();

  for (auto&[eid, entry] : global_symbol_table) {
    if (eid == "Main") {
      // we have to handle Main separately since function "main"
      // does not contain "this" pointer
      auto& ce = std::get<class_entry>(entry);
      auto& fe = ce.func_table.at("main");
      assert(fe.func_body.has_value());
      auto block_ptr = fe.func_body.value();
      auto main_func_type = llvm::FunctionType::get(builtin_types["int"], {}, false);
      auto main_func =
          llvm::Function::Create(main_func_type, llvm::Function::ExternalLinkage, "main", current_module.get());
      auto basic_block = llvm::BasicBlock::Create(current_context, "entry", main_func);
      builder.SetInsertPoint(basic_block);
      Codegen_visitor cv{};
      cv.visit(block_ptr);
      continue;
    }

    if (std::holds_alternative<class_entry>(entry)) {
      auto& ce = std::get<class_entry>(entry);
      auto& cid = eid;
      for (auto&[fid, fe] : ce.func_table) {
        define_func(cid, fid, fe);
      }
    }
  }

  current_module->print(llvm::errs(), nullptr);
}

llvm_driver::llvm_driver(const symbol_table& st) : builder(current_context), global_symbol_table(st) {
  current_module = std::make_unique<llvm::Module>("decaf", current_context);
  // TODO: bad usage of absolute path, use llvm::parseIR instead in future
  extern_module = llvm::parseIRFile("/home/auriaiur/decaf-compiler/build/runtime_lib.ll", err, current_context);
  assert(current_module);
  assert(extern_module);

  decaf_str_t = extern_module->getTypeByName("struct.decaf_str");
  v_entry_t = extern_module->getTypeByName("struct.v_entry");
  v_table_t = extern_module->getTypeByName("struct.v_table");

  assert(decaf_str_t);
  assert(v_entry_t);
  assert(v_table_t);

  // string is special primitive type, we access it by reference
  builtin_types["string"] = decaf_str_t->getPointerTo();
  builtin_types["int"] = builder.getInt32Ty();
  builtin_types["double"] = builder.getDoubleTy();
  builtin_types["bool"] = builder.getInt1Ty();
  builtin_types["void"] = builder.getVoidTy();
  builtin_types["nullptr"] = builder.getInt8PtrTy();

  constexpr std::string_view
      extern_funcs[] = {"print_str", "print_bool", "print_int", "print_double", "read_int", "read_line", "lookup_fptr"};

  // declare external library functions
  for (auto& fid : extern_funcs) {
    auto func_ptr = extern_module->getFunction(fid.data());
    assert(func_ptr);
    auto new_fptr = llvm::Function::Create(func_ptr->getFunctionType(),
                                           llvm::Function::ExternalLinkage,
                                           func_ptr->getName(),
                                           current_module.get());
    builtin_funcs[fid.data()] = new_fptr;
  }
}

// TODO: do we really need this method?
void llvm_driver::declare_func(const class_id& cid, const func_id& fid, const func_entry& fe) {
  auto return_type = get_llvm_type(fe.return_type);
  vector<llvm::Type*> params;
  // every function in decaf has an implicit `this' pointer
  params.push_back(get_llvm_type(cid));

  for (auto&[_, type] : fe.formal_table) {
    params.push_back(get_llvm_type(type));
  }
  auto func_type = llvm::FunctionType::get(return_type, params, false);
  auto
      func = llvm::Function::Create(func_type, llvm::Function::ExternalLinkage, cid + "." + fid, current_module.get());
  assert(func);
  user_defined_func_table[cid][fid] = func;
}

void llvm_driver::define_func(const class_id& cid, const func_id& fid, const func_entry& fe) {
  auto func = user_defined_func_table[cid][fid];
  auto basic_block = llvm::BasicBlock::Create(current_context, "entry", func);
  builder.SetInsertPoint(basic_block);
  assert(fe.func_body.has_value());
  auto scope_ptr = fe.func_body.value()->scope_ptr;
  // the parent scope of a member function scope is the class scope
  // and it must not be nullptr
  assert(scope_ptr->parent_scope_ptr);
  auto& ce = global_symbol_table.try_fetch_class(cid);

  bool is_this_ptr = true;
  // unique identifiers for local variables
  int uid = 0;
  for (auto& param : func->args()) {
    if (is_this_ptr) {
      // if param is "this" pointer
      is_this_ptr = false;
      assert(param.getType() == get_llvm_type(cid));
      for (auto &[vid, _]:ce.inheritance.field_table) {
        // create gep instruction in order to access class member variables in function
        auto& entry = scope_ptr->parent_scope_ptr->local_symbol_table[vid];
        auto val =
            builder.CreateGEP(&param,
                              {create_llvm_constant_signed_int(0),
                               create_llvm_constant_signed_int(std::get<0>(entry))});
        // set llvm::Value*
        scope_ptr->var_uid_to_llvm_value[std::get<0>(entry)] = val;
      }
      uid = ce.inheritance.field_table.size();
      continue;
    }

    auto local_var = builder.CreateAlloca(param.getType(), 0, nullptr, "arg_tmp");
    builder.CreateStore(&param, local_var);
    scope_ptr->var_uid_to_llvm_value[uid++] = local_var;
  }
}

void llvm_driver::init_all_virtual_tables() {
  for (auto&[eid, entry] : global_symbol_table) {
    if (std::holds_alternative<interface_entry>(entry) || eid == "Main") {
      continue;
    }
    vector<llvm::Constant*> v_entry_arr_val;
    auto& ce = std::get<class_entry>(entry);
    auto& cid = eid;
    // `func_decl_class' is declared as ordered map so that the entries of
    // each virtual table are well ordered
    for (auto&[fid, decl_cid] : ce.inheritance.func_decl_class) {
      llvm::Constant* v_entry_val;
      // llvm treat void* like int8ptr, so we need to convert
      // function to int8ptr using getBitCast
      if (decl_cid != cid) {
        // if current class doesn't override function fid, then
        // we should point the function pointer of current virtual
        // table entry to parent function
        v_entry_val =
            llvm::ConstantStruct::get(v_entry_t,
                                      {builder.getInt32(func_name_to_uid[fid]),
                                       llvm::ConstantExpr::getBitCast(user_defined_func_table[decl_cid][fid],
                                                                      builder.getInt8PtrTy())});
      } else {
        v_entry_val =
            llvm::ConstantStruct::get(v_entry_t,
                                      {builder.getInt32(func_name_to_uid[fid]),
                                       llvm::ConstantExpr::getBitCast(user_defined_func_table[cid][fid],
                                                                      builder.getInt8PtrTy())});
      }
      v_entry_arr_val.push_back(v_entry_val);
    }
    auto entry_num = ce.inheritance.func_decl_class.size();
    auto v_entry_arr_t = llvm::ArrayType::get(v_entry_t, entry_num);
    auto v_entry_arr = llvm::ConstantArray::get(v_entry_arr_t, v_entry_arr_val);
    // the type of the two member variables of v_table is int32 and v_entry*
    // TODO: we need to convert v_entry array type to v_entry pointer type
    auto v_table =
        llvm::ConstantStruct::get(v_table_t, {builder.getInt32(entry_num), v_entry_arr});

    // declare virtual table as a global variable in llvm IR
    auto var = new llvm::GlobalVariable(*current_module,
                                        v_table_t,
                                        true,
                                        llvm::GlobalVariable::PrivateLinkage, v_table,
                                        cid + ".v_table");
    class_virtual_table[cid] = var;
  }
}
