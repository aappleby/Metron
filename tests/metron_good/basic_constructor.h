#include "metron_tools.h"

// Modules can use constructors to initialize state

class Module {
public:

  Module() {
    my_reg = 7;
  }

  logic<8> tock() {
    return my_reg;
  }

private:

  logic<8> my_reg;
};