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

  void init() {
    core.init();
    text_memory_bus.init();
    data_memory_bus.init();
  }

  //----------------------------------------

  void tock(logic<1> reset) {
    logic<32> pc           = core.pc();
    logic<32> inst         = text_memory_bus.read_data(pc);
    logic<32> alu_result2  = core.alu_result(inst);
    logic<32> write_data   = core.bus_write_data2(inst, alu_result2);
    logic<1>  write_enable = core.bus_write_enable2(inst);
    logic<4>  byte_enable  = core.bus_byte_enable2(inst, alu_result2);
    logic<1>  read_enable  = core.bus_read_enable2(inst);
    logic<32> read_data    = data_memory_bus.read_data(alu_result2, read_enable);

    o_inst = inst;
    o_bus_read_data = read_data;
    o_bus_address = alu_result2;
    o_bus_write_data = write_data;
    o_bus_byte_enable = byte_enable;
    o_bus_read_enable = read_enable;
    o_bus_write_enable = write_enable;
    o_pc = pc;

    data_memory_bus.tocktick(alu_result2, write_enable, byte_enable, write_data);
    core.tocktick_regs(reset, inst, read_data, alu_result2);
  }

  //----------------------------------------

 private:
  riscv_core core;
  example_text_memory_bus text_memory_bus;
  example_data_memory_bus data_memory_bus;
};

#endif  // RVSIMPLE_TOPLEVEL_H