#include "metron/tools/MtUtils.h"

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

//------------------------------------------------------------------------------

FieldType trace_state_to_field_type(TraceState s) {
  switch(s) {
    case TS_NONE:     return FT_SIGNAL;
    case TS_INPUT:    return FT_INPUT;
    case TS_OUTPUT:   return FT_OUTPUT;
    case TS_MAYBE:    return FT_REGISTER;
    case TS_SIGNAL:   return FT_SIGNAL;
    case TS_REGISTER: return FT_REGISTER;
    case TS_INVALID:  return FT_INVALID;
    case TS_PENDING:  return FT_INVALID;
  }
  return FT_INVALID;
}

//------------------------------------------------------------------------------

TraceState merge_action(TraceState state, TraceAction action) {
  // clang-format off

  TraceState result = TS_INVALID;

  if (action == ACT_READ) {
    switch (state) {
      case TS_NONE:     result = TS_INPUT;   break;
      case TS_INPUT:    result = TS_INPUT;   break;
      case TS_OUTPUT:   result = TS_SIGNAL;  break;
      case TS_MAYBE:    result = TS_INVALID; break;
      case TS_SIGNAL:   result = TS_SIGNAL;  break;
      case TS_REGISTER: result = TS_INVALID; break;
      case TS_INVALID:  result = TS_INVALID; break;
      case TS_PENDING:  result = TS_INVALID; break;
    }
  }

  if (action == ACT_WRITE) {
    switch (state) {
      case TS_NONE:     result = TS_OUTPUT;   break;
      case TS_INPUT:    result = TS_REGISTER; break;
      case TS_OUTPUT:   result = TS_OUTPUT;   break;
      case TS_MAYBE:    result = TS_OUTPUT;   break;
      case TS_SIGNAL:   result = TS_INVALID;  break;
      case TS_REGISTER: result = TS_REGISTER; break;
      case TS_INVALID:  result = TS_INVALID;  break;
      case TS_PENDING:  result = TS_INVALID; break;
    }
  }
  // clang-format on

  if (result == TS_INVALID) {
    //printf("invalid!\n");
      int x = 1;
      x++;
  }

  return result;
}

const char* merge_action_message(TraceState state, TraceAction action) {
  // clang-format off

  TraceState result = TS_INVALID;

  if (action == ACT_READ) {
    switch (state) {
      case TS_NONE:     return "Field was untouched, is now an input";
      case TS_INPUT:    return "Reading from an input is OK";
      case TS_OUTPUT:   return "Reading from an output makes it a signal";
      case TS_MAYBE:    return "Can't read from a maybe";
      case TS_SIGNAL:   return "Reading a signal is OK";
      case TS_REGISTER: return "Can't read from a register after it has been written";
      case TS_INVALID:  return "Can't read from Invalid";
      case TS_PENDING:  return "Can't read from Pending";
    }
  }

  if (action == ACT_WRITE) {
    switch (state) {
      case TS_NONE:     return "Field was untouched, is now an output";
      case TS_INPUT:    return "Writing to an input makes it a register";
      case TS_OUTPUT:   return "Overwriting an output is OK";
      case TS_MAYBE:    return "Overwriting a maybe is OK";
      case TS_SIGNAL:   return "Can't write a signal after it has been read";
      case TS_REGISTER: return "Overwriting a register is OK";
      case TS_INVALID:  return "Cant write to Invalid";
      case TS_PENDING:  return "Cant write to Pending";
    }
  }

  if (action == ACT_PUSH) {
    return "Push a copy of state stack top";
  }

  if (action == ACT_POP) {
    return "Pop state stack top";
  }

  if (action == ACT_SWAP) {
    return "Swap state stack top and next";
  }

  if (action == ACT_MERGE) {
    return "Merge branches";
  }

  // clang-format on

  return "### Fill in action messages ###";
}

//-----------------------------------------------------------------------------

TraceState merge_branch(TraceState ma, TraceState mb) {
  if (ma == TS_PENDING) {
    return mb;
  } else if (mb == TS_PENDING) {
    return ma;
  } else if (ma == mb) {
    return ma;
  } else if (ma == TS_INVALID || mb == TS_INVALID) {
    return TS_INVALID;
  } else {
    // clang-format off
    TraceState table[6][6] = {
      {TS_NONE,     TS_INPUT,    TS_MAYBE,    TS_MAYBE,    TS_INVALID, TS_REGISTER},
      {TS_INPUT,    TS_INPUT,    TS_REGISTER, TS_REGISTER, TS_INVALID, TS_REGISTER},
      {TS_MAYBE,    TS_REGISTER, TS_OUTPUT,   TS_MAYBE,    TS_SIGNAL,  TS_REGISTER},
      {TS_MAYBE,    TS_REGISTER, TS_MAYBE,    TS_MAYBE,    TS_INVALID, TS_REGISTER},
      {TS_INVALID,  TS_INVALID,  TS_SIGNAL,   TS_INVALID,  TS_SIGNAL,  TS_INVALID },
      {TS_REGISTER, TS_REGISTER, TS_REGISTER, TS_REGISTER, TS_INVALID, TS_REGISTER},
    };
    // clang-format on

    assert(table[ma][mb] == table[mb][ma]);

    auto result = table[ma][mb];

    if (result  == TS_INVALID) {
      //printf("invalid!\n");
      int x = 1;
      x++;
    }

    return result;
  }
}

