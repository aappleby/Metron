// RISC-V SiMPLE SV -- single-cycle controller
// BSD 3-Clause License
// (c) 2017-2019, Arthur Matos, Marcus Vinicius Lamar, Universidade de Brasília,
//                Marek Materzok, University of Wrocław

#pragma once
#include "../metron_tools.h"
#include "config.h"
#include "constants.h"

struct singlecycle_control {
  logic<1> o_pc_write_enable;
  logic<1> o_regfile_write_enable;
  logic<1> o_alu_operand_a_select;
  logic<1> o_alu_operand_b_select;
  logic<2> o_alu_op_type;
  logic<1> o_data_mem_read_enable;
  logic<1> o_data_mem_write_enable;
  logic<3> o_reg_writeback_select;
  logic<2> o_next_pc_select;

  void tock_next_pc_select(logic<7> i_inst_opcode,
                           logic<1> i_take_branch) {
    switch (i_inst_opcode) {
      case OPCODE_BRANCH: o_next_pc_select = i_take_branch ? CTL_PC_PC_IMM : CTL_PC_PC4; break;
      case OPCODE_JALR:   o_next_pc_select = CTL_PC_RS1_IMM; break;
      case OPCODE_JAL:    o_next_pc_select = CTL_PC_PC_IMM; break;
      default:            o_next_pc_select = CTL_PC_PC4; break;
    }
  }

  void tock_decode(logic<7> i_inst_opcode) {
    o_pc_write_enable         = b1(1);
    o_regfile_write_enable    = b1(0);
    o_alu_operand_a_select    = b1(DONTCARE);
    o_alu_operand_b_select    = b1(DONTCARE);
    o_alu_op_type             = b2(DONTCARE);
    o_data_mem_read_enable    = b1(0);
    o_data_mem_write_enable   = b1(0);
    o_reg_writeback_select    = b3(DONTCARE);
    
    switch (i_inst_opcode) {
      case OPCODE_LOAD:
      {
        o_regfile_write_enable    = b1(1);
        o_alu_operand_a_select    = CTL_ALU_A_RS1;
        o_alu_operand_b_select    = CTL_ALU_B_IMM;
        o_alu_op_type             = CTL_ALU_ADD;
        o_data_mem_read_enable    = b1(1);
        o_reg_writeback_select    = CTL_WRITEBACK_DATA;
        break;
      }
    
      case OPCODE_MISC_MEM:
      {
        // Fence - ignore
        break;
      }
    
      case OPCODE_OP_IMM:
      {
        o_regfile_write_enable    = b1(1);
        o_alu_operand_a_select    = CTL_ALU_A_RS1;
        o_alu_operand_b_select    = CTL_ALU_B_IMM;
        o_alu_op_type             = CTL_ALU_OP_IMM;
        o_reg_writeback_select    = CTL_WRITEBACK_ALU;
        break;
      }
    
      case OPCODE_AUIPC:
      {
        o_regfile_write_enable    = b1(1);
        o_alu_operand_a_select    = CTL_ALU_A_PC;
        o_alu_operand_b_select    = CTL_ALU_B_IMM;
        o_alu_op_type             = CTL_ALU_ADD;
        o_reg_writeback_select    = CTL_WRITEBACK_ALU;
        break;
      }
    
      case OPCODE_STORE:
      {
        o_alu_operand_a_select    = CTL_ALU_A_RS1;
        o_alu_operand_b_select    = CTL_ALU_B_IMM;
        o_alu_op_type             = CTL_ALU_ADD;
        o_data_mem_write_enable   = b1(1);
        break;
      }
    
      case OPCODE_OP:
      {
        o_regfile_write_enable    = b1(1);
        o_alu_operand_a_select    = CTL_ALU_A_RS1;
        o_alu_operand_b_select    = CTL_ALU_B_RS2;
        o_reg_writeback_select    = CTL_WRITEBACK_ALU;
        o_alu_op_type             = CTL_ALU_OP;
        break;
      }
    
      case OPCODE_LUI:
      {
        o_regfile_write_enable    = b1(1);
        o_alu_operand_a_select    = CTL_ALU_A_RS1;
        o_alu_operand_b_select    = CTL_ALU_B_RS2;
        o_reg_writeback_select    = CTL_WRITEBACK_IMM;
        break;
      }
    
      case OPCODE_BRANCH:
      {
        o_alu_operand_a_select    = CTL_ALU_A_RS1;
        o_alu_operand_b_select    = CTL_ALU_B_RS2;
        o_alu_op_type             = CTL_ALU_BRANCH;
        break;
      }
    
      case OPCODE_JALR:
      {
        o_regfile_write_enable    = b1(1);
        o_alu_operand_a_select    = CTL_ALU_A_RS1;
        o_alu_operand_b_select    = CTL_ALU_B_IMM;
        o_alu_op_type             = CTL_ALU_ADD;
        o_reg_writeback_select    = CTL_WRITEBACK_PC4;
        break;
      }
    
      case OPCODE_JAL:
      {
        o_regfile_write_enable    = b1(1);
        o_alu_operand_a_select    = CTL_ALU_A_PC;
        o_alu_operand_b_select    = CTL_ALU_B_IMM;
        o_alu_op_type             = CTL_ALU_ADD;
        o_reg_writeback_select    = CTL_WRITEBACK_PC4;
        break;
      }
    
      default:
      {
        o_pc_write_enable         = b1(DONTCARE);
        o_regfile_write_enable    = b1(DONTCARE);
        o_data_mem_read_enable    = b1(DONTCARE);
        o_data_mem_write_enable   = b1(DONTCARE);
        break;
      }
    }
  }
};
