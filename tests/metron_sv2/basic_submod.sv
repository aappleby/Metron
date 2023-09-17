`include "metron/tools/metron_tools.sv"

// Modules can contain other modules.

module Submod (
  // global clock
  input logic clock,
  // tock() ports
  input int tock_x
);
/*public:*/

  always_comb begin : tock
    tick_x = tock_x;
    /*tick(x);*/
  end

/*private:*/

  always_ff @(posedge clock) begin : tick
    sub_reg <= sub_reg + tick_x;
  end
  int tick_x;

  logic[7:0] sub_reg;
endmodule

module Module (
  // global clock
  input logic clock,
  // tock() ports
  input int tock_x
);
/*public:*/

  always_comb begin : tock
    submod_tock_x = tock_x;
    /*submod.tock(x);*/
  end

  Submod  submod(
    // global clock
    input logic clock,
  );
  (submod binding fields go here);
endmodule