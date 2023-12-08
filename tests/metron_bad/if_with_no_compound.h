#include "metron/metron_tools.h"

// If statements whose sub-blocks contain submodule calls _must_ use {}.

// X If branches that contain component calls must use {}.

class Submod {
 public:
  int my_sig;
  void tock() {
    my_sig = 1;
  }
};

class Module {
 public:

  int my_sig;

  void tock() {
    if (1)
      submod_.tock();
    else
      submod_.tock();
    my_sig = submod_.my_sig;
  }

  Submod submod_;
};
