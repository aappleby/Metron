// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Design implementation internals
// See Vbasic_lockstep1.h for the primary calling header

#include "verilated.h"

#include "Vbasic_lockstep1__Syms.h"
#include "Vbasic_lockstep1___024root.h"

void Vbasic_lockstep1___024root___ctor_var_reset(Vbasic_lockstep1___024root* vlSelf);

Vbasic_lockstep1___024root::Vbasic_lockstep1___024root(const char* _vcname__)
    : VerilatedModule(_vcname__)
 {
    // Reset structure values
    Vbasic_lockstep1___024root___ctor_var_reset(this);
}

void Vbasic_lockstep1___024root::__Vconfigure(Vbasic_lockstep1__Syms* _vlSymsp, bool first) {
    if (false && first) {}  // Prevent unused
    this->vlSymsp = _vlSymsp;
}

Vbasic_lockstep1___024root::~Vbasic_lockstep1___024root() {
}
