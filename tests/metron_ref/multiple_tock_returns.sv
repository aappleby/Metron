`include "metron_tools.sv"

// Tock functions can't have more than a single return at the end.

module Module
(
  input logic clock,
  input logic[7:0] data,
  output logic[7:0] tock
);
/*public:*/

  always_comb begin /*tock*/
    tock = 11;
  end
endmodule
