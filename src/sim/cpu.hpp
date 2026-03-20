#pragma once

#include <cstdint>
#include <functional>
#include <string>

#include "operation.hpp"
#include "regs.hpp"

namespace Sim
{

class CPU
{
   public:
    using LoadFunc = std::function<uint8_t(uint64_t address)>;
    using StoreFunc = std::function<void(uint64_t address, uint8_t value)>;

   private:
    Regs regs_;
    uint64_t pc_;
    LoadFunc loadByteFunc_;
    StoreFunc storeByteFunc_;

   private:
    static uint64_t SignExtend8(uint64_t value);
    static uint64_t SignExtend16(uint64_t value);
    static uint64_t SignExtend32(uint64_t value);

    static void CheckAlignment(uint64_t address, uint8_t alignment);

    uint64_t LoadMemoryDoubleWord(uint64_t address);
    uint64_t LoadMemoryWord(uint64_t address);
    uint64_t LoadMemoryHalfWord(uint64_t address);
    uint64_t LoadMemoryByte(uint64_t address);

    void StoreMemoryDoubleWord(uint64_t address, uint64_t value);
    void StoreMemoryWord(uint64_t address, uint64_t value);
    void StoreMemoryHalfWord(uint64_t address, uint64_t value);
    void StoreMemoryByte(uint64_t address, uint64_t value);

    void ExecuteLoad(const Operation& op);
    void ExecuteStore(const Operation& op);
    void ExecuteArithmetic(const Operation& op);
    void ExecuteBranch(const Operation& op);
    void ExecuteJump(const Operation& op);
    void ExecuteUpper(const Operation& op);
    void ExecuteMiscMem(const Operation& op);
    void ExecuteSystem(const Operation& op);

    void ExecuteOperation(const Operation& op);

   public:
    CPU() = default;
    ~CPU() = default;

    void SetLoadByteFunction(LoadFunc func) { loadByteFunc_ = std::move(func); }
    void SetStoreByteFunction(StoreFunc func) { storeByteFunc_ = std::move(func); }

    void Reset();
    void SetPC(uint64_t pc) { pc_ = pc; }
    void Step();

    std::string RegsString() const;
};

}  // namespace Sim
