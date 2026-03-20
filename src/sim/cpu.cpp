#include "cpu.hpp"

#include <format>
#include <stdexcept>

using namespace Sim;

uint64_t CPU::SignExtend8(uint64_t value)
{
    return (value & 0x80) ? (value | 0xFFFFFFFFFFFFFF00) : value;
}

uint64_t CPU::SignExtend16(uint64_t value)
{
    return (value & 0x8000) ? (value | 0xFFFFFFFFFFFF0000) : value;
}

uint64_t CPU::SignExtend32(uint64_t value)
{
    return (value & 0x80000000) ? (value | 0xFFFFFFFF00000000) : value;
}

void CPU::CheckAlignment(uint64_t address, uint8_t alignment)
{
    if (address % alignment != 0)
    {
        throw std::runtime_error("Misaligned memory access");
    }
}

uint64_t CPU::LoadMemoryDoubleWord(uint64_t address)
{
    CheckAlignment(address, 8);
    return (static_cast<uint64_t>(loadByteFunc_(address)) << 0) |
           (static_cast<uint64_t>(loadByteFunc_(address + 1)) << 8) |
           (static_cast<uint64_t>(loadByteFunc_(address + 2)) << 16) |
           (static_cast<uint64_t>(loadByteFunc_(address + 3)) << 24) |
           (static_cast<uint64_t>(loadByteFunc_(address + 4)) << 32) |
           (static_cast<uint64_t>(loadByteFunc_(address + 5)) << 40) |
           (static_cast<uint64_t>(loadByteFunc_(address + 6)) << 48) |
           (static_cast<uint64_t>(loadByteFunc_(address + 7)) << 56);
}

uint64_t CPU::LoadMemoryWord(uint64_t address)
{
    CheckAlignment(address, 4);
    return (static_cast<uint64_t>(loadByteFunc_(address)) << 0) |
           (static_cast<uint64_t>(loadByteFunc_(address + 1)) << 8) |
           (static_cast<uint64_t>(loadByteFunc_(address + 2)) << 16) |
           (static_cast<uint64_t>(loadByteFunc_(address + 3)) << 24);
}

uint64_t CPU::LoadMemoryHalfWord(uint64_t address)
{
    CheckAlignment(address, 2);
    return (static_cast<uint64_t>(loadByteFunc_(address)) << 0) |
           (static_cast<uint64_t>(loadByteFunc_(address + 1)) << 8);
}

uint64_t CPU::LoadMemoryByte(uint64_t address)
{
    CheckAlignment(address, 1);
    return (static_cast<uint64_t>(loadByteFunc_(address)) << 0);
}

void CPU::StoreMemoryDoubleWord(uint64_t address, uint64_t value)
{
    CheckAlignment(address, 8);
    storeByteFunc_(address, value & 0xFF);
    storeByteFunc_(address + 1, (value >> 8) & 0xFF);
    storeByteFunc_(address + 2, (value >> 16) & 0xFF);
    storeByteFunc_(address + 3, (value >> 24) & 0xFF);
    storeByteFunc_(address + 4, (value >> 32) & 0xFF);
    storeByteFunc_(address + 5, (value >> 40) & 0xFF);
    storeByteFunc_(address + 6, (value >> 48) & 0xFF);
    storeByteFunc_(address + 7, (value >> 56) & 0xFF);
}

void CPU::StoreMemoryWord(uint64_t address, uint64_t value)
{
    CheckAlignment(address, 4);
    storeByteFunc_(address, value & 0xFF);
    storeByteFunc_(address + 1, (value >> 8) & 0xFF);
    storeByteFunc_(address + 2, (value >> 16) & 0xFF);
    storeByteFunc_(address + 3, (value >> 24) & 0xFF);
}

void CPU::StoreMemoryHalfWord(uint64_t address, uint64_t value)
{
    CheckAlignment(address, 2);
    storeByteFunc_(address, value & 0xFF);
    storeByteFunc_(address + 1, (value >> 8) & 0xFF);
}

void CPU::StoreMemoryByte(uint64_t address, uint64_t value)
{
    CheckAlignment(address, 1);
    storeByteFunc_(address, value & 0xFF);
}

