`include "metron_tools.sv"

// Basic #defines should work as long as their contents are simultaneously
// valid C++ and SV

`define MY_CONSTANT1 10
`define MY_CONSTANT2 20
`define MY_OTHER_CONSTANT (`MY_CONSTANT1 + `MY_CONSTANT2 + 7)

module Module
(
  input logic clock,
  output logic[7:0] tock
);
/*public:*/

  always_comb begin /*tock*/
    tock = `MY_OTHER_CONSTANT;
  end

endmodule
