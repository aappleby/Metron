#include "MtTracer.h"

#include "MtModule.h"
#include "MtSourceFile.h"
#include "MtModLibrary.h"

#include "metron_tools.h"

//-----------------------------------------------------------------------------
// Given two blocks that execute as branches, merge their dependencies and
// writes together.

bool merge_branch(MtDelta& a, MtDelta& b, MtDelta& out) {
  out.wipe();

  // Delta A's input and delta B's input must agree about the state of all
  // matching fields.

  for (auto& s1 : a.state_old) {
    if (b.state_old.contains(s1.first)) {
      auto& s2 = *b.state_old.find(s1.first);
      assert(s1.second != ERROR);
      assert(s2.second != ERROR);
      if (s1.second != s2.second) return false;
    }
  }

  // The merged state inputs is the union of the two delta's inputs.

  for (auto& s1 : a.state_old) out.state_old[s1.first] = s1.second;
  for (auto& s2 : b.state_old) out.state_old[s2.first] = s2.second;

  // If the two deltas disagree on the state of an output, we set it to "maybe".

  for (auto& s : a.state_new) out.state_new[s.first] = ERROR;
  for (auto& s : b.state_new) out.state_new[s.first] = ERROR;

  for (auto& s : out.state_new) {
    auto& an = a.state_new[s.first];
    auto& bn = b.state_new[s.first];

    s.second = an == bn ? an : MAYBE;
  }

  return true;
}

//------------------------------------------------------------------------------
// Given two blocks that execute in series, merge their dependencies and
// writes together.

bool merge_series(MtDelta& a, MtDelta& b, MtDelta& out) {
  // Delta A's output must be compatible with delta B's input.

  for (auto& s2 : b.state_old) {
    if (a.state_new.contains(s2.first)) {
      auto& s1 = *a.state_new.find(s2.first);
      assert(s1.second != ERROR);
      assert(s2.second != ERROR);
      if (s1.second != s2.second) {
        /*
        log_error(
          MnNode::null,
          "merge_series error - a.state_new %s = %s, b.state_old %s = %s\n",
          s1.first.c_str(), to_string(s1.second), s2.first.c_str(),
          to_string(s2.second));
        */
        LOG_R("merge_series error - a.state_new %s = %s, b.state_old %s = %s\n", s1.first.c_str(), to_string(s1.second), s2.first.c_str(), to_string(s2.second));
        out.error = true;
        return false;
      }
    }
  }

  // The resulting state is just delta B applied on top of delta A.

  out.state_old = a.state_old;
  out.state_new = a.state_new;

  for (auto& s2 : b.state_old) out.state_old[s2.first] = s2.second;
  for (auto& s2 : b.state_new) out.state_new[s2.first] = s2.second;

  return true;
}

//------------------------------------------------------------------------------

void MtTracer::trace_dispatch(MnNode n) {
  switch(n.sym) {
  case sym_assignment_expression: trace_assign(n);   break;
  case sym_call_expression:       trace_call(n);     break;
  case sym_identifier:            trace_id(n);       break;
  case sym_if_statement:          trace_if(n);       break;
  case sym_switch_statement:      trace_switch(n);   break;
  default:                        trace_children(n); break;
  }
}

//------------------------------------------------------------------------------

void MtTracer::trace_children(MnNode n) {
  for (auto c : n) {
    depth++;
    trace_dispatch(c);
    depth--;
  }
}

//------------------------------------------------------------------------------

void MtTracer::trace_assign(MnNode n) {
  for (int i = 0; i < depth; i++) printf(" ");
  printf("ASSIGNMENT %s\n", n.text().c_str());
  trace_children(n);
}

//------------------------------------------------------------------------------
// FIXME I need to traverse the args before stepping into the call

