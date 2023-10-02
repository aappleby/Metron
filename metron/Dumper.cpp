#include "Dumper.hpp"

#include <string>
#include <vector>

#include "metrolib/core/Log.h"
#include "metron/CInstance.hpp"
#include "metron/CNode.hpp"
#include "metron/nodes/CNodeClass.hpp"
#include "metron/nodes/CNodeField.hpp"
#include "metron/nodes/CNodeFunction.hpp"
#include "metron/nodes/CNodeConstructor.hpp"

//------------------------------------------------------------------------------
// Node debugging

static std::string escape(const char* a, const char* b) {
  std::string result;
  result.push_back('"');
  for (; a < b; a++) {
    switch (*a) {
      case '\n':
        result.append("\\n");
        break;
      case '\r':
        result.append("\\r");
        break;
      case '\t':
        result.append("\\t");
        break;
      case '"':
        result.append("\\\"");
        break;
      case '\\':
        result.append("\\\\");
        break;
      default:
        result.push_back(*a);
        break;
    }
  }
  result.push_back('"');
  return result;
}

//------------------------------------------------------------------------------

template <typename T>
const char* class_name(const T& t) {
  const char* name = typeid(t).name();
  while (*name && isdigit(*name)) name++;
  return name;
}

//------------------------------------------------------------------------------

struct NodeDumper {
  //----------------------------------------

  void dump_tree_recurse(const CNode& n, int depth, int max_depth) {
    color_stack.push_back(n.color);
    dump_node(n, depth);
    if (!max_depth || depth < max_depth) {
      for (auto child = n.child_head; child; child = child->node_next) {
        dump_tree_recurse(*child, depth + 1, max_depth);
      }
    }
    color_stack.pop_back();
  }

  //----------------------------------------

  void dump_node(const CNode& n, int depth) {
    LOG(" ");

    /*
    const char* trellis_1 = "   ";
    const char* trellis_2 = "|  ";
    const char* trellis_3 = "|--";
    const char* trellis_4 = "\\--";
    const char* dot_1     = "O ";
    const char* dot_2     = "O ";
    */

    const char* trellis_1 = "    ";
    const char* trellis_2 = "┃   ";
    const char* trellis_3 = "┣━━╸";
    const char* trellis_4 = "┗━━╸";
    const char* dot_1 = "▆ ";
    const char* dot_2 = "▆ ";

    for (int i = 0; i < color_stack.size() - 1; i++) {
      bool stack_top = i == color_stack.size() - 2;
      uint32_t color = color_stack[i];
      if (color == -1)
        LOG_C(0x000000, trellis_1);
      else if (!stack_top)
        LOG_C(color, trellis_2);
      else if (n.node_next)
        LOG_C(color, trellis_3);
      else
        LOG_C(color, trellis_4);
    }

    auto color = color_stack.back();

    if (n.child_head != nullptr) {
      LOG_C(color, dot_1);
    } else {
      LOG_C(color, dot_2);
    }

    if (n.match_tag) {
      LOG_C(color, "%s : ", n.match_tag);
    }
    LOG_C(color, "%s = ", class_name(n));

    if (n.child_head == nullptr) {
      std::string escaped = escape(n.text_begin(), n.text_end());
      LOG_C(color, "%s", escaped.c_str());
    }

    LOG_C(color, "\n");

    if (n.node_next == nullptr && color_stack.size() > 1) {
      color_stack[color_stack.size() - 2] = -1;
    }
  }

  //----------------------------------------

  std::string indent;
  std::vector<uint32_t> color_stack;
};

void dump_parse_tree(CNode* node) {
  NodeDumper d;
  d.dump_tree_recurse(*node, 0, 1000);
}

//------------------------------------------------------------------------------

