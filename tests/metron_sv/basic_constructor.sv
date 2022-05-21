`include "metron_tools.sv"

// Modules can use constructors to initialize state

module Module
(
  // get_reg() bindings
  output logic[7:0] get_reg_ret
);
/*public:*/

  initial begin
    my_reg = 7;
  end

  function logic[7:0] get_reg();
    get_reg = my_reg;
  endfunction
  always_comb get_reg_ret = get_reg();

/*private:*/

  logic[7:0] my_reg;
endmodule


`ifdef IVERILOG
module Test;
  logic clock;
  logic[7:0] tock;
  Module mod(.clock(clock), .tock(tock));
  initial begin
    if (tock != 7) $display("FAIL");
  end
endmodule
`endif

