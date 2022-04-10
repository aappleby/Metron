// RISC-V SiMPLE SV -- Toplevel
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

#ifndef RVSIMPLE_TOPLEVEL_H
#define RVSIMPLE_TOPLEVEL_H

#include "config.h"
#include "constants.h"
#include "example_data_memory_bus.h"
#include "example_text_memory_bus.h"
#include "metron_tools.h"
#include "riscv_core.h"

class toplevel {
 public:
  logic<32> o_bus_read_data;
  logic<32> o_bus_address;
  logic<32> o_bus_write_data;
  logic<4>  o_bus_byte_enable;
  logic<1>  o_bus_read_enable;
  logic<1>  o_bus_write_enable;
  logic<32> o_inst;
  logic<32> o_pc;

  //----------------------------------------

  void tock(logic<1> reset) {
    core.reset = reset;
    core.tock1();

    text_memory_bus.address = core.pc;
    text_memory_bus.tock_read_data();

    core.inst = text_memory_bus.read_data;
    core.tock2();

    data_memory_bus.address = core.alu_result;
    data_memory_bus.read_enable = core.bus_read_enable;
    data_memory_bus.tock_q();

    o_inst = text_memory_bus.read_data;
    o_bus_read_data = data_memory_bus.q;
    o_bus_address = core.alu_result;
    o_bus_write_data = core.bus_write_data;
    o_bus_byte_enable = core.bus_byte_enable;
    o_bus_read_enable = core.bus_read_enable;
    o_bus_write_enable = core.bus_write_enable;
    o_pc = core.pc;

    data_memory_bus.write_enable = core.bus_write_enable;
    data_memory_bus.byte_enable = core.bus_byte_enable;
    data_memory_bus.write_data = core.bus_write_data;
    data_memory_bus.tock();

    core.bus_read_data = data_memory_bus.q;
    core.tock3();
  }

  //----------------------------------------

 private:
  riscv_core core;
  example_text_memory_bus text_memory_bus;
  example_data_memory_bus data_memory_bus;
};

#endif  // RVSIMPLE_TOPLEVEL_H
