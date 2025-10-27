#include "CPU.h"
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>

#include "CLI/CLI.hpp"

int main(int argc, char **argv)
{
    CLI::App app{"My simulator"};
    std::string elf_file;
    app.add_option("-f,--file", elf_file, "required elf file")->required()->check(CLI::ExistingFile);

    CLI11_PARSE(app, argc, argv);

    CpuState cpu_state;
    Decode decode_;

    ctor(&cpu_state, memory_size);

    std::cout << "=== Fibonacci ===" << std::endl;

    std::ifstream file(elf_file, std::ios::binary | std::ios::ate);

    std::streamsize file_size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::cout << "File size: " << file_size << " bytes" << std::endl;

    if (!file.read(reinterpret_cast<char *>(cpu_state.data), file_size))
    {
        std::cerr << "Error: Failed to read file" << std::endl;
        return 1;
    }
    file.close();

    int n = 7;

    int instruction_count = 0;
    const int MAX_INSTRUCTIONS = 100;

    cpu_state.pc = 0;

    while (cpu_state.pc < memory_size && instruction_count < MAX_INSTRUCTIONS)
    {
        uint32_t old_pc = cpu_state.pc;

        decode(&cpu_state, &decode_);
        execute(&cpu_state, &decode_);
        instruction_count++;

        if (cpu_state.pc == old_pc)
        {
            break;
        }

        if (cpu_state.pc >= file_size)
        {
            std::cout << "end of program" << std::endl;
            break;
        }
    }

    std::cout << "instr count: " << instruction_count << std::endl;

    std::cout << "\nregisters!:" << std::endl;
    for (int i = 0; i < 32; i++)
    {
        if (cpu_state.gpr_regs[i] != 0 || i == 2)
        {
            std::cout << "  reg[" << i << "] = " << cpu_state.gpr_regs[i] << std::endl;
        }
    }

    int expected;
    if (n == 0)
        expected = 1;
    else if (n == 1)
        expected = 1;
    else
    {
        int a = 0, b = 1;
        for (int i = 2; i <= n; i++)
        {
            int temp = a + b;
            a = b;
            b = temp;
        }
        expected = b;
    }

    std::cout << "true answer " << expected << std::endl;

    free(cpu_state.data);
    return 0;
}