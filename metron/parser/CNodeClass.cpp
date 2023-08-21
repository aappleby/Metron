#include "CNodeClass.hpp"

#include "NodeTypes.hpp"
#include "CNodeFunction.hpp"
#include "CNodeField.hpp"
#include "metrolib/core/Log.h"
#include "CNodeClass.hpp"
#include "CNodeStruct.hpp"

//------------------------------------------------------------------------------

void CNodeClass::init(const char* match_tag, SpanType span, uint64_t flags) {
  CNode::init(match_tag, span, flags);
}

uint32_t CNodeClass::debug_color() const {
  return 0x00FF00;
}

std::string_view CNodeClass::get_name() const {
  return child("name")->get_name();
}

CNodeField* CNodeClass::get_field(std::string_view name) {
  for (auto f : all_fields) if (f->get_name() == name) return f;
  return nullptr;
}

CNodeFunction* CNodeClass::get_method(std::string_view name) {
  for (auto m : all_functions) if (m->get_name() == name) return m;
  return nullptr;
}

CNodeDeclaration* CNodeClass::get_modparam(std::string_view name) {
  for (auto p : all_modparams) if (p->get_name() == name) return p;
  return nullptr;
}

//------------------------------------------------------------------------------

Err CNodeClass::collect_fields_and_methods(CSourceRepo* repo) {
  Err err;

  auto body = child("body");

  bool is_public = false;

  for (auto c : body) {
    if (auto access = c->as_a<CNodeAccess>()) {
      is_public = c->get_text() == "public:";
    }

    if (auto n = c->as_a<CNodeField>()) {
      n->_static = n->child("static") != nullptr;
      n->_const  = n->child("const")  != nullptr;
      n->_public = is_public;

      n->_parent_class  = n->ancestor<CNodeClass>();
      n->_parent_struct = n->ancestor<CNodeStruct>();
      n->_type_class    = repo->get_class(n->get_type_name());
      n->_type_struct   = repo->get_struct(n->get_type_name());


      all_fields.push_back(n);
    }
    if (auto n = c->as_a<CNodeFunction>()) {
      n->is_public_ = is_public;
      all_functions.push_back(n);

      // Hook up _type_struct on all struct params
      auto params = n->child("params");
      for (auto p : params) {
        p->dump_tree();
        auto decl = p->is_a<CNodeDeclaration>();
        decl->_type_class = repo->get_class(decl->get_type_name());
        decl->_type_struct = repo->get_struct(decl->get_type_name());
        LOG_G("%p %p\n", decl->_type_class, decl->_type_struct);
      }


    }
  }

  if (auto parent = node_parent->as_a<CNodeTemplate>()) {
    //LOG_B("CNodeClass has template parent\n");
    CNode* params = parent->child("template_params");
    for (CNode*  param : params) {
      if (param->as_a<CNodeDeclaration>()) {
        //param->dump_tree(3);
        all_modparams.push_back(param->as_a<CNodeDeclaration>());
      }
    }
  }

  return err;
}

//------------------------------------------------------------------------------

Err CNodeClass::build_call_graph(CSourceRepo* repo) {
  Err err;

  for (auto src_method : all_functions) {
    visit(src_method, [&](CNode* child) {
      auto call = child->as_a<CNodeCall>();
      if (!call) return;

      auto dst_method = repo->resolve(this, call->child("func_name"))->as_a<CNodeFunction>();
      assert(dst_method);

      if (dst_method->get_parent_class() == this) {
        src_method->internal_callees.insert(dst_method);
        dst_method->internal_callers.insert(src_method);
      }
      else {
        src_method->external_callees.insert(dst_method);
        dst_method->external_callers.insert(src_method);
      }

      //
      //src_method->called_by.insert(src_method);
    });
  }

  /*
  for (auto src_method : all_methods) {
    auto src_mod = this;

    src_method->_node.visit_tree([&](MnNode child) {
      if (child.sym != sym_call_expression) return;

      auto func = child.get_field(field_function);

      if (func.sym == sym_identifier) {
        auto dst_mod = this;
        auto dst_method = get_method(func.text());
        if (dst_method) {
          dst_method->internal_callers.insert(src_method);
          src_method->internal_callees.insert(dst_method);
        }
      }

      if (func.sym == sym_field_expression) {
        auto component_name = func.get_field(field_argument);
        auto component_method_name = func.get_field(field_field).text();

        auto component = get_field(component_name.name4());
        if (component) {
          auto dst_mod = source_file->lib->get_module(component->type_name());
          if (dst_mod) {
            auto dst_method = dst_mod->get_method(component_method_name);
            if (dst_method) {
              dst_method->external_callers.insert(src_method);
              src_method->external_callees.insert(dst_method);
            }
          }
        }
      }
    });
  }
  */

  return err;
}


//------------------------------------------------------------------------------

Err CNodeClass::categorize_fields(bool verbose) {
  Err err;

  if (verbose) {
    auto name = get_name();
    LOG_G("Categorizing %.*s\n", int(name.size()), name.data());
  }

#if 0
  for (auto f : all_fields) {
    if (f->is_param()) {
      continue;
    }

    if (f->is_component()) {
      components.push_back(f);
    }
    else if (f->is_public() && f->is_input()) {
      input_signals.push_back(f);
    }
    else if (f->is_public() && f->is_signal()) {
      output_signals.push_back(f);
    }
    else if (f->is_public() && f->is_register()) {
      output_registers.push_back(f);
    }
    else if (f->is_private() && f->is_register()) {
      private_registers.push_back(f);
    }
    else if (f->is_private() && f->is_signal()) {
      private_signals.push_back(f);
    }
    else if (!f->is_public() && f->is_input()) {
      private_registers.push_back(f);
    }
    else if (f->is_enum()) {
    }
    else if (f->is_dead()) {
      dead_fields.push_back(f);
    }
    else {
      err << ERR("Don't know how to categorize %s = %s\n", f->cname(),
                 to_string(f->_state));
      f->error();
    }
  }
#endif

  return err;
}