void MtTracer::trace_call(MnNode n) {
  auto node_func = n.get_field(field_function);

  if (node_func.sym == sym_field_expression) {
    for (int i = 0; i < depth; i++) printf(" ");
    printf("FCALL! %s.%s -> %s\n",
      mod()->name().c_str(), method()->name().c_str(),
      node_func.text().c_str());

    // Field call. Pull up the submodule and traverse into the method.

    //node_func.dump_tree();
    auto submod_name = node_func.get_field(field_argument).text();
    auto submod = mod()->get_submod(submod_name);
    assert(submod);
    auto submod_type = submod->type_name();
    auto submod_mod = mod()->source_file->lib->get_module(submod_type);
    assert(submod_mod);
    auto submod_method = submod_mod->get_method(node_func.get_field(field_field).text());
    assert(submod_method);

    //for (int i = 0; i < depth + 1; i++) printf(" ");
    //printf("TO SUBMOD!\n");

    depth++;
    mod_stack.push_back(submod_mod);
    method_stack.push_back(submod_method);
    trace_dispatch(submod_method->node.get_field(field_body));
    method_stack.pop_back();
    mod_stack.pop_back();
    depth--;

  } else if (node_func.sym == sym_identifier) {
    //node_func.dump_tree();

    auto sibling_method = mod()->get_method(node_func.text());

    if (sibling_method) {
      for (int i = 0; i < depth; i++) printf(" ");
      printf("MCALL! %s.%s -> %s\n",
        mod()->name().c_str(), method()->name().c_str(),
        node_func.text().c_str());
      depth++;
      method_stack.push_back(sibling_method);
      trace_dispatch(sibling_method->node.get_field(field_body));
      method_stack.pop_back();
      depth--;
    }
    else {
      // Utility method call like bN()
      /*
      for (int i = 0; i < depth; i++) printf(" ");
      printf("XCALL! %s.%s -> %s\n",
      name().c_str(), method->name().c_str(),
      node_func.text().c_str());
      */
    }

  } else if (node_func.sym == sym_template_function) {

    auto sibling_method = mod()->get_method(node_func.get_field(field_name).text());

    if (sibling_method) {
      // Should probably not see any of these yet?
      debugbreak();
      for (int i = 0; i < depth; i++) printf(" ");
      printf("TCALL! %s.%s -> %s\n",
        mod()->name().c_str(), method()->name().c_str(),
        node_func.text().c_str());
    }
    else {
      // Templated utility method call like bx<>, dup<>
    }

    //node_func.dump_tree();

  } else {
    // printf("  CALL! %s\n", c.text().c_str());
    // printf("  CALL! %s\n", c.get_field(field_function).text().c_str());

    LOG_R("MtModule::build_call_tree - don't know what to do with %s\n",
      n.ts_node_type());
    n.dump_tree();
    debugbreak();
  }
}

//------------------------------------------------------------------------------

void MtTracer::trace_id(MnNode n) {
  if (mod()->get_field(n.text())) {
    for (int i = 0; i < depth; i++) printf(" ");
    printf("{}{}{}{}{} id %s\n", n.text().c_str());
  }
}

//------------------------------------------------------------------------------

void MtTracer::trace_if(MnNode n) {
  for (int i = 0; i < depth; i++) printf(" ");
  printf("IF STATEMENT\n");
}

//------------------------------------------------------------------------------

void MtTracer::trace_switch(MnNode n) {
  for (int i = 0; i < depth; i++) printf(" ");
  printf("SWITCH STATEMENT\n");
  trace_children(n);
}

//------------------------------------------------------------------------------











#if 0

void MtMethod::update_delta() {
  if (delta == nullptr) {
    auto temp_delta = new MtDelta();
    auto body = node.get_field(field_body);
    //check_dirty_dispatch(body, *temp_delta);
    delta = temp_delta;
  }
}

void MtMethod::check_dirty_dispatch(MnNode n, MtDelta& d) {
  for (auto& n : d.state_old) assert(n.second != ERROR);
  for (auto& n : d.state_new) assert(n.second != ERROR);

  if (n.is_null() || !n.is_named()) return;

  switch (n.sym) {
  case sym_field_expression:
    check_dirty_read_submod(n, d);
    break;
  case sym_identifier:
    check_dirty_read_identifier(n, d);
    break;
  case sym_assignment_expression:
    check_dirty_assign(n, d);
    break;
  case sym_if_statement:
    check_dirty_if(n, d);
    break;
  case sym_call_expression: {
    if (n.get_field(field_function).sym == sym_field_expression) {
      // submod.tick()/tock()
      check_dirty_call(n, d);
    } else if (n.get_field(field_function).sym == sym_template_function) {
      // foo = bx<x>(bar);
      check_dirty_call(n, d);
    } else {
      // cat() etc
      check_dirty_call(n, d);
    }
    break;
  }
  case sym_switch_statement:
    check_dirty_switch(n, d);
    break;

  default: {
    for (auto c : n) check_dirty_dispatch(c, d);
  }
  }
}

//------------------------------------------------------------------------------

