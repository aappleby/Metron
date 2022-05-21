`include "metron_tools.sv"

// We can instantiated templated classes as submodules.

module Submod
(
  // global clock
  input logic clock,
  // output registers
  output logic[7:0] sub_reg
);
/*public:*/

  always_comb begin : tock
  end


/*private:*/

  always_ff @(posedge clock) begin : tick
    sub_reg <= sub_reg + 1;
  end

endmodule

module Module
(
  // global clock
  input logic clock,
  // get_submod_reg() bindings
  output logic[7:0] get_submod_reg_ret
);
/*public:*/

  function logic[7:0] get_submod_reg();
    get_submod_reg = submod_sub_reg;
  endfunction
  always_comb get_submod_reg_ret = get_submod_reg();

  always_comb begin : tock
  end

  Submod submod(
    .clock(clock),
    .sub_reg(submod_sub_reg)
  );
  logic[7:0] submod_sub_reg;

endmodule