//------------------------------------------------------------------------------

CNode* CNodeClass::resolve(CNode* name, CSourceRepo* repo) {
  if (!name) return nullptr;

  //----------

  if (auto field = name->as_a<CNodeFieldExpression>()) {
    return resolve(name->child_head, repo);
  }

  //----------

  if (auto qual = name->as_a<CNodeQualifiedIdentifier>()) {
    return resolve(name->child_head, repo);
  }

  //----------

  if (auto id = name->as_a<CNodeIdentifier>()) {
    if (name->tag_is("scope")) {
      assert(false);
      return nullptr;
    }

    if (name->tag_is("field")) {
      auto field = get_field(name->get_text());
      auto field_class = repo->get_class(field->get_type_name());
      return repo->resolve(field_class, name->node_next);
    }

    printf("### %s ###\n", name->match_tag);
    return get_method(name->get_name());
  }

  //----------

  assert(false && "Could not resolve function name");
  return nullptr;
}

//------------------------------------------------------------------------------

bool CNodeClass::needs_tick() {
  for (auto m : all_functions) {
    //if (m->is_tick()) return true;
  }

  /*
  for (auto f : all_fields) {
    if (f->needs_tick()) return true;
  }
  */

  return false;
}

bool CNodeClass::needs_tock() {
  /*
  for (auto m : all_methods) {
    if (m->is_tock()) return true;
    if (m->is_func() && m->internal_callers.empty() && m->is_public()) return true;
  }

  for (auto f : all_fields) {
    if (f->is_module() && f->needs_tock()) return true;
  }
  */

  return false;
}


//------------------------------------------------------------------------------

Err CNodeClass::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);

  auto n_class = child("class");
  auto n_name  = child("name");
  auto n_body  = child("body");

  err << cursor.emit_replacements(n_class->get_text(), "class", "module");
  err << cursor.emit_gap(n_class, n_name);
  err << cursor.emit(n_name);
  err << cursor.emit_gap(n_name, n_body);

  err << cursor.emit_print("(\n");
  //cursor.push_indent(node_body);
  err << cursor.emit_print("{{port list}}");
  //err << cursor.emit_module_ports(node_body);
  //cursor.pop_indent(node_body);
  err << cursor.emit_line(");");

  //err << cursor.emit(n_body);
  for (auto child : n_body) {
    if (child->get_text() == "{") {
      err << cursor.skip_over(child);
      err << cursor.emit_gap_after(child);
      err << cursor.emit_print("{{template parameter list}}\n");
    }
    else if (child->get_text() == "}") {
      err << cursor.emit_replacement(child, "endmodule");
      err << cursor.emit_gap_after(child);
    }
    else {
      err << cursor.emit(child);
      err << cursor.emit_gap_after(child);
    }
  }

  return err << cursor.check_done(this);
}

/*
CHECK_RETURN Err MtCursor::emit_module_ports(MnNode class_body) {
  Err err;

  if (current_mod.top()->needs_tick()) {
    err << emit_line("// global clock");
    err << emit_line("input logic clock,");
  }

  if (current_mod.top()->input_signals.size()) {
    err << emit_line("// input signals");
    for (auto f : current_mod.top()->input_signals) {
      err << emit_field_port(f);
    }
  }

  if (current_mod.top()->output_signals.size()) {
    err << emit_line("// output signals");
    for (auto f : current_mod.top()->output_signals) {
      err << emit_field_port(f);
    }
  }

  if (current_mod.top()->output_registers.size()) {
    err << emit_line("// output registers");
    for (auto f : current_mod.top()->output_registers) {
      err << emit_field_port(f);
    }
  }

  for (auto m : current_mod.top()->all_methods) {
    if (!m->is_init_ && m->is_public() && m->internal_callers.empty()) {
      err << emit_method_ports(m);
    }
  }

  // Remove trailing comma from port list
  if (at_comma) {
    err << emit_backspace();
  }

  return err;
}
*/


//------------------------------------------------------------------------------

/*
Err CNodeFi eldList::emit(Cursor& cursor) {
  Err err = cursor.check_at(this);

  for (auto c : (CNode*)this) {
    if (c->get_text() == "{") {
      err << cursor.skip_over(c);
      err << cursor.emit_gap_after(c);
      err << cursor.emit_print("{{template parameter list}}\n");
    }
    else if (c->get_text() == "}") {
      err << cursor.emit_replacement(c, "endmodule");
      err << cursor.emit_gap_after(c);
    }
    else {
      err << cursor.emit(c);
      err << cursor.emit_gap_after(c);
    }

  }

  return err << cursor.check_done(this);
}
*/


//------------------------------------------------------------------------------

void CNodeClass::dump() {
  auto name = get_name();
  LOG_B("Class %.*s @ %p\n", name.size(), name.data(), this);
  LOG_INDENT();

  if (refcount) {
    LOG_G("Refcount %d\n", refcount);
  }
  else {
    LOG_G("Top module\n");
  }

  if (all_fields.size()) {
    for (auto f : all_fields) f->dump();
  }

  if (all_functions.size()) {
    for (auto m : all_functions) m->dump();
  }

  LOG_DEDENT();
}