void CPU::ExecuteLoad(const Operation& op)
{
    uint64_t value = 0, address = regs_.Read(op.Rs1()) + op.Immediate();

    switch (op.Instruction())
    {
        case Operation::Instruction::LB:
            value = LoadMemoryByte(address);
            value = SignExtend8(value);
            break;

        case Operation::Instruction::LH:
            value = LoadMemoryHalfWord(address);
            value = SignExtend16(value);
            break;

        case Operation::Instruction::LW:
            value = LoadMemoryWord(address);
            value = SignExtend32(value);
            break;

        case Operation::Instruction::LD:
            value = LoadMemoryDoubleWord(address);
            break;

        case Operation::Instruction::LBU:
            value = LoadMemoryByte(address);
            break;

        case Operation::Instruction::LHU:
            value = LoadMemoryHalfWord(address);
            break;

        case Operation::Instruction::LWU:
            value = LoadMemoryWord(address);
            break;

        default:
            throw std::runtime_error("Unsupported load instruction");
            break;
    }

    regs_.Write(op.Rd(), value);
}

void CPU::ExecuteStore(const Operation& op)
{
    uint64_t value = regs_.Read(op.Rs2());
    uint64_t address = regs_.Read(op.Rs1()) + op.Immediate();

    switch (op.Instruction())
    {
        case Operation::Instruction::SB:
            StoreMemoryByte(address, value);
            break;

        case Operation::Instruction::SH:
            StoreMemoryHalfWord(address, value);
            break;

        case Operation::Instruction::SW:
            StoreMemoryWord(address, value);
            break;

        case Operation::Instruction::SD:
            StoreMemoryDoubleWord(address, value);
            break;

        default:
            throw std::runtime_error("Unsupported store instruction");
            break;
    }
}

void CPU::ExecuteArithmetic(const Operation& op)
{
    bool isImmediate = (op.Type() == Operation::InstructionType::OP_IMM ||
                        op.Type() == Operation::InstructionType::OP_IMM_32);
    uint64_t value1 = regs_.Read(op.Rs1());
    uint64_t value2 = isImmediate ? op.Immediate() : regs_.Read(op.Rs2());
    uint64_t result = 0;

    switch (op.Instruction())
    {
        case Operation::Instruction::ADDI:
        case Operation::Instruction::ADD:
            result = value1 + value2;
            break;

        case Operation::Instruction::SUB:
            result = value1 - value2;
            break;

        case Operation::Instruction::ADDIW:
        case Operation::Instruction::ADDW:
            result = static_cast<uint32_t>(value1) + static_cast<uint32_t>(value2);
            result = SignExtend32(result);
            break;

        case Operation::Instruction::SUBW:
            result = static_cast<uint32_t>(value1) - static_cast<uint32_t>(value2);
            result = SignExtend32(result);
            break;

        case Operation::Instruction::SLTI:
        case Operation::Instruction::SLT:
            result = (static_cast<int64_t>(value1) < static_cast<int64_t>(value2)) ? 1 : 0;
            break;

        case Operation::Instruction::SLTIU:
        case Operation::Instruction::SLTU:
            result = (static_cast<uint64_t>(value1) < static_cast<uint64_t>(value2)) ? 1 : 0;
            break;

        case Operation::Instruction::XORI:
        case Operation::Instruction::XOR:
            result = value1 ^ value2;
            break;

        case Operation::Instruction::ORI:
        case Operation::Instruction::OR:
            result = value1 | value2;
            break;

        case Operation::Instruction::ANDI:
        case Operation::Instruction::AND:
            result = value1 & value2;
            break;

        case Operation::Instruction::SLLI:
        case Operation::Instruction::SLL:
            result = value1 << (value2 & 0x3F);
            break;

        case Operation::Instruction::SLLIW:
        case Operation::Instruction::SLLW:
            result = static_cast<uint32_t>(value1) << (value2 & 0x1F);
            result = SignExtend32(result);
            break;

        case Operation::Instruction::SRLI:
        case Operation::Instruction::SRL:
            result = static_cast<uint64_t>(value1) >> (value2 & 0x3F);
            break;

        case Operation::Instruction::SRLIW:
        case Operation::Instruction::SRLW:
            result = static_cast<uint32_t>(value1) >> (value2 & 0x1F);
            result = SignExtend32(result);
            break;

        case Operation::Instruction::SRAI:
        case Operation::Instruction::SRA:
            result = static_cast<int64_t>(value1) >> (value2 & 0x3F);
            break;

        case Operation::Instruction::SRAIW:
        case Operation::Instruction::SRAW:
            result = static_cast<int32_t>(value1) >> (value2 & 0x1F);
            result = SignExtend32(result);
            break;

        default:
            throw std::runtime_error("Unsupported arithmetic instruction");
            break;
    }

    regs_.Write(op.Rd(), result);
}

