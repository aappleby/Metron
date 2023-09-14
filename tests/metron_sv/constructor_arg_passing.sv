`include "metron/tools/metron_tools.sv"

// We should be able to pass constructor arguments to submodules.

//----------------------------------------

module Module (
  // global clock
  input logic clock,
  // tock() ports
  input logic[9:0] tock_new_addr,
  // get_data() ports
  output logic[7:0] get_data_ret
);
  parameter data_len = 1024;
  parameter blarp = 0;

/*public:*/

  parameter /*const*/ filename = "";
  initial begin
    if (filename) $readmemh(filename, data);
  end

  always_comb begin : tock
    addr = tock_new_addr;
  end

  always_ff @(posedge clock) begin : tick
    out <= data[addr];
  end

  always_comb begin : get_data
    get_data_ret = out;
  end

/*private:*/
  logic[9:0] addr;
  logic[7:0] data[data_len];
  logic[7:0] out;
endmodule

//----------------------------------------

module Top (
  // global clock
  input logic clock,
  // tock() ports
  input logic[9:0] tock_addr
);
/*public:*/
  initial begin
  end

  always_comb begin : tock
    mod_tock_new_addr = tock_addr;
  end

  always_ff @(posedge clock) begin : tick
    //mod.tick();
  end

  Module #(
    // Template Parameters
    .data_len(7777),
    .blarp(8383),
    // Constructor Parameters
    .filename("examples/uart/message.hex")
  ) mod(
    // Global clock
    .clock(clock),
    // tock() ports
    .tock_new_addr(mod_tock_new_addr),
    // get_data() ports
    .get_data_ret(mod_get_data_ret)
  );
  logic[9:0] mod_tock_new_addr;
  logic[7:0] mod_get_data_ret;
endmodule

//----------------------------------------
