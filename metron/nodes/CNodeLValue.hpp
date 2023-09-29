#pragma once

#include "metron/CNode.hpp"

//==============================================================================

struct CNodeLValue : public CNode {
  CHECK_RETURN Err trace(CInstance* inst, call_stack& stack) override;
  CHECK_RETURN Err emit(Cursor& cursor) override;
};

//==============================================================================
