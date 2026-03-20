#include "operation.hpp"

#include <format>
#include <magic_enum/magic_enum.hpp>
#include <stdexcept>

using namespace Sim;

Operation::Operation(uint64_t address, uint32_t opcode) : address_(address), opcode_(opcode) {}

std::string Operation::NameOf(enum InstructionType type)
{
    return std::string(magic_enum::enum_name(type));
}

std::string Operation::NameOf(enum Instruction instr)
{
    return std::string(magic_enum::enum_name(instr));
}

std::string Operation::TypeString() const noexcept
{
    return NameOf(type_);
}

std::string Operation::InstructionString() const noexcept
{
    return NameOf(instr_);
}

void Operation::DecodeImmediate()
{
    switch (type_)
    {
        // Format I
        case InstructionType::LOAD:
        case InstructionType::OP_IMM:
        case InstructionType::OP_IMM_32:
        case InstructionType::JALR:
            imm_ = (opcode_ >> 20) & 0xfff;
            if (imm_ & 0x800) imm_ |= 0xfffffffffffff000;
            break;

        // Format S
        case InstructionType::STORE:
            imm_ = ((opcode_ >> 20) & 0xfe0) | ((opcode_ >> 7) & 0x1f);
            if (imm_ & 0x800) imm_ |= 0xfffffffffffff000;
            break;

        // Format U
        case InstructionType::LUI:
        case InstructionType::AUIPC:
            imm_ = (opcode_ & 0xfffff000);
            if (imm_ & 0x80000000) imm_ |= 0xffffffff00000000;
            break;

        // Format B
        case InstructionType::BRANCH:
            imm_ = ((opcode_ >> 19) & 0x1000) | ((opcode_ >> 20) & 0x7e0) |
                   ((opcode_ >> 7) & 0x1e) | ((opcode_ << 4) & 0x800);
            if (imm_ & 0x1000) imm_ |= 0xfffffffffffff000;
            break;

        // Format J
        case InstructionType::JAL:
            imm_ = (opcode_ & 0x000ff000) | ((opcode_ >> 9) & 0x800) | ((opcode_ >> 20) & 0x7fe) |
                   ((opcode_ >> 11) & 0x100000);
            if (imm_ & 0x100000) imm_ |= 0xfffffffffff00000;
            break;

        default:
            imm_ = 0;
            break;
    }
}