const char* merge_branch_message(TraceState ma, TraceState mb) {
  if (ma == TS_INVALID || mb == TS_INVALID) {
    return "Merging invalid branches is an error";
  } else if (ma == TS_PENDING || mb == TS_PENDING) {
    return "Can't merge a Pending";
  } else if (ma == mb) {
    return "Merging identical branches is OK";
  } else {
    // clang-format off
    const char* table[6][6] = {
      // CTX_NONE
      {
        "Nothing happened in either branch", /* CTX_NONE + CTX_NONE = CTX_NONE */
        "Read happened in one branch, this is now an input", /* CTX_NONE + CTX_INPUT = CTX_INPUT */
        "Only one branch wrote the field - this could be an output, could be a signal",/* CTX_NONE + CTX_OUTPUT = CTX_MAYBE */
        "Still don't know if this is an output or a signal",/* CTX_NONE + CTX_MAYBE = CTX_MAYBE */
        "Field was untouched in one branch and written and then read in the other branch - this is not valid",/* CTX_NONE + CTX_SIGNAL = CTX_INVALID */
        "One branch read and then wrote the field",/* CTX_NONE + CTX_REGISTER = CTX_REGISTER */
      },
      {
        "One branch read the field, this is an input",/* CTX_INPUT + CTX_NONE = CTX_INPUT */
        "Merging identical branches is OK",/* CTX_INPUT + CTX_INPUT = CTX_INPUT */
        "Read in one branch, write in the other - must be a register",/* CTX_INPUT + CTX_OUTPUT = CTX_REGISTER */
        "Read in one branch, maybe write in the other - must be a register",/* CTX_INPUT + CTX_MAYBE = CTX_REGISTER */
        "Read in one branch, write-read in the other - invalid",/* CTX_INPUT + CTX_SIGNAL = CTX_INVALID */
        "Read in one branch, read-write in the other - must be a register",/* CTX_INPUT + CTX_REGISTER = CTX_REGISTER */
      },
      {
        "Only one branch wrote the field - this could be an output, could be a signal",/* CTX_OUTPUT + CTX_NONE = CTX_MAYBE */
        "Read in one branch, write in the other - must be a register",/* CTX_OUTPUT + CTX_INPUT = CTX_REGISTER */
        "Merging identical branches is OK",/* CTX_OUTPUT + CTX_OUTPUT = CTX_OUTPUT */
        "Not all branches write - must be a maybe",/* CTX_OUTPUT + CTX_MAYBE = CTX_MAYBE */
        "Still a signal",/* CTX_OUTPUT + CTX_SIGNAL = CTX_SIGNAL */
        "Still a register",/* CTX_OUTPUT + CTX_REGISTER = CTX_REGISTER */
      },
      {
        "Still don't know if this is an output or a signal",/* CTX_MAYBE + CTX_NONE = CTX_MAYBE */
        "Read in one branch, maybe write in the other - must be a register",/* CTX_MAYBE + CTX_INPUT = CTX_REGISTER */
        "Not all branches write - must be a maybe",/* CTX_MAYBE + CTX_OUTPUT = CTX_MAYBE */
        "Merging identical branches is OK",/* CTX_MAYBE + CTX_MAYBE = CTX_MAYBE */
        "A signal that only might be written in the other branch is invalid",/* CTX_MAYBE + CTX_SIGNAL = CTX_INVALID */
        "Still a register",/* CTX_MAYBE + CTX_REGISTER = CTX_REGISTER */
      },
      {
        "WR in one branch, none in the other - invalid",/* CTX_SIGNAL + CTX_NONE = CTX_INVALID */
        "WR in one branch, R in the other - invalid",/* CTX_SIGNAL + CTX_INPUT = CTX_INVALID */
        "Still a signal",/* CTX_SIGNAL + CTX_OUTPUT = CTX_SIGNAL */
        "A signal that only might be written in the other branch is invalid",/* CTX_SIGNAL + CTX_MAYBE = CTX_INVALID */
        "Merging identical branches is OK",/* CTX_SIGNAL + CTX_SIGNAL = CTX_SIGNAL */
        "Signal in one branch, register in the other - invalid",/* CTX_SIGNAL + CTX_REGISTER = CTX_INVALID */
      },
      {
        "Still a register",/* CTX_REGISTER + CTX_NONE = CTX_REGISTER */
        "Still a register",/* CTX_REGISTER + CTX_INPUT = CTX_REGISTER */
        "Still a register",/* CTX_REGISTER + CTX_OUTPUT = CTX_REGISTER */
        "Still a register",/* CTX_REGISTER + CTX_MAYBE = CTX_REGISTER */
        "Register in one branch, signal in the other - invalid",/* CTX_REGISTER + CTX_SIGNAL = CTX_INVALID */
        "Merging identical branches is OK",/* CTX_REGISTER + CTX_REGISTER = CTX_REGISTER */
      },
    };
    // clang-format on

    return table[ma][mb];
  }
}

//------------------------------------------------------------------------------

// KCOV_OFF
std::string str_printf(const char* fmt, ...) {
  va_list args;

  va_start(args, fmt);
  int size = vsnprintf(nullptr, 0, fmt, args);
  va_end(args);

  std::string result;
  result.resize(size + 1);
  va_start(args, fmt);
  vsnprintf(result.data(), size_t(size + 1), fmt, args);
  va_end(args);
  assert(result.back() == 0);
  return result;
}
// KCOV_ON

//------------------------------------------------------------------------------
