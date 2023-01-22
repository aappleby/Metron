`include "metron_tools.sv"

//------------------------------------------------------------------------------
// verilator lint_off unusedsignal
// verilator lint_off undriven

typedef struct packed {
  logic[31:0] a_data;
} tilelink_a;

module block_ram (
  // global clock
  input logic clock,
  // output signals
  output logic[31:0] data,
  // unshell() ports
  input tilelink_a unshell_tla,
  output logic[31:0] unshell_ret,
  // tick() ports
  input tilelink_a tick_tla
);
/*public:*/

  always_comb begin : unshell
    unshell_ret = unshell_tla.a_data;
  end

  always_ff @(posedge clock) begin : tick
    data <= tick_tla.a_data;
  end

endmodule

// verilator lint_on unusedsignal
// verilator lint_on undriven

//------------------------------------------------------------------------------
