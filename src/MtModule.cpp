#include "MtModule.h"

#include "Log.h"
//#include "MtCursor.h"
#include <deque>

#include "MtModLibrary.h"
#include "MtNode.h"
#include "MtSourceFile.h"
#include "MtTracer.h"
#include "Platform.h"
#include "metron_tools.h"
#include "submodules/tree-sitter/lib/include/tree_sitter/api.h"

#pragma warning(disable : 4996)  // unsafe fopen()

extern "C" {
extern const TSLanguage *tree_sitter_cpp();
}

MtField::MtField(MtModule *_parent_mod, const MnNode &n, bool is_public)
    : _parent_mod(_parent_mod), _public(is_public), node(n) {
  assert(node.sym == sym_field_declaration);
  _name = node.name4();
  _type = node.type5();
  _type_mod = _parent_mod->lib->get_module(_type);
}

//------------------------------------------------------------------------------

void MtInstance::dump() const {
  LOG_G("%s", get_path().c_str());

  if (field) {
    if (field->_type_mod) {
      LOG_G(" : %s", field->_type_mod->cname());
    } else {
      LOG_G(" : %s", field->_type.c_str());
    }
  }

  LOG_G("\n");

  // LOG_INDENT_SCOPE();
  for (auto inst : children) {
    inst->dump();
  }
}

//------------------------------------------------------------------------------

MtInstance *MtField::instantiate(MtInstance *parent) {
  MtInstance *result = new MtInstance();
  result->parent = parent;
  result->name = _name;
  result->field = this;
  result->mod = _parent_mod->lib->get_module(_type);

  if (result->mod) {
    for (auto f : result->mod->all_fields) {
      result->children.push_back(f->instantiate(result));
    }
  }
  return result;
}

static MtField *construct(MtModule *mod, const MnNode &n, bool is_public) {
  return new MtField(mod, n, is_public);
}

//------------------------------------------------------------------------------

