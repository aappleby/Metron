#pragma once

#include "metron/MtUtils.h"
#include "metrolib/core/Err.h"
#include "metrolib/core/Platform.h"
#include "metron/NodeTypes.hpp"
#include "metron/CInstance.hpp"


struct Tracer {

  Err trace(CNodeIdentifier* node) {
    auto scope = node->ancestor<CNodeCompound>();
    if (auto inst_field = top_inst->resolve(node)) {
      return inst_field->log_action(node, ACT_READ, stack);
    }
    return Err();
  }

  CInstance* top_inst;
  call_stack& stack;
};