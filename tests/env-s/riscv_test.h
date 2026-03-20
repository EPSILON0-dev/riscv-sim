// Simplified envirnonment only supports the RV64U tests, it does not
//  interact with any non-ISA elements of the CPU.

#ifndef _ENV_SIMPLIFIED_SINGLE_CORE_H
#define _ENV_SIMPLIFIED_SINGLE_CORE_H

#include "../riscv-tests/env/encoding.h"

//-----------------------------------------------------------------------
// Begin Macro
//-----------------------------------------------------------------------

#define RVTEST_RV64U \
    .macro init;     \
    .endm

#define INIT_XREG \
    li x1, 0;     \
    li x2, 0;     \
    li x3, 0;     \
    li x4, 0;     \
    li x5, 0;     \
    li x6, 0;     \
    li x7, 0;     \
    li x8, 0;     \
    li x9, 0;     \
    li x10, 0;    \
    li x11, 0;    \
    li x12, 0;    \
    li x13, 0;    \
    li x14, 0;    \
    li x15, 0;    \
    li x16, 0;    \
    li x17, 0;    \
    li x18, 0;    \
    li x19, 0;    \
    li x20, 0;    \
    li x21, 0;    \
    li x22, 0;    \
    li x23, 0;    \
    li x24, 0;    \
    li x25, 0;    \
    li x26, 0;    \
    li x27, 0;    \
    li x28, 0;    \
    li x29, 0;    \
    li x30, 0;    \
    li x31, 0;

// clang-format off
// Formatting messes up section directive
#define RVTEST_CODE_BEGIN \
    .section .text.init;  \
    .align 6;             \
    .globl _start;        \
    _start:               \
    /* reset vector */    \
    j reset_vector;       \
    .align 2;             \
    reset_vector:         \
    INIT_XREG;            \
    li TESTNUM, 0;
// clang-format on

//-----------------------------------------------------------------------
// End Macro
//-----------------------------------------------------------------------

#define RVTEST_CODE_END unimp

//-----------------------------------------------------------------------
// Pass/Fail Macro
//-----------------------------------------------------------------------

#define TESTNUM gp
#define RVTEST_PASS \
    fence;          \
    li TESTNUM, 1;  \
    1 : j 1b;

#define RVTEST_FAIL \
    fence;          \
    1 : j 1b;

//-----------------------------------------------------------------------
// Data Section Macro
//-----------------------------------------------------------------------

#define EXTRA_DATA

// clang-format off
// Formatting messes up section directive
#define RVTEST_DATA_BEGIN                           \
    EXTRA_DATA                                      \
    .pushsection .tohost, "aw", @progbits;          \
    .align 6;                                       \
    .global tohost;                                 \
    tohost:                                         \
    .dword 0;                                       \
    .size tohost, 8;                                \
    .align 6;                                       \
    .global fromhost;                               \
    fromhost:                                       \
    .dword 0;                                       \
    .size fromhost, 8;                              \
    .popsection;                                    \
    .align 4;                                       \
    .global begin_signature;                        \
    begin_signature:
// clang-format on

#define RVTEST_DATA_END    \
    .align 4;              \
    .global end_signature; \
    end_signature:

#endif
