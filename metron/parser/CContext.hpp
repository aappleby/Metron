// SPDX-FileCopyrightText:  2023 Austin Appleby <aappleby@gmail.com>
// SPDX-License-Identifier: MIT License

#pragma once

#include "matcheroni/Matcheroni.hpp"
#include "matcheroni/Parseroni.hpp"
#include "matcheroni/Utilities.hpp"

#include "CConstants.hpp"
#include "CToken.hpp"
#include "CLexer.hpp"
#include "CNode.hpp"
#include "CScope.hpp"
#include "SST.hpp"

#include <array>
#include <stdio.h>
#include <string>
#include <vector>
#include <set>
#include <filesystem>

struct CToken;
struct CNode;
struct CContext;
struct CScope;

using TokenSpan = matcheroni::Span<CToken>;

//------------------------------------------------------------------------------

class CContext : public parseroni::NodeContext<CNode> {
 public:

  std::vector<std::string> search_paths = {""};
  std::set<std::string> visited_files;

  std::string find_file(const std::string& file_path) {
    bool found = false;
    for (auto &path : search_paths) {
      auto full_path = path + file_path;
      if (!std::filesystem::is_regular_file(full_path)) continue;
      return full_path;
    }
    return "";
  }

  using AtomType = CToken;
  using SpanType = matcheroni::Span<CToken>;
  using NodeType = CNode;

  CContext();

  static int atom_cmp(char a, int b) {
    return (unsigned char)a - b;
  }

  static int atom_cmp(const CToken& a, const LexemeType& b) {
    return a.type - b;
  }

  static int atom_cmp(const CToken& a, const char& b) {
    if (auto d = a.text.len() - 1) return d;
    return a.text.begin[0] - b;
  }

  static int atom_cmp(const CToken& a, const matcheroni::TextSpan& b) {
    return strcmp_span(a.text, b);
  }

  void reset();
  TokenSpan parse(matcheroni::TextSpan text, TokenSpan lexemes);

  TokenSpan match_builtin_type_base  (TokenSpan body);
  TokenSpan match_builtin_type_prefix(TokenSpan body);
  TokenSpan match_builtin_type_suffix(TokenSpan body);

  TokenSpan match_class_name  (TokenSpan body);
  TokenSpan match_struct_name (TokenSpan body);
  TokenSpan match_union_name  (TokenSpan body);
  TokenSpan match_enum_name   (TokenSpan body);
  TokenSpan match_typedef_name(TokenSpan body);

  void add_class  (const CToken* a);
  void add_struct (const CToken* a);
  void add_union  (const CToken* a);
  void add_enum   (const CToken* a);
  void add_typedef(const CToken* a);

  void push_scope();
  void pop_scope();

  void append_node(CNode* node);
  void enclose_nodes(CNode* start, CNode* node);

  //----------------------------------------

  matcheroni::TextSpan handle_include(matcheroni::TextSpan body);

  //----------------------------------------

  void debug_dump(std::string& out) {
    for (auto node = top_head; node; node = node->node_next) {
      node->debug_dump(out);
    }
  }

  //----------------------------------------

  matcheroni::TextSpan text_span;
  TokenSpan  lexemes;

  std::vector<CToken> tokens;
  CScope* type_scope;
};

//------------------------------------------------------------------------------