void Operation::Decode()
{
    // Check for 16-bit instruction (currently not supported)
    if ((opcode_ & 0x3) != 0x3)
    {
        type_ = InstructionType::INVALID;
        auto error =
            std::format("Invalid 16bit instruction 0x{:04x} at address 0x{:x}", opcode_, address_);
        throw std::runtime_error(error);
    }
    lengthBits_ = 32;

    // Check for a valid instruction type
    uint8_t opcode5 = (opcode_ >> 2) & 0x1F;
    if (!magic_enum::enum_contains<InstructionType>(opcode5))
    {
        type_ = InstructionType::INVALID;
        auto error =
            std::format("Invalid insturction type 0x{:08x} at address 0x{:x}", opcode_, address_);
        throw std::runtime_error(error);
    }
    type_ = static_cast<InstructionType>(opcode5);

    // Extract fields for later use
    funct3_ = (opcode_ >> 12) & 0x7;
    funct7_ = (opcode_ >> 25) & 0x7F;
    rd1_ = static_cast<Regs::Reg>((opcode_ >> 7) & 0x1F);
    rs1_ = static_cast<Regs::Reg>((opcode_ >> 15) & 0x1F);
    rs2_ = static_cast<Regs::Reg>((opcode_ >> 20) & 0x1F);

    // Decode based on instruction type
    bool valid = true;
    switch (type_)
    {
        case InstructionType::LOAD:
            // clang-format off
            switch (funct3_)
            {
                case 0b000: instr_ = Instruction::LB;  break;
                case 0b001: instr_ = Instruction::LH;  break;
                case 0b010: instr_ = Instruction::LW;  break;
                case 0b011: instr_ = Instruction::LD;  break;
                case 0b100: instr_ = Instruction::LBU; break;
                case 0b101: instr_ = Instruction::LHU; break;
                case 0b110: instr_ = Instruction::LWU; break;
                default: valid = false; break;
            }
            // clang-format on
            break;

        case InstructionType::STORE:
            // clang-format off
            switch (funct3_)
            {
                case 0b000: instr_ = Instruction::SB; break;
                case 0b001: instr_ = Instruction::SH; break;
                case 0b010: instr_ = Instruction::SW; break;
                case 0b011: instr_ = Instruction::SD; break;
                default: valid = false; break;
            }
            // clang-format on
            break;

        case InstructionType::OP:
            // clang-format off
            switch (funct3_)
            {
                case 0b000:
                    instr_ = (funct7_ == 0x20) ? Instruction::SUB : Instruction::ADD;  
                    break;
                case 0b001: instr_ = Instruction::SLL;  break;
                case 0b010: instr_ = Instruction::SLT;  break;
                case 0b011: instr_ = Instruction::SLTU; break;
                case 0b100: instr_ = Instruction::XOR;  break;
                case 0b101: 
                    instr_ = (funct7_ == 0x20) ? Instruction::SRA : Instruction::SRL;  
                    break;
                case 0b110: instr_ = Instruction::OR;   break;
                case 0b111: instr_ = Instruction::AND;  break;
                default: valid = false; break;
            }
            // clang-format on
            break;

        case InstructionType::OP_IMM:
            // clang-format off
            switch (funct3_)
            {
                case 0b000: instr_ = Instruction::ADDI;  break;
                case 0b001: instr_ = Instruction::SLLI;  break;
                case 0b010: instr_ = Instruction::SLTI;  break;
                case 0b011: instr_ = Instruction::SLTIU; break;
                case 0b100: instr_ = Instruction::XORI;  break;
                case 0b101: 
                    instr_ = ((funct7_ & 0x7e) == 0x20) ? Instruction::SRAI : Instruction::SRLI;  
                    break;
                case 0b110: instr_ = Instruction::ORI;   break;
                case 0b111: instr_ = Instruction::ANDI;  break;
                default: valid = false; break;
            }
            // clang-format on
            break;

        case InstructionType::OP_32:
            // clang-format off
            switch (funct3_)
            {
                case 0b000:
                    instr_ = (funct7_ == 0x20) ? Instruction::SUBW : Instruction::ADDW;  
                    break;
                case 0b001: instr_ = Instruction::SLLW;  break;
                case 0b101: 
                    instr_ = (funct7_ == 0x20) ? Instruction::SRAW : Instruction::SRLW;  
                    break;
                default: valid = false; break;
            }
            // clang-format on
            break;

        case InstructionType::OP_IMM_32:
            // clang-format off
            switch (funct3_)
            {
                case 0b000: instr_ = Instruction::ADDIW;  break;
                case 0b001: instr_ = Instruction::SLLIW;  break;
                case 0b101: 
                    instr_ = (funct7_ == 0x20) ? Instruction::SRAIW : Instruction::SRLIW;  
                    break;
                default: valid = false; break;
            }
            // clang-format on
            break;

        case InstructionType::BRANCH:
            // clang-format off
            switch (funct3_)
            {
                case 0b000: instr_ = Instruction::BEQ;   break;
                case 0b001: instr_ = Instruction::BNE;   break;
                case 0b100: instr_ = Instruction::BLT;   break;
                case 0b101: instr_ = Instruction::BGE;   break;
                case 0b110: instr_ = Instruction::BLTU;  break;
                case 0b111: instr_ = Instruction::BGEU;  break;
                default: valid = false; break;
            }
            // clang-format on
            break;

        case InstructionType::MISC_MEM:
            // clang-format off
            if (opcode_ == 0x8330000f)       instr_ = Instruction::FENCE_TSO;
            else if (opcode_ == 0x0100000f)  instr_ = Instruction::PAUSE;
            else if (funct3_ == 0x0)         instr_ = Instruction::FENCE;
            else                             valid = false;
            // clang-format on
            break;

        case InstructionType::SYSTEM:
            // clang-format off
            if (opcode_ == 0x00000073)       instr_ = Instruction::ECALL;
            else if (opcode_ == 0x00100073)  instr_ = Instruction::EBREAK;
            else                             valid = false;
            // clang-format on
            break;

        case InstructionType::LUI:
            instr_ = Instruction::LUI;
            break;

        case InstructionType::AUIPC:
            instr_ = Instruction::AUIPC;
            break;

        case InstructionType::JAL:
            instr_ = Instruction::JAL;
            break;

        case InstructionType::JALR:
            instr_ = Instruction::JALR;
            break;

        default:
            valid = false;
            break;
    }

    if (!valid)
    {
        type_ = InstructionType::INVALID;
        instr_ = Instruction::INVALID;
        auto error = std::format(
            "Failed to decode instruction 0x{:08x} at address 0x{:x}", opcode_, address_);
        throw std::runtime_error(error);
    }

    DecodeImmediate();
}
