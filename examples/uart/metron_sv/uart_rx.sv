`include "metron_tools.sv"

//==============================================================================

module uart_rx
#(parameter int cycles_per_bit = 4)
(
  input logic clock,
  input logic tock_i_rstn,
  input logic tock_i_serial,
  output logic  tock_valid,
  output logic[7:0]  tock_buffer,
  output logic[31:0] tock_sum
);
 /*public:*/
  //----------------------------------------

  // yosys doesn't appear to handle return values from functions at all
  // v*rilator doesn't allow "return;" if the function has a return value
  // even if you set it via "func_name = value;" first.


  always_comb begin /*tock_valid*/ tock_valid = cursor == 1; end
  always_comb begin /*tock_buffer*/ tock_buffer = buffer; end
  always_comb begin /*tock_sum*/ tock_sum = sum; end

  always_comb begin /*tock*/ tick_i_rstn = tock_i_rstn;
tick_i_serial = tock_i_serial;
/*tick(i_rstn, i_serial)*/; end

  //----------------------------------------
 /*private:*/
  logic tick_i_rstn;
  logic tick_i_serial;
  always_ff @(posedge clock) begin /*tick*/
    if (!tick_i_rstn) begin
      cycle <= 0;
      cursor <= 0;
      buffer <= 0;
      sum <= 0;
    end else begin
      if (cycle != 0) begin
        cycle <= cycle - 1;
      end else if (cursor != 0) begin
        logic[7:0] temp;

        temp = (tick_i_serial << 7) | (buffer >> 1);
        if (cursor - 1 == 1) sum <= sum + temp;
        cycle <= cycle_max;
        cursor <= cursor - 1;
        buffer <= temp;
      end else if (tick_i_serial == 0) begin
        cycle <= cycle_max;
        cursor <= cursor_max;
      end
    end
  end

  localparam int cycle_bits = $clog2(cycles_per_bit);
  localparam int cycle_max = cycles_per_bit - 1;
  localparam int cursor_max = 9;
  localparam int cursor_bits = $clog2(cursor_max);

  logic[cycle_bits-1:0] cycle;
  logic[cursor_bits-1:0] cursor;
  logic[7:0] buffer;
  logic[31:0] sum;
endmodule

//==============================================================================

