// SPDX-FileCopyrightText:  2023 Austin Appleby <aappleby@gmail.com>
// SPDX-License-Identifier: MIT License

#include "CNode.hpp"

#include "Cursor.hpp"
#include "metrolib/core/Log.h"
#include <ctype.h>

using namespace matcheroni;

//------------------------------------------------------------------------------

uint32_t CNode::debug_color() const {
  return 0x222244;
}

std::string_view CNode::get_name() const {
  assert(false);
  return "<CNode>";
}

Err CNode::emit(Cursor& cursor) {
  return cursor.emit_replacement(this, "{{%s}}", typeid(*this).name());
}

//------------------------------------------------------------------------------

Err CNode::trace(IContext* context, TraceAction action) {
  LOG_R("Don't know how to trace {{%s}}\n", typeid(*this).name());
  exit(-1);
}

Err CNode::trace_children(IContext* context, TraceAction action) {
  Err err;
  for (auto c : this) err << c->trace(context, action);
  return err;
}

//------------------------------------------------------------------------------
// Node debugging

static std::string escape(const char* a, const char* b) {
  std::string result;
  result.push_back('"');
  for (; a < b; a++) {
    switch (*a) {
      case '\n': result.append("\\n");  break;
      case '\r': result.append("\\r");  break;
      case '\t': result.append("\\t");  break;
      case '"':  result.append("\\\""); break;
      case '\\': result.append("\\\\"); break;
      default:   result.push_back(*a);  break;
    }
  }
  result.push_back('"');
  return result;
}

//------------------------------------------------------------------------------

template<typename T>
const char* class_name(const T& t) {
  const char* name = typeid(t).name();
  while(*name && isdigit(*name)) name++;
  return name;
}

//------------------------------------------------------------------------------

struct NodeDumper {

  //----------------------------------------

  void dump_tree_recurse(const CNode& n, int depth, int max_depth) {
    color_stack.push_back(n.debug_color());
    dump_node(n, depth);
    if (!max_depth || depth < max_depth) {
      for (auto c = n.child_head; c; c = c->node_next) {
        dump_tree_recurse(*c, depth + 1, max_depth);
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
    const char* dot_1     = "▆ ";
    const char* dot_2     = "▆ ";

    for (int i = 0; i < color_stack.size() - 1; i++) {
      bool stack_top = i == color_stack.size() - 2;
      uint32_t color = color_stack[i];
      if      (color == -1)  LOG_C(0x000000, trellis_1);
      else if (!stack_top)   LOG_C(color,    trellis_2);
      else if (n.node_next)  LOG_C(color,    trellis_3);
      else                   LOG_C(color,    trellis_4);
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
      //color_stack.pop_back();
      //color_stack.push_back(-1);
    }
  }

  //----------------------------------------

  std::string indent;
  std::vector<uint32_t> color_stack;
};

//------------------------------------------------------------------------------

void CNode::dump_tree(int max_depth) const {
  //LOG("\n");
  //LOG("========== tree dump begin\n");
  NodeDumper d;
  d.dump_tree_recurse(*this, 0, max_depth);
  //LOG("========== tree dump end\n");
}

//------------------------------------------------------------------------------
