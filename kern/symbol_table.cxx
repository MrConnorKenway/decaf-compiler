#include "kern/symbol_table.h"

#include "build/static_semantic_analysis_visitor.h"

void func_entry::display(const string& fid, vector<bool>& is_last_bools,
                         bool& is_last) const {
  Indent i(is_last_bools, is_last);
  i.indent("Function: ", fid);
  {
    Indent i(is_last_bools, is_last);
    i.indent("parameter list");
    if (formal_table.empty()) {
      is_last = true;
      Indent i(is_last_bools, is_last);
      i.indent("void");
    } else {
      for (auto iter = formal_table.cbegin(); iter != formal_table.cend();
           ++iter) {
        auto [vid, vt] = *iter;
        is_last = std::next(iter) == formal_table.cend();
        Indent i(is_last_bools, is_last);
        i.indent(vid, ": ", vt);
      }
    }
  }
  if (func_body.has_value()) {
    Indent i(is_last_bools, is_last);
    i.indent("Function body");
    func_body.value()->scope_ptr->display(is_last_bools, is_last);
  }
  is_last = true;
  {
    Indent i(is_last_bools, is_last);
    i.indent("return type: ", return_type);
  }
}

void interface_entry::display(const string& iid, vector<bool>& is_last_bools,
                              bool& is_last) const {
  Indent i(is_last_bools, is_last);
  i.indent("Interface: ", iid);
  for (auto iter = cbegin(); iter != cend(); ++iter) {
    auto& [fid, fe] = *iter;
    is_last = std::next(iter) == cend();
    fe.display(fid, is_last_bools, is_last);
  }
}

void class_entry::display(const symbol_table& global_symbol_table, const string& cid,
                          vector<bool>& is_last_bools, bool& is_last) const {
  Indent i(is_last_bools, is_last);
  i.indent("Class: ", cid);
  {
    Indent i(is_last_bools, is_last);
    i.indent("Fields");
    for (auto iter = inheritance.field_table.cbegin();
         iter != inheritance.field_table.cend(); ++iter) {
      auto& [vid, ve] = *iter;
      is_last = std::next(iter) == inheritance.field_table.cend();
      Indent i(is_last_bools, is_last);
      auto& [uid, vt] = ve;
      i.indent(vid, " ", uid, " ", vt);
    }
  }

  {
    Indent i(is_last_bools, is_last);
    i.indent("Functions");
    for (auto iter = inheritance.func_decl_class.cbegin();
         iter != inheritance.func_decl_class.cend(); ++iter) {
      auto [fid, decl_class] = *iter;
      auto& fe = global_symbol_table.try_fetch_func(decl_class, fid);
      is_last = std::next(iter) == inheritance.func_decl_class.cend();
      fe.display(fid, is_last_bools, is_last);
    }
  }

  {
    is_last = true;
    Indent i(is_last_bools, is_last);
    i.indent("Interfaces");
    for (auto iter = inheritance.interface_ids.cbegin();
         iter != inheritance.interface_ids.cend(); ++iter) {
      auto iid = *iter;
      is_last = std::next(iter) == inheritance.interface_ids.cend();
      Indent i(is_last_bools, is_last);
      i.indent("Implements: ", iid);
    }
  }
}

void symbol_table::display(vector<bool>& is_last_bools, bool& is_last) const {
  cout << endl << "Global Symbol Table" << endl;
  for (auto iter = cbegin(); iter != cend(); ++iter) {
    auto& [eid, e] = *iter;
    is_last = std::next(iter) == cend();
    if (std::holds_alternative<class_entry>(e)) {
      std::get<class_entry>(e).display(*this, eid, is_last_bools, is_last);
    } else {
      std::get<interface_entry>(e).display(eid, is_last_bools, is_last);
    }
  }
}