void dump_inst_tree(CInstance* inst) {
  if (auto inst_class = inst->as<CInstClass>()) {
    LOG_G("Class %s\n", inst_class->path.c_str());

    {
      LOG_INDENT_SCOPE();
      LOG_G("Ports:\n");
      LOG_INDENT_SCOPE();
      for (auto child : inst_class->ports) dump_inst_tree(child);
    }
    {
      LOG_INDENT_SCOPE();
      LOG_R("Parts:\n");
      LOG_INDENT_SCOPE();
      for (auto child : inst_class->parts) dump_inst_tree(child);
    }
  }

  if (auto inst_struct = inst->as<CInstStruct>()) {
    LOG_G("Struct %s\n", inst_struct->path.c_str());

    {
      LOG_INDENT_SCOPE();
      LOG_G("Parts:\n");
      LOG_INDENT_SCOPE();
      for (auto child : inst_struct->parts) dump_inst_tree(child);
    }
  }

  if (auto inst_prim = inst->as<CInstPrim>()) {
    if (inst_prim->node_field && inst_prim->node_field->is_public) {
      LOG_G("Prim %s", inst_prim->path.c_str());
    } else {
      LOG_R("Prim %s", inst_prim->path.c_str());
    }

    for (auto state : inst_prim->state_stack) LOG(" %s", to_string(state));
    LOG("\n");
  }

  if (auto inst_func = inst->as<CInstFunc>()) {
    LOG_G("Func %s\n", inst_func->path.c_str());

    LOG_INDENT();

    if (inst_func->params.size()) {
      LOG_G("Params:\n");
      LOG_INDENT();
      for (auto p : inst_func->params) dump_inst_tree(p);
      LOG_DEDENT();
    }

    if (inst_func->inst_return) {
      LOG_G("Return:\n");
      LOG_INDENT();
      dump_inst_tree(inst_func->inst_return);
      LOG_DEDENT();
    }

    LOG_DEDENT();
  }
}

#if 0
//------------------------------------------------------------------------------

void CNode::dump() const {
  LOG_R("CNode::dump() : %s\n", typeid(*this).name());
}

void CNodePunct::dump() const override {
  auto text = get_text();
  LOG_B("CNodePunct \"%.*s\"\n", text.size(), text.data());
}

void CNodeNamespace::dump() const override {
  LOG_G("Fields:\n");
  LOG_INDENT_SCOPE();
  for (auto n : all_fields) n->dump();
}


void CNodeEnum::dump() const override {
  LOG_G("Enum %.*s\n", name.size(), name.data());
}


void CNodeDeclaration::dump() const override {
  auto text = get_text();
  LOG_G("Declaration `%.*s`\n", text.size(), text.data());
}

void CNodeAccess::dump() const override {
  auto text = get_text();
  LOG_B("CNodeAccess \"%.*s\"\n", text.size(), text.data());
}

//------------------------------------------------------------------------------

void CNode::dump_tree(int max_depth) const {
  NodeDumper d;
  d.dump_tree_recurse(*this, 0, max_depth);
}

//----------------------------------------------------------------------------

void CScope::dump() {

  LOG_G("class_types\n");
  for (auto s : class_types) {
    LOG("  ");
    LOG_SPAN(s);
    LOG("\n");
  }

  LOG_G("struct_types\n");
  for (auto s : struct_types) {
    LOG("  ");
    LOG_SPAN(s);
    LOG("\n");
  }

  LOG_G("union_types\n");
  for (auto s : union_types) {
    LOG("  ");
    LOG_SPAN(s);
    LOG("\n");
  }

  LOG_G("enum_types\n");
  for (auto s : enum_types) {
    LOG("  ");
    LOG_SPAN(s);
    LOG("\n");
  }

  LOG_G("typedef_types\n");
  for (auto s : typedef_types) {
    LOG("  ");
    LOG_SPAN(s);
    LOG("\n");
  }
}

//----------------------------------------------------------------------------

void Lexeme::dump_lexeme() const {
  const int span_len = 20;
  std::string dump = "";

  if (type == LEX_BOF) dump = "<bof>";
  if (type == LEX_EOF) dump = "<eof>";

  for (auto c = text_begin; c < text_end; c++) {
    if      (*c == '\n') dump += "\\n";
    else if (*c == '\t') dump += "\\t";
    else if (*c == '\r') dump += "\\r";
    else                 dump += *c;
    if (dump.size() >= span_len) break;
  }

  dump = '`' + dump + '`';
  if (dump.size() > span_len) {
    dump.resize(span_len - 4);
    dump = dump + "...`";
  }
  while (dump.size() < span_len) dump += " ";

  printf("r%04d c%02d i%02d ", row, col, indent);

  utils::set_color(type_to_color());
  printf("%-14.14s ", type_to_str());
  utils::set_color(0);
  printf("%s", dump.c_str());
  utils::set_color(0);
}