bool MtField::is_component() const {
  return (node.source->lib->get_module(_type));
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtModule::init(MtSourceFile *_source_file,
                                MnTemplateDecl _node) {
  Err err;

  source_file = _source_file;
  lib = source_file->lib;
  mod_template = _node;

  for (int i = 0; i < mod_template.child_count(); i++) {
    auto child = mod_template.child(i);

    if (child.sym == sym_template_parameter_list) {
      mod_param_list = MnTemplateParamList(child);
    }

    if (child.sym == sym_class_specifier) {
      mod_class = MnClassSpecifier(child);
    }
  }

  if (mod_param_list.is_null()) {
    err << ERR("No template parameter list found under template");
  }

  if (mod_class) {
    mod_name = mod_class.get_field(field_name).text();
  } else {
    err << ERR("No class node found under template");
  }

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtModule::init(MtSourceFile *_source_file,
                                MnClassSpecifier _node) {
  Err err;

  source_file = _source_file;
  mod_template = MnNode::null;
  mod_param_list = MnNode::null;
  mod_class = _node;

  if (mod_class) {
    mod_name = mod_class.get_field(field_name).text();
  } else {
    err << ERR("mod_class is null");
  }

  return err;
}

//------------------------------------------------------------------------------

MtMethod *MtModule::get_method(const std::string &name) {
  for (auto n : all_methods)
    if (n->name() == name) return n;
  return nullptr;
}

MtField *MtModule::get_field(const std::string &name) {
  for (auto f : all_fields) {
    if (f->name() == name) return f;
  }
  return nullptr;
}

#if 0
MtField* MtModule::get_enum(const std::string &name) {
  for (auto n : all_enums) {
    if (n->name() == name) return n;
  }
  return nullptr;
}

MtField *MtModule::get_input_field(const std::string &name) {
  for (auto f : input_signals)
    if (f->name() == name) return f;
  return nullptr;
}

MtParam *MtModule::get_input_param(const std::string &name) {
  for (auto p : input_arguments)
    if (p->name() == name) return p;
  return nullptr;
}

MtField *MtModule::get_output_signal(const std::string &name) {
  for (auto n : output_signals)
    if (n->name() == name) return n;
  return nullptr;
}

MtField *MtModule::get_output_register(const std::string &name) {
  for (auto n : public_registers)
    if (n->name() == name) return n;
  return nullptr;
}

MtMethod *MtModule::get_output_return(const std::string &name) {
  for (auto n : output_returns)
    if (n->name() == name) return n;
  return nullptr;
}

MtField *MtModule::get_component(const std::string &name) {
  for (auto n : components) {
    if (n->name() == name) return n;
  }
  return nullptr;
}
#endif

//------------------------------------------------------------------------------

void MtModule::dump_method_list(const std::vector<MtMethod *> &methods) const {
  for (auto n : methods) {
    LOG_INDENT_SCOPE();
    LOG_R("%s(", n->name().c_str());

    if (n->param_nodes.size()) {
      int param_count = int(n->param_nodes.size());
      int param_index = 0;

      for (auto &param : n->param_nodes) {
        LOG_R("%s", param.text().c_str());
        if (param_index++ != param_count - 1) LOG_R(", ");
      }
    }
    LOG_R(")\n");
  }
}

//------------------------------------------------------------------------------

void MtModule::dump_banner() const {
  LOG_Y("//----------------------------------------\n");
  if (mod_class.is_null()) {
    LOG_Y("// Package %s\n", source_file->full_path.c_str());
    LOG_Y("\n");
    return;
  }

  LOG_Y("// Module %s", mod_name.c_str());
  if (parents.empty()) LOG_Y(" ########## TOP ##########");
  LOG_Y("\n");

  //----------

  LOG_B("Modparams:\n");
  for (auto param : all_modparams) LOG_G("  %s\n", param->name().c_str());

  LOG_B("Localparams:\n");
  for (auto param : localparams) LOG_G("  %s\n", param->name().c_str());

  LOG_B("Enums:\n");
  for (auto n : all_enums) LOG_G("  %s\n", n->name().c_str());

  LOG_B("All Fields:\n");
  for (auto n : all_fields) {
    LOG_G("  %s:%s %s\n", n->name().c_str(), n->type_name().c_str(),
          to_string(n->state));
  }

  LOG_B("Input Fields:\n");
  for (auto n : input_signals)
    LOG_G("  %s:%s\n", n->name().c_str(), n->type_name().c_str());

  LOG_B("Input Params:\n");
  for (auto n : input_arguments)
    LOG_G("  %s:%s\n", n->name().c_str(), n->type_name().c_str());

  LOG_B("Output Signals:\n");
  for (auto n : output_signals)
    LOG_G("  %s:%s\n", n->name().c_str(), n->type_name().c_str());

  LOG_B("Output Registers:\n");
  for (auto n : public_registers)
    LOG_G("  %s:%s\n", n->name().c_str(), n->type_name().c_str());

  LOG_B("Output Returns:\n");
  for (auto n : output_returns)
    LOG_G("  %s:%s\n", n->name().c_str(),
          n->_node.get_field(field_type).text().c_str());

  /*
  LOG_B("Regs:\n");
  for (auto n : registers)
    LOG_G("  %s:%s\n", n->name().c_str(), n->type_name().c_str());
  */

  LOG_B("Components:\n");
  for (auto component : components) {
    auto component_mod = source_file->lib->get_module(component->type_name());
    LOG_G("  %s:%s\n", component->name().c_str(),
          component_mod->mod_name.c_str());
  }

  //----------

  LOG_B("Init methods:\n");
  dump_method_list(init_methods);

  LOG_B("Tick methods:\n");
  dump_method_list(tick_methods);

  LOG_B("Tock methods:\n");
  dump_method_list(tock_methods);

  LOG_B("Tasks:\n");
  dump_method_list(task_methods);

  LOG_B("Functions:\n");
  dump_method_list(func_methods);

  //----------

  /*
  LOG_B("Port map:\n");
  for (auto &kv : port_map)
    LOG_G("  %s = %s\n", kv.first.c_str(), kv.second.c_str());
  */

  /*
  LOG_B("State map:\n");
  for (auto &kv : mod_states.get_map())
    LOG_G("  %s = %s\n", kv.first.c_str(), to_string(kv.second));
  */

  LOG_B("\n");
}

//------------------------------------------------------------------------------

#if 0
void MtModule::dump_deltas() const {
  if (mod_struct.is_null()) return;

  LOG_G("%s\n", mod_name.c_str());
  {
    LOG_INDENT_SCOPE();

    for (auto tick : tick_methods) {
      LOG_G("%s error %d\n", tick->name().c_str(), tick->delta->error);
      LOG_INDENT_SCOPE();
      {
        for (auto &s : tick->delta->state_old) {
          LOG_G("%s", s.first.c_str());
          LOG_W(" : ");
          log_field_state(s.second);

          if (tick->delta->state_new.contains(s.first)) {
            auto s2 = tick->delta->state_new[s.first];
            if (s2 != s.second) {
              LOG_W(" -> ");
              log_field_state(s2);
            }
          }

          LOG_G("\n");
        }
      }
    }

    for (auto tock : tock_methods) {
      LOG_G("%s error %d\n", tock->name().c_str(), tock->delta->error);
      LOG_INDENT_SCOPE();
      {
        for (auto &s : tock->delta->state_old) {
          LOG_G("%s", s.first.c_str());
          LOG_W(" : ");
          log_field_state(s.second);

          if (tock->delta->state_new.contains(s.first)) {
            auto s2 = tock->delta->state_new[s.first];
            if (s2 != s.second) {
              LOG_W(" -> ");
              log_field_state(s2);
            }
          }

          LOG_G("\n");
        }
      }
    }
  }
  LOG_G("\n");
}
#endif

//------------------------------------------------------------------------------

CHECK_RETURN Err MtModule::collect_modparams() {
  Err err;

  if (mod_template) {
    auto params = mod_template.get_field(field_parameters);

    for (int i = 0; i < params.named_child_count(); i++) {
      auto child = params.named_child(i);
      if (child.sym == sym_parameter_declaration ||
          child.sym == sym_optional_parameter_declaration) {
        all_modparams.push_back(MtParam::construct("<template>", child));
      } else {
        err << ERR("Unknown template parameter type %s", child.ts_node_type());
      }
    }
  }

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtModule::collect_fields() {
  Err err;

  assert(all_fields.empty());
  assert(components.empty());
  assert(localparams.empty());

  auto mod_body = mod_class.get_field(field_body).check_null();
  bool in_public = false;

  for (const auto &n : mod_body) {
    if (n.sym == sym_access_specifier) {
      in_public = n.child(0).text() == "public";
    }

    if (n.sym == sym_field_declaration) {
      auto new_field = MtField::construct(this, n, in_public);
      all_fields.push_back(new_field);
    }
  }

  return err;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtModule::collect_methods() {
  Err err;

  assert(all_methods.empty());

  bool in_public = false;
  auto mod_body = mod_class.get_field(field_body).check_null();

  //----------
  // Create method objects for all function defition nodes

  for (const auto &n : mod_body) {
    if (n.sym == sym_access_specifier) {
      in_public = n.child(0).text() == "public";
    }

    if (n.sym == sym_function_definition) {
      auto func_decl = n.get_field(field_declarator);
      auto func_name = func_decl.get_field(field_declarator).name4();
      auto func_args = func_decl.get_field(field_parameters);

      MtMethod *method = MtMethod::construct(n, this, source_file->lib);

      method->is_public = in_public;

      for (int i = 0; i < func_args.named_child_count(); i++) {
        auto param = func_args.named_child(i);
        assert(param.sym == sym_parameter_declaration);
        // auto param_name = param.get_field(field_declarator).text();
        // method->params.push_back(param_name);
        method->param_nodes.push_back(param);
      }

      all_methods.push_back(method);
    }
  }

  return err;
}

//------------------------------------------------------------------------------

#if 0

int MtMethod::categorize() {
  int changes = 0;

  if (in_init) return 0;


  // Methods that only call ticks and are only called by tocks could be either a tick or a tock.
  // Assume they are tocks.
  if (fields_written.empty() && !categorized() && !callers.empty() && !callees.empty()) {
    bool only_called_by_tocks = !callers.empty();
    bool only_calls_ticks_or_funcs = !callees.empty();
    for (auto caller : callers) {
      if (!caller->in_tock) only_called_by_tocks = false;
    }
    for (auto callee : callees) {
      if (!callee->in_tick && !callee->in_func) only_calls_ticks_or_funcs = false;
    }
    if (only_called_by_tocks && only_calls_ticks_or_funcs && !in_tock) {
      in_tock = true;
      changes++;
    }
  }

  /*
  if (fields_written.empty() && !in_tock) {
    LOG_B("%-20s is tock because it doesn't call or write anything.\n", cname());
    in_tock = true;
    changes++;
  }
  */

  /*
  // Methods that call a tock are tocks.
  for (auto& ref : callees) {
    if (ref.method->in_tock && !in_tock) {
      LOG_B("%-20s is tock because it calls tock %s.\n", cname(), ref.method->cname());
      in_tock = true;
      changes++;
    }
  }
  */

  /*
  // Methods that call a tick that has parameters are tocks.
  for (auto& ref : callees) {
    if (ref.method->in_tick && ref.method->params.size() && !in_tock) {
      LOG_B("%-20s is tock because it calls tick with params %s.\n", cname(), ref.method->cname());
      in_tock = true;
      changes++;
    }
  }
  */

  /*
  // Methods called by a tick are ticks.
  for (auto& ref : callers) {
    if (ref.method->in_tick && !in_tick) {
      LOG_B("%-20s is tick because it calls tick %s.\n", cname(), ref.method->cname());
      in_tick = true;
      changes++;
    }
  }
  */

  /*
  // Methods that write a signal are tocks.
  for (auto ref : fields_written) {
    auto f = ref.subfield ? ref.subfield : ref.field;
    if (f->state == FIELD_SIGNAL && !in_tock) {
      LOG_B("%-20s is tock because it writes signal %s.\n", cname(), f->cname());
      in_tock = true;
      changes++;
    }
  }

  // Methods that write a register are ticks.
  for (auto ref : fields_written) {
    auto f = ref.subfield ? ref.subfield : ref.field;
    if (f->state == FIELD_REGISTER && !in_tick) {
      in_tick = true;
      changes++;
    }
  }
  */

  /*
  // Methods that only write outputs are tocks.
  {
    bool only_writes_outputs = !fields_written.empty();
    for (auto ref : fields_written) {
      if (ref.subfield) {
        if (ref.subfield->state != FIELD_OUTPUT) only_writes_outputs = false;
      }
      else if (ref.field) {
        if (ref.field->state != FIELD_OUTPUT) only_writes_outputs = false;
      }
    }
    if (only_writes_outputs && !in_tock) {
      in_tock = true;
      changes++;
    }
  }
  */



  return changes;
}

#endif

//------------------------------------------------------------------------------

CHECK_RETURN Err MtModule::build_call_graph() {
  Err err;

  for (auto src_method : all_methods) {
    auto src_mod = this;

    src_method->_node.visit_tree([&](MnNode child) {
      if (child.sym == sym_call_expression) {
        auto func = child.get_field(field_function);
        if (func.sym == sym_identifier) {
          auto dst_mod = this;
          auto dst_method = get_method(func.text());
          if (dst_method) {
            dst_method->callers.push_back(src_method);
            src_method->callees.push_back(dst_method);
          }
        }

        if (func.sym == sym_field_expression) {
          auto component_name = func.get_field(field_argument).text();
          auto component_method_name = func.get_field(field_field).text();

          auto component = get_field(component_name);
          if (component) {
            auto dst_mod = source_file->lib->get_module(component->type_name());
            if (dst_mod) {
              auto dst_method = dst_mod->get_method(component_method_name);
              if (dst_method) {
                dst_method->callers.push_back(src_method);
                src_method->callees.push_back(dst_method);
              }
            }
          }
        }
      }
    });
  }

  if (constructor) constructor->in_init = true;

  return err;
}

//------------------------------------------------------------------------------

std::vector<std::string> split_field_path(const std::string &path) {
  std::vector<std::string> result;
  std::string temp = "";
  for (auto i = 0; i < path.size(); i++) {
    if (path[i] == '.') {
      result.push_back(temp);
      temp.clear();
    } else {
      temp.push_back(path[i]);
    }
  }

  if (temp.size()) result.push_back(temp);

  return result;
}

//------------------------------------------------------------------------------

CHECK_RETURN Err MtModule::trace() {
  Err err;
  LOG_G("Tracing all public tocks in %s\n", cname());

  mod_state.clear();

  for (auto m : all_methods) {
    LOG_INDENT_SCOPE();

    if (m == constructor) continue;
    if (m->callers.size()) continue;

    LOG_G("Tracing %s.%s\n", cname(), m->cname());
    LOG_INDENT_SCOPE();

    MtTracer tracer(source_file->lib);
    tracer._component_stack.push_back(nullptr);
    tracer._mod_stack.push_back(this);
    tracer.push_state(&mod_state);

    err << tracer.trace_dispatch(m->_node);
  }

  if (err.has_err()) {
    err << ERR("Method trace failed.");
    return err;
  }

  // Check that all entries in the state map ended up in a valid state.

  for (auto &pair : mod_state) {
    if (pair.second == FIELD_INVALID) {
      err << ERR("Field %s has invalid state\n", pair.first.c_str(),
                 to_string(pair.second));
    }
  }

  // Assign the final merged states back from the map to the fields.

  for (auto &pair : mod_state) {
    auto path = pair.first;
    auto split = split_field_path(path);

    assert(split[0] == "<top>");
    split.erase(split.begin());

    auto tail_field = split.back();
    split.pop_back();

    MtModule *mod_cursor = this;
    for (int i = 0; i < split.size(); i++) {
      auto component = mod_cursor->get_field(split[i]);
      mod_cursor =
          mod_cursor->source_file->lib->get_module(component->type_name());
    }

    MtField *field = mod_cursor->get_field(tail_field);
    assert(field);
    field->state = pair.second;
  }

  return err;
}

//------------------------------------------------------------------------------
// Collect all inputs to all tock and getter methods and merge them into a list
// of input ports.

CHECK_RETURN Err MtModule::sort_fields() {
  Err err;

#if 0
  LOG_G("Categorizing %s\n", name().c_str());

  for (auto f : all_fields) {

    if (f->is_param()) {
      localparams.push_back(MtParam::construct("<localparam>", f->node));
    }
    else if (f->is_public()) {
      switch(f->state) {
      case FIELD_NONE     : assert(false); break;
      case FIELD_INPUT    : input_signals.push_back(f); break;
      case FIELD_OUTPUT   : output_signals.push_back(f); break;
      case FIELD_SIGNAL   : output_signals.push_back(f); break;
      case FIELD_REGISTER : public_registers.push_back(f); break;
      case FIELD_INVALID  : assert(false); break;
      default: assert(false);
      }
    }
    else {
      switch(f->state) {
      case FIELD_NONE     : {
        err << ERR("Field %s was not touched during trace\n", f->name().c_str());
        break;
      }
      case FIELD_INPUT    : private_registers.push_back(f); break; // read-only thing like mem initialized in init()
      case FIELD_OUTPUT   : private_signals.push_back(f); break;
      case FIELD_SIGNAL   : private_signals.push_back(f); break;
      case FIELD_REGISTER : private_registers.push_back(f); break;
      case FIELD_INVALID  : assert(false); break;
      default: assert(false);
      }
    }
  }

  for (auto m : all_methods) {
    if (!m->is_public) continue;
    if (!m->is_tock) continue;

    auto params =
      m->node.get_field(field_declarator).get_field(field_parameters);

    for (const auto& param : params) {
      if (param.sym != sym_parameter_declaration) continue;
      MtParam* new_input = MtParam::construct(m->name(), param);
      input_arguments.push_back(new_input);
    }

    if (m->is_tock && m->has_return()) {
      output_returns.push_back(m);
    }
  }
#endif

  return err;
}

//------------------------------------------------------------------------------
// Go through all calls in the tree and build a {call_param -> arg} map.
// FIXME we aren't actually using this now?

#if 0
CHECK_RETURN Err MtModule::build_port_map() {
  assert(port_map.empty());

  Err err;

  //----------------------------------------
  // Assignments to component fields bind ports.

  mod_class.visit_tree([&](MnNode child) {
    if (child.sym != sym_assignment_expression) return;

    auto node_lhs = child.get_field(field_left);
    auto node_rhs = child.get_field(field_right);

    if (node_lhs.sym == sym_field_expression) {
      auto component_name = node_lhs.get_field(field_argument).text();
      if (get_component(component_name)) {
        port_map[node_lhs.text()] = node_rhs.text();
      }
    }
  });

  //----------------------------------------
  // Calls to methods bind ports.

  mod_class.visit_tree([&](MnNode child) {
    if (child.sym != sym_call_expression) return;
    if (child.get_field(field_function).sym != sym_field_expression) return;

    auto node_call = child;
    auto node_func = node_call.get_field(field_function);
    auto node_args = node_call.get_field(field_arguments);

    MtField *call_member = nullptr;
    MtModule *call_component = nullptr;
    MtMethod *call_method = nullptr;

    if (node_func.sym == sym_field_expression) {
      auto node_member = node_func.get_field(field_argument);
      auto node_method = node_func.get_field(field_field);
      if (node_method.text() == "as_signed") {
        return;
      }
    }

    if (node_func.sym == sym_field_expression) {
      auto node_this = node_func.get_field(field_argument);
      auto node_method = node_func.get_field(field_field);

      if (node_method.text() == "as_signed") {
      } else {
        call_member = get_component(node_this.text());
        if (call_member) {
          call_component = source_file->lib->get_module(call_member->type_name());
          call_method = call_component->get_method(node_method.text());
        } else {
          err << ERR("Component %s not found!\n", node_this.text().c_str());
          return;
        }
      }
    }

    auto arg_count = node_args.named_child_count();

    for (auto i = 0; i < arg_count; i++) {
      auto key = call_member->name() + "." + call_method->params[i];

      std::string val;
      MtCursor cursor(source_file->lib, source_file, this, &val);
      auto arg_node = node_args.named_child(i);
      cursor.cursor = arg_node.start();
      err << cursor.emit_dispatch(arg_node);

      auto it = port_map.find(key);
      if (it != port_map.end()) {
        if ((*it).second != val) {
        }
      } else {
        port_map.insert({key, val});
      }
    }
  });

  //----------------------------------------
  // Port mapping walk done.

  // Verify that all input ports of all components are bound.

  for (const auto& component : all_components) {
    auto component_mod = source_file->lib->get_module(component->type_name());

    for (int i = 0; i < component_mod->input_signals.size(); i++) {
      auto key = component->name() + "." + component_mod->input_signals[i]->name();

      if (!port_map.contains(key)) {
        err << ERR("No input bound to component port %s\n", key.c_str());
      }
    }

    for (int i = 0; i < component_mod->input_arguments.size(); i++) {
      auto key = component->name() + "." + component_mod->input_arguments[i]->name();

      if (!port_map.contains(key)) {
        err << ERR("No input bound to component port %s\n", key.c_str());
      }
    }
  }
    
  return err;
}
#endif

//------------------------------------------------------------------------------
