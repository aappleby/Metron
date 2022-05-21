`include "metron_tools.sv"

// Cramming various statements into one line should not break anything.

module Module
(
  // global clock
  input logic clock,
  // output registers
  output logic[7:0] my_reg,
  // test() bindings
  output logic[7:0] test_ret
);
/*public:*/

  function logic[7:0] test();  logic[7:0] a;
a = 1; a = a + 7; test = a; endfunction
  always_comb test_ret = test();

  always_ff @(posedge clock) begin : tick if (my_reg & 1) my_reg <= my_reg - 7; end


endmodule
