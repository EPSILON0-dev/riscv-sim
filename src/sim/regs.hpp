#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>

namespace Sim
{

class Regs
{
   public:
    // clang-format off
    enum class Reg : uint8_t
    {
        ZERO, RA,   SP,   GP,   TP,   T0,   T1,   T2,
        S0,   S1,   A0,   A1,   A2,   A3,   A4,   A5,
        A6,   A7,   S2,   S3,   S4,   S5,   S6,   S7,
        S8,   S9,   S10,  S11,  T3,   T4,   T5,   T6
    };
    // clang-format on

   private:
   static const std::array<std::string, 32> regNames;
    std::array<uint64_t, 32> regs_;

   public:
    Regs() = default;
    ~Regs() = default;

   public:
    void Write(Reg reg, uint64_t value);
    uint64_t Read(Reg reg) const;
    void Write(size_t reg, uint64_t value);
    uint64_t Read(size_t reg) const;
    
    void Reset();

    static const std::string& NameOf(Reg reg);
    std::string ToString() const;
};

}  // namespace Sim
