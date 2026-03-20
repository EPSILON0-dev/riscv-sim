#pragma once

#include "regs.hpp"

namespace Sim
{

class Operation
{
   public:
    // clang-format off
    enum class InstructionType : uint8_t
    {
        LOAD      = 0b00000,
        MISC_MEM  = 0b00011,
        OP_IMM    = 0b00100,
        AUIPC     = 0b00101,
        OP_IMM_32 = 0b00110,
        STORE     = 0b01000,
        OP        = 0b01100,
        LUI       = 0b01101,
        OP_32     = 0b01110,
        BRANCH    = 0b11000,
        JALR      = 0b11001,
        JAL       = 0b11011,
        SYSTEM    = 0b11100,
        INVALID   = 0xFF
    };

    enum class Instruction : uint8_t
    {
        INVALID,
        LUI, AUIPC, JAL, JALR,
        BEQ, BNE, BLT, BGE, BLTU, BGEU,
        LB, LH, LW, LD, LBU, LHU, LWU,
        SB, SH, SW, SD,
        ADDI, SLTI, SLTIU, XORI, ORI, ANDI, SLLI, SRLI, SRAI, 
        ADDIW, SLLIW, SRLIW, SRAIW,
        ADD, SUB, SLT, SLTU, XOR, OR, AND, SLL, SRL, SRA, 
        ADDW, SUBW, SLLW, SRLW, SRAW,
        FENCE, FENCE_TSO,
        PAUSE, ECALL, EBREAK
    };
    // clang-format on

   private:
    uint64_t address_;
    uint64_t opcode_;
    uint8_t lengthBits_, funct3_, funct7_;
    InstructionType type_;
    Instruction instr_;
    uint64_t imm_;
    Regs::Reg rd1_, rs1_, rs2_;

    void DecodeImmediate();

   public:
    Operation(uint64_t address, uint32_t opcode);
    ~Operation() = default;

   public:
    static std::string NameOf(InstructionType type);
    static std::string NameOf(Instruction instr);

    std::string TypeString() const noexcept;
    std::string InstructionString() const noexcept;
    void Decode();

    auto Address() const noexcept { return address_; }
    auto Type() const noexcept { return type_; }
    auto Instruction() const noexcept { return instr_; }
    auto Immediate() const noexcept { return imm_; }
    auto Rd() const noexcept { return rd1_; }
    auto Rs1() const noexcept { return rs1_; }
    auto Rs2() const noexcept { return rs2_; }
};

}  // namespace Sim
