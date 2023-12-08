// RISC-V SiMPLE SV -- data memory interface
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

`ifndef DATA_MEMORY_INTERFACE_H
`define DATA_MEMORY_INTERFACE_H

`include "config.sv"
`include "constants.sv"
`include "metron/metron_tools.sv"

module data_memory_interface (
);
 /*public:*/


 /*private:*/
  logic[31:0] position_fix;
  logic[31:0] sign_fix;

 /*public:*/
  always_comb begin : tock_bus
    bus_address = address;
    bus_write_enable = write_enable;
    bus_read_enable = read_enable;
    bus_write_data = write_data << (8 * 2'(address));

    // calculate byte enable
    // clang-format off
    case (2'(data_format))
      2'b00:  begin bus_byte_enable = 4'b0001 << 2'(address); /*break;*/ end
      2'b01:  begin bus_byte_enable = 4'b0011 << 2'(address); /*break;*/ end
      2'b10:  begin bus_byte_enable = 4'b1111 << 2'(address); /*break;*/ end
      default:   bus_byte_enable = 4'b0000; /*break;*/
    endcase
    // clang-format on
  end

  // correct for unaligned accesses
  always_comb begin : tock_read_data
    position_fix = 32'(bus_read_data >> (8 * 2'(address)));

    // sign-extend if necessary
    // clang-format off
    case (2'(data_format))
      2'b00:  begin sign_fix = {{24 {1'(~data_format[2] & position_fix[7])}}, 8'(position_fix)}; /*break;*/ end
      2'b01:  begin sign_fix = {{16 {1'(~data_format[2] & position_fix[15])}}, 16'(position_fix)}; /*break;*/ end
      2'b10:  begin sign_fix = 32'(position_fix); /*break;*/ end
      default:   sign_fix = 'x; /*break;*/
    endcase
    // clang-format on

    read_data = sign_fix;
  end
endmodule

`endif // DATA_MEMORY_INTERFACE_H