void MtMethod::check_dirty_read_identifier(MnNode n, MtDelta& d) {
  assert(n.sym == sym_identifier);
  auto field = n.text();

  // Only check reads of regs and outputs.
  if (!mod->get_register(field) && !mod->get_output(field)) return;

  // Reading from a dirty field in tick() is forbidden.
  if (is_tick) {
    if (d.state_new.contains(field)) {
      if (d.state_new[field] == MAYBE) {
        log_error(n, "%s() read maybe new field - %s\n", name().c_str(),
          field.c_str());
        d.error = true;
      } else if (d.state_new[field] == DIRTY) {
        log_error(n, "%s() read dirty new field - %s\n", name().c_str(),
          field.c_str());
        d.error = true;
      }
    } else if (d.state_old.contains(field)) {
      if (d.state_old[field] == MAYBE) {
        log_error(n, "%s() read maybe old field - %s\n", name().c_str(),
          field.c_str());
        d.error = true;
      } else if (d.state_old[field] == DIRTY) {
        log_error(n, "%s() read dirty old field - %s\n", name().c_str(),
          field.c_str());
        d.error = true;
      }
    } else {
      // Haven't seen this field before. We're in tock(), so we expect it to be
      // clean.
      d.state_old[field] = CLEAN;
      d.state_new[field] = CLEAN;
    }
  }

  // Reading from a clean field in tock() is forbidden.
  if (is_tock) {
    if (d.state_new.contains(field)) {
      if (d.state_new[field] == CLEAN) {
        log_error(n, "%s() read clean new field - %s\n", name().c_str(),
          field.c_str());
        d.error = true;
      } else if (d.state_new[field] == MAYBE) {
        log_error(n, "%s() read maybe new field - %s\n", name().c_str(),
          field.c_str());
        d.error = true;
      }
    } else if (d.state_old.contains(field)) {
      if (d.state_old[field] == CLEAN) {
        log_error(n, "%s() read clean old field - %s\n", name().c_str(),
          field.c_str());
        d.error = true;
      } else if (d.state_old[field] == MAYBE) {
        log_error(n, "%s() read maybe old field - %s\n", name().c_str(),
          field.c_str());
        d.error = true;
      }
    } else {
      // Haven't seen this field before. We're in tock(), so we expect it to be
      // dirty.
      d.state_old[field] = DIRTY;
      d.state_new[field] = DIRTY;
    }
  }
}

//------------------------------------------------------------------------------

void MtMethod::check_dirty_read_submod(MnNode n, MtDelta& d) {
  assert(n.sym == sym_field_expression);
  auto field = n.text();

  auto dot_pos = field.find('.');
  std::string submod_name = field.substr(0, dot_pos);
  std::string submod_field = field.substr(dot_pos + 1, field.size());
  auto submod_node = mod->get_submod(submod_name);
  //auto submod_mod = submod_node->mod;

  /*
  if (!submod_mod->has_output(submod_field)) {
  log_error(n, "%s() read non-output from submodule - %s\n", name.c_str(),
  field.c_str());
  d.error = true;
  }
  */

  //----------

  // Reading from a dirty field in tick() is forbidden.
  if (is_tick) {
    if (d.state_new.contains(field)) {
      if (d.state_new[field] != CLEAN) {
        log_error(n, "%s() read dirty new submod field - %s\n", name().c_str(),
          field.c_str());
        d.error = true;
      }
    } else if (d.state_old.contains(field)) {
      if (d.state_old[field] != CLEAN) {
        log_error(n, "%s() read dirty old submod field - %s\n", name().c_str(),
          field.c_str());
        d.error = true;
      }
    } else {
      d.state_old[field] = CLEAN;
      d.state_new[field] = CLEAN;
    }
  }

  // Reading from a clean field in tock() is forbidden.
  if (is_tock) {
    if (d.state_new.contains(field)) {
      if (d.state_new[field] == CLEAN) {
        log_error(n, "%s() read clean new submod field - %s\n", name().c_str(),
          field.c_str());
        d.error = true;
      }
    } else if (d.state_old.contains(field)) {
      if (d.state_old[field] == CLEAN) {
        log_error(n, "%s() read clean old submod field - %s\n", name().c_str(),
          field.c_str());
        d.error = true;
      }
    } else {
      d.state_old[field] = DIRTY;
      d.state_new[field] = DIRTY;
    }
  }
} 

//------------------------------------------------------------------------------

