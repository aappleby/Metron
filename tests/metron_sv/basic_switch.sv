`include "metron_tools.sv"

// Simple switch statements are OK.

module Module
(
  input logic clock,
  input logic[1:0] tock_selector
);
/*public:*/

  always_comb begin : tock
    tick_selector = tock_selector;
  end

/*private:*/

  task automatic tick(logic[1:0] selector);
    case(selector)
      0: // comment
        my_reg <= 17;
      1:  // comment
        my_reg <= 22;
      2: my_reg <= 30;
      3, // fallthrough
      4,
      5,
      6: my_reg <= 72;
    endcase
  endtask
  logic[1:0] tick_selector;
  always_ff @(posedge clock) tick(tick_selector);

  logic[7:0] my_reg;
endmodule