void CPU::ExecuteBranch(const Operation& op)
{
    uint64_t value1 = regs_.Read(op.Rs1());
    uint64_t value2 = regs_.Read(op.Rs2());
    bool takeBranch = false;

    switch (op.Instruction())
    {
        case Operation::Instruction::BEQ:
            takeBranch = (value1 == value2);
            break;

        case Operation::Instruction::BNE:
            takeBranch = (value1 != value2);
            break;

        case Operation::Instruction::BLT:
            takeBranch = (static_cast<int64_t>(value1) < static_cast<int64_t>(value2));
            break;

        case Operation::Instruction::BGE:
            takeBranch = (static_cast<int64_t>(value1) >= static_cast<int64_t>(value2));
            break;

        case Operation::Instruction::BLTU:
            takeBranch = (value1 < value2);
            break;

        case Operation::Instruction::BGEU:
            takeBranch = (value1 >= value2);
            break;

        default:
            throw std::runtime_error("Unsupported branch instruction");
            break;
    }

    if (takeBranch) pc_ += op.Immediate() - 4;
}

void CPU::ExecuteJump(const Operation& op)
{
    switch (op.Instruction())
    {
        case Operation::Instruction::JAL:
            regs_.Write(op.Rd(), pc_);
            pc_ += op.Immediate() - 4;
            break;

        case Operation::Instruction::JALR:
        {
            uint64_t prevPc = pc_;
            pc_ = (regs_.Read(op.Rs1()) + op.Immediate()) & ~static_cast<uint64_t>(1);
            regs_.Write(op.Rd(), prevPc);
        }
        break;

        default:
            throw std::runtime_error("Unsupported jump instruction");
            break;
    }
}

void CPU::ExecuteUpper(const Operation& op)
{
    switch (op.Instruction())
    {
        case Operation::Instruction::LUI:
            regs_.Write(op.Rd(), op.Immediate());
            break;

        case Operation::Instruction::AUIPC:
            regs_.Write(op.Rd(), pc_ + op.Immediate() - 4);
            break;

        default:
            throw std::runtime_error("Unsupported upper instruction");
            break;
    }
}

void CPU::ExecuteMiscMem(const Operation& op)
{
    (void)regs_;

    switch (op.Instruction())
    {
        case Operation::Instruction::FENCE:
        case Operation::Instruction::FENCE_TSO:
            // TODO: Treated as HINTs for now
            break;

        default:
            throw std::runtime_error("Unsupported misc mem instruction");
            break;
    }
}

void CPU::ExecuteSystem(const Operation& op)
{
    (void)regs_;

    switch (op.Instruction())
    {
        case Operation::Instruction::EBREAK:
        case Operation::Instruction::ECALL:
        case Operation::Instruction::PAUSE:
            // TODO: Treated as HINTs for now
            break;

        default:
            throw std::runtime_error("Unsupported system instruction");
            break;
    }
}

void CPU::ExecuteOperation(const Operation& op)
{
    switch (op.Type())
    {
        case Operation::InstructionType::LOAD:
            ExecuteLoad(op);
            break;

        case Operation::InstructionType::STORE:
            ExecuteStore(op);
            break;

        case Operation::InstructionType::OP_IMM:
        case Operation::InstructionType::OP_IMM_32:
        case Operation::InstructionType::OP:
        case Operation::InstructionType::OP_32:
            ExecuteArithmetic(op);
            break;

        case Operation::InstructionType::BRANCH:
            ExecuteBranch(op);
            break;

        case Operation::InstructionType::JALR:
        case Operation::InstructionType::JAL:
            ExecuteJump(op);
            break;

        case Operation::InstructionType::LUI:
        case Operation::InstructionType::AUIPC:
            ExecuteUpper(op);
            break;

        case Operation::InstructionType::MISC_MEM:
            ExecuteMiscMem(op);
            break;

        case Operation::InstructionType::SYSTEM:
            ExecuteSystem(op);
            break;

        default:
            auto error = std::format("Unsupported instruction type {} at address 0x{:x}",
                Operation::NameOf(op.Type()), op.Address());
            throw std::runtime_error(error);
            break;
    }
}

void CPU::Reset()
{
    pc_ = 0;
    regs_.Reset();
}

void CPU::Step()
{
    uint64_t address = pc_;
    uint32_t opcode = LoadMemoryWord(address);
    pc_ += 4;
    Operation op(address, opcode);
    op.Decode();
    ExecuteOperation(op);
}

std::string CPU::RegsString() const
{
    return regs_.ToString();
}