//------------------------------------------------------------------------------

void CSourceFile::dump() {
  auto source_span = matcheroni::utils::to_span(context.source);
  matcheroni::utils::print_trees(context, source_span, 50, 2);
}

//------------------------------------------------------------------------------

void CSourceRepo::dump() {
  {
    LOG_G("Search paths:\n");
    LOG_INDENT_SCOPE();
    for (auto s : search_paths) LOG_G("`%s`\n", s.c_str());
    LOG("\n");
  }

  {
    LOG_G("Source files:\n");
    LOG_INDENT_SCOPE();
    for (auto pair : source_map) LOG_L("`%s`\n", pair.first.c_str());
    LOG("\n");
  }

  {
    LOG_G("Classes:\n");
    LOG_INDENT_SCOPE();
    for (auto n : all_classes) {
      n->dump();
      LOG("\n");
    }
    if (all_classes.empty()) LOG("\n");
  }

  {
    LOG_G("Structs:\n");
    LOG_INDENT_SCOPE();
    for (auto n : all_structs) {
      n->dump();
      LOG("\n");
    }
    if (all_structs.empty()) LOG("\n");
  }

  {
    LOG_G("Namespaces:\n");
    LOG_INDENT_SCOPE();
    for (auto n : all_namespaces) {
      n->dump();
      LOG("\n");
    }
    if (all_namespaces.empty()) LOG("\n");
  }

  {
    LOG_G("Enums:\n");
    LOG_INDENT_SCOPE();
    for (auto n : all_enums) {
      n->dump();
      LOG("\n");
    }
    if (all_enums.empty()) LOG("\n");
  }
}

//------------------------------------------------------------------------------

