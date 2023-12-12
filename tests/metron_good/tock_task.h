#include "metron/metron_tools.h"

// Tocks should be able to call private tasks and functions

class Module {
public:

  logic<8> my_signal;

  int tock_ret;
  void tock() {
    set_signal(get_number());
    tock_ret = set_signal_ret;
  }

private:

  logic<8> get_number() const {
    return 7;
  }

  int set_signal_ret;
  void set_signal(logic<8> number) {
    my_signal = number;
    set_signal_ret = my_signal;
  }

};
