#include "kern/static_semantic_analyser.h"

class_entry& static_semantic_analyser::try_fetch_class(string cid) {
  ss_assert(global_symbol_table.count(cid),
            "Undefined reference to class \"%s\"\n", cid.c_str());
  return std::get<class_entry>(global_symbol_table[cid]);
}

func_entry& static_semantic_analyser::try_fetch_func(string cid, string fid) {
  ss_assert(global_symbol_table.count(cid),
            "Undefined reference to class \"%s\"\n", cid.c_str());
  auto& ce = std::get<class_entry>(global_symbol_table[cid]);
  ss_assert(ce.func_table.count(fid),
            "Undefined reference to function \"%s\" in class \"%s\"\n",
            fid.c_str(), cid.c_str());
  return ce.func_table[fid];
}

interface_entry& static_semantic_analyser::try_fetch_interface(string iid) {
  ss_assert(global_symbol_table.count(iid),
            "Undefined reference to interface \"%s\"\n", iid.c_str());
  return std::get<interface_entry>(global_symbol_table[iid]);
}

void static_semantic_analyser::visit(string id, symbol_table_entry& entry) {
  if (std::holds_alternative<class_entry>(entry)) {
    analyse(id, std::get<class_entry>(entry));
  } else {
    analyse(id, std::get<interface_entry>(entry));
  }
}

void static_semantic_analyser::analyse(
    string iid, interface_entry& current_interface_entry) {
  // TODO
}

void static_semantic_analyser::analyse(string cid,
                                       class_entry& current_class_entry) {
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
    ss_assert(false, "Found cyclic inheritance in class \"%s\"\n", cid.c_str());
  }

  if (current_class_entry.parent_class_id != "") {
    auto& parent_class_entry =
        try_fetch_class(current_class_entry.parent_class_id);
    analyse(current_class_entry.parent_class_id, parent_class_entry);

    // inherit everything from parent class
    current_class_entry.inheritance = parent_class_entry.inheritance;
  }

  // try insert variables that are declared in current class into inheritance
  for (auto [vid, ve] : current_class_entry.field_table) {
    current_class_entry.inheritance.field_table.try_emplace(std::move(vid),
                                                            std::move(ve));
  }

  for (auto& [fid, fe] : current_class_entry.func_table) {
    if (current_class_entry.inheritance.func_decl_class.count(fid)) {
      // if current class tries to override an inherited function
      auto decl_class_id = current_class_entry.inheritance.func_decl_class[fid];
      auto& prototype = try_fetch_func(decl_class_id, fid);
      ss_assert(fe == prototype,
                "Overloading inherited function \"%s\" from parent class "
                "\"%s\" is not allowed\n",
                fid.c_str(), decl_class_id.c_str());
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

    auto& ie = try_fetch_interface(iid);
    // for every prototype declaration in interface entry ie,
    // there should be corresponding function declaration in
    // current class' function table
    for (auto& [fid, prototype] : ie) {
      auto& fe = try_fetch_func(cid, fid);
      ss_assert(fe == prototype,
                "Prototype of function \"%s\" mismatches interface \"%s\"\n",
                fid.c_str(), iid.c_str());
    }

    // insert the implemented interface into inheritance
    current_class_entry.inheritance.interface_ids.insert(iid);
  }

  // we have finished visiting cid
  is_visited[cid] = true;
}

void static_semantic_analyser::analyse() {
  for (auto [id, entry] : global_symbol_table) {
    visit(id, entry);
  }

  Static_semantic_analysis_visitor sv(global_symbol_table);
  for (auto& [eid, e] : global_symbol_table) {
    if (std::holds_alternative<class_entry>(e)) {
      auto& ce = std::get<class_entry>(e);
      for (auto& [fid, fe] : ce.func_table) {
        sv.visit(fe.func_body.value());
      }
    }
  }
}