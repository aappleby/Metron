#include <stdio.h>

#include "toplevel.h"

//------------------------------------------------------------------------------

uint64_t total_tocks = 0;
uint64_t total_time = 0;

const int reps = 1000;
const int max_cycles = 1000;

int run_test(const char* test_name, const int reps, const int timeout, bool verbose) {
  fflush(stdout);

  char buf1[256];
  char buf2[256];
  sprintf(buf1, "+text_file=rv_tests/%s.text.vh", test_name);
  sprintf(buf2, "+data_file=rv_tests/%s.data.vh", test_name);
  const char* argv2[2] = {buf1, buf2};

  metron_reset();
  metron_init(2, argv2);

  int time = 0;
  int result = 0;

  toplevel top;
  top.init();

  if (verbose) LOG_R("running %6s: ", test_name);

  auto time_a = timestamp();
  for (int rep = 0; rep < reps; rep++) {
    top.tick(1);
    total_tocks++;
    for (time = 0; time < timeout; time++) {
      top.tick(0);
      total_tocks++;

      if (top.o_bus_write_enable && top.o_bus_address == 0xfffffff0) {
        result = top.o_bus_write_data;
        break;
      }
    }
  }
  auto time_b = timestamp();
  total_time += time_b - time_a;

  if (time == max_cycles) {
    if (verbose) LOG_Y("TIMEOUT\n");
    return -1;
  } else if (result) {
    if (verbose) LOG_G("PASS %d @ %d\n", result, time);
    return 0;
  } else {
    if (verbose) LOG_R("FAIL %d @ %d\n", result, time);
    return -1;
  }
}

//------------------------------------------------------------------------------

int main(int argc, const char** argv) {
  LOG_B("Starting %s @ %d reps...\n", argv[0], reps);

  const char* instructions[38] = {
    "add", "addi", "and", "andi", "auipc", "beq",  "bge", "bgeu",
    "blt", "bltu", "bne", "jal",  "jalr",  "lb",   "lbu", "lh",
    "lhu", "lui",  "lw",  "or",   "ori",   "sb",   "sh",  "simple",
    "sll", "slli", "slt", "slti", "sltiu", "sltu", "sra", "srai",
    "srl", "srli", "sub", "sw",   "xor",   "xori"};


  LOG_B("Warming up...\n");
  for (int i = 0; i < 38; i++) {
    run_test(instructions[i], reps / 10, max_cycles, false);
  }

  total_tocks = 0;
  total_time = 0;

  LOG_B("Testing...\n");
  for (int i = 0; i < 38; i++) {
    run_test(instructions[i], reps, max_cycles, true);
  }

  double rate = double(total_tocks) / double(total_time);
  LOG_B("Sim rate %f mhz\n", rate * 1000.0);

  return 0;
}

//------------------------------------------------------------------------------