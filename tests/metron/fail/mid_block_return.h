#include "metron/metron_tools.h"

// If for some reason you put a return in the middle of a block, we should catch
// it.

// X Method tock has non-terminal return

class Module {
 public:
  logic<8> tock() {
    logic<8> a = 10;
    logic<8> b = 2;
    logic<8> c;

    c = a + b;
    return c;
    c = b + b;
  }
};
