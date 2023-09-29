#pragma once

#include "metron/nodes/CNodeStatement.hpp"

//==============================================================================

struct CNodeSwitch : public CNodeStatement {
  void init(const char* match_tag, SpanType span, uint64_t flags);
  CHECK_RETURN Err trace(CInstance* inst, call_stack& stack) override;
  CHECK_RETURN Err emit(Cursor& cursor) override;

  CNodeList* node_condition = nullptr;
  CNodeList* node_body = nullptr;
};

//------------------------------------------------------------------------------

struct CNodeCase : public CNodeStatement {
  void init(const char* match_tag, SpanType span, uint64_t flags);

  CHECK_RETURN Err trace(CInstance* inst, call_stack& stack) override;
  CHECK_RETURN Err emit(Cursor& cursor) override;

  CNodeKeyword* node_case  = nullptr;
  CNode*        node_cond  = nullptr;
  CNodePunct*   node_colon = nullptr;
  CNodeList*    node_body  = nullptr;
};

//------------------------------------------------------------------------------

struct CNodeDefault : public CNodeStatement {
  void init(const char* match_tag, SpanType span, uint64_t flags);

  CHECK_RETURN Err trace(CInstance* inst, call_stack& stack) override;
  CHECK_RETURN Err emit(Cursor& cursor) override;

  CNodeKeyword* node_default = nullptr;
  CNodePunct*   node_colon   = nullptr;
  CNodeList*    node_body    = nullptr;
};

//==============================================================================
