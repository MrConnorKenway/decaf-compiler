#include "kern/static_semantic_analyser.h"

int scope::next_tid = 0;

void static_semantic_analyser::analyse(string cid,
                                       class_entry& current_class_entry) {
  yylloc_manager y(current_class_entry.classdecl_node_ptr);
  if (is_visited.count(cid) == 0) {
    is_visited[cid] = false;
  } else {
    if (is_visited[cid]) {
      return;
    }
    // if "is_visited" contains cid, but is_visited[cid] is false, this
    // means that the visiting of cid is not finished yet, so we have to
    // visit the parent class of cid first. However, while visiting the
    // ancestors, we encounter cid again, which leads to a cyclic inheritance
    ss_assert(false, "Found cyclic inheritance in class ", cid);
  }

  if (current_class_entry.parent_class_id != "") {
    auto& parent_class_entry = global_symbol_table.try_fetch_class(
        current_class_entry.parent_class_id);
    analyse(current_class_entry.parent_class_id, parent_class_entry);

    // inherit everything from parent class
    current_class_entry.inheritance = parent_class_entry.inheritance;
  }

  // try insert variables that are declared in current class into inheritance
  for (auto [vid, vt] : current_class_entry.field_table) {
    current_class_entry.inheritance.field_table.try_emplace(std::move(vid),
                                                            std::move(vt));
  }

  for (auto& [fid, fe] : current_class_entry.func_table) {
    if (current_class_entry.inheritance.func_decl_class.count(fid)) {
      // if current class tries to override an inherited function
      auto decl_class_id = current_class_entry.inheritance.func_decl_class[fid];
      auto& prototype = global_symbol_table.try_fetch_func(decl_class_id, fid);
      ss_assert(fe == prototype, "Class ", cid,
                ": overloading inherited function ", fid, " from parent class ",
                decl_class_id, " is not allowed\n");
    }

    // update or insert the declare class of fid into inheritance
    current_class_entry.inheritance.func_decl_class[fid] = cid;
  }

  // check whether this class does implement target interface
  for (auto iid : current_class_entry.implemented_interface_set) {
    if (current_class_entry.inheritance.interface_ids.count(iid)) {
      // if parent class implements the same interface, since
      // we always analyse parent class first, so it must be
      // the case that this class does implement target interface
      continue;
    }

    auto& ie = global_symbol_table.try_fetch_interface(iid);
    // for every prototype declaration in interface entry ie,
    // there should be corresponding function declaration in
    // current class' function table
    for (auto& [fid, prototype] : ie) {
      auto& fe = global_symbol_table.try_fetch_func(cid, fid);
      ss_assert(fe == prototype, "Prototype of function ", fid,
                " mismatches interface ", iid);
    }

    // insert the implemented interface into inheritance
    current_class_entry.inheritance.interface_ids.insert(iid);
  }

  // we have finished visiting cid
  is_visited[cid] = true;
}

void static_semantic_analyser::analyse() {
  for (auto& [id, entry] : global_symbol_table) {
    if (std::holds_alternative<class_entry>(entry)) {
      analyse(id, std::get<class_entry>(entry));
    }
  }

  vector<bool> is_last_bools;
  bool is_last;
  global_symbol_table.display(is_last_bools, is_last);

  for (auto& [eid, e] : global_symbol_table) {
    if (std::holds_alternative<class_entry>(e)) {
      auto& ce = std::get<class_entry>(e);
      Static_semantic_analysis_visitor sv(global_symbol_table, ce,
                                          std::move(eid));
      for (auto& [fid, fe] : ce.func_table) {
        ss_assert(fe.func_body.has_value(), "function ", fid, " has no body");
        sv.current_func_id = fid;
        auto& func_body_ptr = fe.func_body.value();
        func_body_ptr->scope_ptr = new scope(fe);
        sv.visit(func_body_ptr);
      }
    }
  }
}