void MtMethod::check_dirty_write(MnNode n, MtDelta& d) {
  // If the LHS is a subscript expression, check the source field.
  if (n.sym == sym_subscript_expression) {
    return check_dirty_write(n.get_field(field_argument), d);
  }

  // If the LHS is a slice expression, check the source field.
  if (n.sym == sym_call_expression) {
    auto func_name = n.get_field(field_function).text();
    for (int i = 0; i < func_name.size(); i++) {
      assert(i ? isdigit(func_name[i]) : func_name[i] == 's');
    }
    auto field_node = n.get_field(field_arguments).named_child(0);
    return check_dirty_write(field_node, d);
  }

  //----------

  assert(n.sym == sym_identifier);
  auto field = n.text();

  // Writing to inputs is forbidden.
  assert(!mod->get_input(field));

  // Only check writes to regs and outputs

  bool field_is_register = mod->get_register(field);
  bool field_is_output = mod->get_output(field);

  if (!field_is_register && !field_is_output) return;

  //----------

  // allowing reg outputs means outputs can change in tick()
  /*
  if (is_tick && field_is_output) {
  log_error(n, "%s() wrote output - %s\n", name.c_str(), field.c_str());
  d.error = true;
  }
  */

  if (is_tock && field_is_register) {
    log_error(n, "%s() wrote reg - %s\n", name().c_str(), field.c_str());
    d.error = true;
  }

  if (d.state_old.contains(field)) {
    if (d.state_old[field] != CLEAN) {
      log_error(n, "%s() wrote dirty old field - %s\n", name().c_str(),
        field.c_str());
      d.error = true;
    }
  }

  // multiple writes doesn't actually break anything
  /*
  if (d.state_new.contains(field)) {
  if (d.state_new[field] != CLEAN) {
  log_error(n, "%s() wrote dirty new field - %s\n", name.c_str(),
  field.c_str());
  d.error = true;
  }
  }
  */

  d.state_old[field] = CLEAN;
  d.state_new[field] = DIRTY;
}

//------------------------------------------------------------------------------
// Check for reads on the RHS of an assignment, then check the write on the
// left.

void MtMethod::check_dirty_assign(MnNode n, MtDelta& d) {
  auto lhs = n.get_field(field_left);
  auto rhs = n.get_field(field_right);

  check_dirty_dispatch(rhs, d);
  check_dirty_write(lhs, d);
}

//------------------------------------------------------------------------------
// Check the "if" branch and the "else" branch independently and then merge the
// results.

void MtMethod::check_dirty_if(MnNode n, MtDelta& d) {
  check_dirty_dispatch(n.get_field(field_condition), d);

  MtDelta if_delta = d;
  MtDelta else_delta = d;

  check_dirty_dispatch(n.get_field(field_consequence), if_delta);
  check_dirty_dispatch(n.get_field(field_alternative), else_delta);

  merge_branch(if_delta, else_delta, d);
}

//------------------------------------------------------------------------------
// Traverse function calls.

void MtMethod::check_dirty_call(MnNode n, MtDelta& d) {
  auto node_call = MnCallExpr(n);
  auto node_func = node_call.get_field(field_function);
  auto node_args = node_call.get_field(field_arguments);

  MtField*  call_member = nullptr;
  MtModule* call_submod = nullptr;
  MtMethod* call_method = nullptr;

  if (node_call.sym == sym_field_expression) {
    auto node_this = node_call.get_field(field_argument);
    auto node_method = node_call.get_field(field_field);

    if (node_method.text() == "as_signed") {
    } else {
      call_member = mod->get_submod(node_this.text());
      call_submod = mod->source_file->lib->get_mod(call_member->type_name());
      call_method = call_submod->get_method(node_method.text());
    }
  }

  assert(node_args.sym == sym_argument_list);
  check_dirty_dispatch(node_args, d);

  if (node_func.sym == sym_identifier) {
    // local function call, traverse args and then function body
    // TODO - traverse function body
    // printf("%s\n", node_func.text().c_str());
    // n.dump_tree();
    // debugbreak();

    // We hit this in b9(x) calls and whatnot, we need to sort them out and
    // resolve the local task/function calls.

  } else if (node_func.sym == sym_field_expression) {
    call_method->update_delta();

    MtDelta temp_delta = d;
    MtDelta call_delta = *call_method->delta;
    call_delta.add_prefix(call_member->name());

    merge_series(temp_delta, call_delta, d);
  } else if (node_func.sym == sym_template_function) {
    // local function call, probably bx<n>(things);
    // n.dump_tree();
    // debugbreak();
  } else {
    n.dump_tree();
    debugbreak();
  }
}

//------------------------------------------------------------------------------
// Check the condition of a switch statement, then check each case
// independently.

void MtMethod::check_dirty_switch(MnNode n, MtDelta& d) {
  check_dirty_dispatch(n.get_field(field_condition), d);

  MtDelta init_delta = d;
  MtMethod old_method = *this;

  bool first_branch = true;

  auto body = n.get_field(field_body);
  for (auto c : body) {
    if (c.sym == sym_case_statement) {
      // c.dump_tree();
      MtDelta case_delta = init_delta;
      check_dirty_dispatch(c, case_delta);

      if (first_branch) {
        d = case_delta;
        first_branch = false;
      } else {
        MtDelta temp = d;
        merge_branch(temp, case_delta, d);
      }
    }
  }
}

#endif
//------------------------------------------------------------------------------