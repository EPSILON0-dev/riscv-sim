#include "regs.hpp"

#include <format>
#include <stdexcept>

using namespace Sim;

// clang-format off
const std::array<std::string, 32> Regs::regNames = {
    "zero", "ra",   "sp",   "gp",   "tp",   "t0",   "t1",   "t2",
    "s0",   "s1",   "a0",   "a1",   "a2",   "a3",   "a4",   "a5",
    "a6",   "a7",   "s2",   "s3",   "s4",   "s5",   "s6",   "s7",
    "s8",   "s9",   "s10",  "s11",  "t3",   "t4",   "t5",   "t6"
};
// clang-format on

void Regs::Write(Reg reg, uint64_t value)
{
    Write(static_cast<size_t>(reg), value);
}

uint64_t Regs::Read(Reg reg) const
{
    return Read(static_cast<size_t>(reg));
}

void Regs::Write(size_t reg, uint64_t value)
{
    if (reg >= regs_.size()) throw std::out_of_range("Register index out of range");
    if (reg != 0) regs_[reg] = value;
}

uint64_t Regs::Read(size_t reg) const
{
    if (reg >= regs_.size()) throw std::out_of_range("Register index out of range");
    if (reg == 0) return 0;
    return regs_[reg];
}

void Regs::Reset()
{
    regs_.fill(0);
}

const std::string& Regs::NameOf(Reg reg)
{
    if (static_cast<size_t>(reg) >= regNames.size())
        throw std::out_of_range("Register index out of range");
    return regNames[static_cast<size_t>(reg)];
}

std::string Regs::ToString() const
{
    std::string output;
    for (int i = 0; i < 32; i++)
    {
        const auto reg = static_cast<Reg>(i);
        output += std::format("{:5} 0x{:016x}{}", NameOf(reg), Read(reg), i % 2 == 1 ? "\n" : ",  ");
    }
    return output;
}
