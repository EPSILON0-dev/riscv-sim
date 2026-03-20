# RISC-V Simulator

This is a RISC-V (RVA20S64) simulator capable of running Linux. _Actually it's not compliant with RVA20S64 yet, but soon will be ^^_

## Compatibility

Currently implemented modules:
* RV64I - Base Integer 64-bit instructions
* M - Integer Multiplication and Division
* Zifencei - Instruction-Fetch fence (_treated as a hint_)

Planned ISA and Extensions:
* Zicsr - Control and Status Registers
* Zimop - May-be-operations
* Zicntr - Hardware counters
* Zihpm - Hardware performance monitors
* A - Atomic Instructions
* F - Single-precision Floating Point Operations
* D - Double-precision Floating Point Operations
* C - Compressed instructions

Planned privileged extensions:
* Svbare (satp mode) -  Supervisor Address Translation and Protection
* Sv39 - Page-Based 39-bit Virtual-Memory System
* Svade - Addressing and Memory Protection
* Ssccptr - Main memory cacheability
* Sstvecd - Direct access support
* Sstvala - Storing virtual address on access fault

## Usage

### Compiling the simulator

```sh
cmake -B build; make -j -C build
```

### Compiling and running simplified environment tests

```sh
cd tests/env-s
make -j # use "RISCV_PREFIX=riscv64-unknown-elf-" to set the toolchain prefix
cd ..
./test_simplified.sh
```