void CNodeClass::dump() const {
  LOG_B("Class %s @ %s, refcount %d\n", name.c_str(), file->filename.c_str(), refcount);
  LOG_INDENT();
  {
    if (all_modparams.size()) {
      LOG_G("Modparams\n");
      LOG_INDENT_SCOPE();
      for (auto f : all_modparams) f->dump();
    }

    if (all_localparams.size()) {
      LOG_G("Localparams\n");
      LOG_INDENT_SCOPE();
      for (auto f : all_localparams) f->dump();
    }

    for (auto child : node_body) {
      child->dump();
    }
  }
  LOG_DEDENT();
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

void CNodeField::dump() const {
  LOG_A("Field %.*s : ", name.size(), name.data());

  if (node_decl->node_static) LOG_A("static ");
  if (node_decl->node_const)  LOG_A("const ");
  if (is_public)              LOG_A("public ");
  if (node_decl->is_array())  LOG_A("array ");

  if (parent_class) {
    LOG_A("parent class %s ", parent_class->get_namestr().c_str());
  }

  if (parent_struct) {
    LOG_A("parent struct %s ", parent_struct->get_namestr().c_str());
  }

  if (node_decl->_type_class) {
    LOG_A("type class %s ", node_decl->_type_class->get_namestr().c_str());
  }

  if (node_decl->_type_struct) {
    LOG_A("type struct %s ", node_decl->_type_struct->get_namestr().c_str());
  }

  LOG_A("\n");
}

//------------------------------------------------------------------------------

// FIXME constructor needs to be in internal_callers

void CNodeFunction::dump() const {
  LOG_S("Method \"%.*s\" ", name.size(), name.data());

  if (is_public)  LOG_G("public ");
  if (!is_public) LOG_G("private ");
  LOG_G("%s ", to_string(method_type));
  LOG_G("\n");

  if (params.size()) {
    LOG_INDENT_SCOPE();
    for (auto p : params) {
      auto param_name = p->child("name")->get_textstr();
      auto param_type = p->child("type")->get_textstr();
      LOG_G("Param %s : %s", param_name.c_str(), param_type.c_str());

      if (auto val = p->child("value")) {
        auto text = val->get_text();
        LOG_G(" = %.*s", text.size(), text.data());
      }

      LOG_G("\n");
    }
  }

  if (self_reads.size()) {
    LOG_INDENT_SCOPE();
    for (auto r : self_reads) {
      LOG_G("Directly reads  %s : %s\n", r->path.c_str(), to_string(r->get_state()));
    }
  }

  if (self_writes.size()) {
    LOG_INDENT_SCOPE();
    for (auto w : self_writes) {
      LOG_G("Directly writes %s : %s\n", w->path.c_str(), to_string(w->get_state()));
    }
  }

  if (all_reads.size()) {
    LOG_INDENT_SCOPE();
    for (auto r : all_reads) {
      if (self_reads.contains(r)) continue;
      LOG_G("Indirectly reads  %s : %s\n", r->path.c_str(), to_string(r->get_state()));
    }
  }

  if (all_writes.size()) {
    LOG_INDENT_SCOPE();
    for (auto w : all_writes) {
      if (self_writes.contains(w)) continue;
      LOG_G("Indirectly writes %s : %s\n", w->path.c_str(), to_string(w->get_state()));
    }
  }

  if (internal_callers.size()) {
    LOG_INDENT_SCOPE();
    for (auto c : internal_callers) {
      auto func_name = c->name;
      auto class_name = c->ancestor<CNodeClass>()->name;
      LOG_V("Called by %.*s::%.*s\n", class_name.size(), class_name.data(), func_name.size(), func_name.data());
    }
  }

  if (external_callers.size()) {
    LOG_INDENT_SCOPE();
    for (auto c : external_callers) {
      auto func_name = c->name;
      auto class_name = c->ancestor<CNodeClass>()->name;
      LOG_V("Called by %.*s::%.*s\n", class_name.size(), class_name.data(), func_name.size(), func_name.data());
    }
  }

  if (internal_callees.size()) {
    LOG_INDENT_SCOPE();
    for (auto c : internal_callees) {
      auto func_name = c->name;
      auto class_name = c->ancestor<CNodeClass>()->name;
      LOG_T("Calls %.*s::%.*s\n", class_name.size(), class_name.data(), func_name.size(), func_name.data());
    }
  }

  if (external_callees.size()) {
    LOG_INDENT_SCOPE();
    for (auto c : external_callees) {
      auto func_name = c->name;
      auto class_name = c->ancestor<CNodeClass>()->name;
      LOG_T("Calls %.*s::%.*s\n", class_name.size(), class_name.data(), func_name.size(), func_name.data());
    }
  }
}
#endif

//------------------------------------------------------------------------------

void dump_call_graph(CNodeClass* node_class) {
  LOG_G("Class %s\n", node_class->name.c_str());

  LOG_INDENT();

  if (node_class->constructor) {
    LOG("Constructor\n");
    LOG_INDENT();
    dump_call_graph(node_class->constructor);
    LOG_DEDENT();
  }

  for (auto node_func : node_class->all_functions) {
    if (node_func->internal_callers.size()) {
      continue;
    }
    else {
      LOG("Func %s\n", node_func->name.c_str());
      LOG_INDENT();
      dump_call_graph(node_func);
      LOG_DEDENT();
    }
  }
  LOG_DEDENT();
}


void dump_call_graph(CNodeFunction* node_func) {
  for (auto r : node_func->self_reads) {
    LOG("Reads %s\n", r->path.c_str());
  }
  for (auto w : node_func->self_writes) {
    LOG("Writes %s\n", w->path.c_str());
  }

  if (node_func->internal_callees.size()) {
    for (auto c : node_func->internal_callees) {
      auto func_name = c->name;
      auto class_name = c->ancestor<CNodeClass>()->name;
      LOG_T("%.*s::%.*s\n", class_name.size(), class_name.data(),
            func_name.size(), func_name.data());
      LOG_INDENT();
      dump_call_graph(c);
      LOG_DEDENT();
    }
  }

  if (node_func->external_callees.size()) {
    for (auto c : node_func->external_callees) {
      auto func_name = c->name;
      auto class_name = c->ancestor<CNodeClass>()->name;
      LOG_T("%.*s::%.*s\n", class_name.size(), class_name.data(),
            func_name.size(), func_name.data());
      LOG_INDENT();
      dump_call_graph(c);
      LOG_DEDENT();
    }
  }
}


//------------------------------------------------------------------------------

/*
void CNodeStruct::dump() const {
  LOG_B("Struct %.*s @ %p\n", name.size(), name.data(), this);
  LOG_INDENT();

  if (all_fields.size()) {
    for (auto f : all_fields) f->dump();
  }

  LOG_DEDENT();
}
*/