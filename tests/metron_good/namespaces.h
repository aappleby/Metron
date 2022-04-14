#include "metron_tools.h"

// Namespaces turn into packages.

namespace MyPackage {
  static const int foo = 3;
};

class Module {
public:

  logic<8> tock1() {
    return MyPackage::foo;
  }

  logic<8> tock2() {
    using namespace MyPackage;
    return foo;
  }
};
