#include <cstdint>
#include <fstream>
#include <iostream>
#include <memory>
#include <format>

#include "sim/cpu.hpp"

namespace
{

struct CLIOptions
{
    std::string hexFilePath;
    uint64_t memoryBase = 0;
    uint64_t memorySize = 0;
    uint64_t maxCycles = 10000;
    bool dumpRegistersAtExit = false;
    bool ignoreInvalidOps = false;
};

CLIOptions ParseCLI(int argc, char** argv)
{
    CLIOptions options;
    for (int index = 1; index < argc; index++)
    {
        if (std::string(argv[index]) == "-h" || std::string(argv[index]) == "--help")
        {
            std::cout << "Usage: " << argv[0] << " [options]\n"
                      << "Options:\n"
                      << "  -h, --help                Show this help message\n"
                      << "  -f, --hex <file>          Path to HEX file to load\n"
                      << "  -b, --base <addr>         Base address for memory (hex)\n"
                      << "  -s, --size <size>         Size of memory in bytes (hex)\n"
                      << "  -c, --cycles <num>        Maximum number of CPU cycles to execute\n"
                      << "  -i, --ignore-invalid-ops  Disable exceptions on invalid instructions\n"
                      << "  -d, --dump-regs-at-exit   Dump register values at exit\n";
            std::exit(0);
        }
        else if (std::string(argv[index]) == "-d" ||
                 std::string(argv[index]) == "--dump-regs-at-exit")
        {
            options.dumpRegistersAtExit = true;
        }
        else if (std::string(argv[index]) == "-i" ||
                 std::string(argv[index]) == "--ignore-invalid-ops")
        {
            options.ignoreInvalidOps = true;
        }
        else if ((std::string(argv[index]) == "-f" || std::string(argv[index]) == "--hex") &&
                 index + 1 < argc)
        {
            options.hexFilePath = argv[++index];
        }
        else if ((std::string(argv[index]) == "-b" || std::string(argv[index]) == "--base") &&
                 index + 1 < argc)
        {
            options.memoryBase = std::stoull(argv[++index], nullptr, 16);
        }
        else if ((std::string(argv[index]) == "-s" || std::string(argv[index]) == "--size") &&
                 index + 1 < argc)
        {
            options.memorySize = std::stoull(argv[++index], nullptr, 16);
        }
        else if ((std::string(argv[index]) == "-c" || std::string(argv[index]) == "--cycles") &&
                 index + 1 < argc)
        {
            options.maxCycles = std::stoull(argv[++index]);
        }
        else
        {
            std::cerr << "Unknown option: " << argv[index]
                      << "\nUse -h or --help for usage information.\n";
            std::exit(1);
        }
    }
    return options;
}

}  // namespace

int main(int argc, char** argv)
{
    auto options = ParseCLI(argc, argv);
    if (options.hexFilePath.empty())
    {
        std::cerr << "No HEX file specified.\n";
        std::exit(1);
    }

    uint64_t memoryBase = options.memoryBase ? options.memoryBase : 0;  // Start of address space
    uint64_t memorySize = options.memorySize ? options.memorySize : 32 * 1024 * 1024;  // 32MB
    std::unique_ptr<uint8_t[]> memory(new uint8_t[memorySize]);
    {
        std::ifstream hexFile(options.hexFilePath);
        if (!hexFile)
        {
            std::cerr << "Failed to open HEX file: " << options.hexFilePath << std::endl;
            std::exit(1);
        }
        hexFile.seekg(0, std::ios::end);
        size_t len = hexFile.tellg();
        hexFile.seekg(0, std::ios::beg);
        if (len > memorySize) std::cerr << "WARN: HEX file size exceeds allocated memory size.\n";
        auto loadSize = std::min(len, memorySize);
        hexFile.read(reinterpret_cast<char*>(memory.get()), loadSize);
        std::cout << std::format("Loaded {} bytes of the HEX file\n", loadSize);
    }

    auto loadByte = [&](uint64_t addr) -> uint8_t
    {
        if (addr < memoryBase || addr >= memoryBase + memorySize)
            throw std::out_of_range("Memory access out of range");
        return memory[addr - memoryBase];
    };
    auto storeByte = [&](uint64_t addr, uint8_t value)
    {
        if (addr < memoryBase || addr >= memoryBase + memorySize)
            throw std::out_of_range("Memory access out of range");
        memory[addr - memoryBase] = value;
    };

    Sim::CPU cpu;
    cpu.Reset();
    cpu.SetPC(options.memoryBase);
    cpu.SetLoadByteFunction(loadByte);
    cpu.SetStoreByteFunction(storeByte);

    for (uint64_t i = 0; i < options.maxCycles || !options.maxCycles; ++i)
    {
        try
        {
            cpu.Step();
        }
        catch (const std::exception& e)
        {
            if (options.ignoreInvalidOps)
            {
                std::cerr << "CPU halted with error: " << e.what() << std::endl;
            }
            else
            {
                std::cerr << "Invalid instruction: " << e.what() << std::endl;
                break;
            }
        }
    }

    if (options.dumpRegistersAtExit)
    {
        std::cout << cpu.RegsString();
    }

    return 0;
}
