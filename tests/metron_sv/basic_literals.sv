`include "metron_tools.sv"

// Number literals
// don't forget the ' spacers

module Module
(
  output int tock1_ret,
  output int tock2_ret
);
/*public:*/

  function int tock1();
    logic[31:0] a;
    logic[31:0] b;
    logic[31:0] c;
    logic[31:0] d;
    logic[31:0] e;
    logic[31:0] f;
    logic[31:0] g;
    logic[31:0] h;
    a = 1'b0;
    b = 2'b00;
    c = 3'b000;
    d = 4'b0000;
    e = 5'b00000;
    f = 6'b000000;
    g = 7'b0000000;
    h = 8'b00000000;
    tock1 = 0;
  endfunction
  always_comb tock1_ret = tock1();

  function int tock2();
    logic[31:0] a;
    logic[31:0] b;
    logic[31:0] c;
    logic[31:0] d;
    logic[31:0] e;
    logic[31:0] f;
    logic[31:0] g;
    logic[31:0] h;
    a = 1'b0;
    b = 2'b0_0;
    c = 3'b0_00;
    d = 4'b00_00;
    e = 5'b00_000;
    f = 6'b0_000_00;
    g = 7'b000_0000;
    h = 8'b0_0_0_0_0_0_0_0;
    tock2 = 0;
  endfunction
  always_comb tock2_ret = tock2();

endmodule
