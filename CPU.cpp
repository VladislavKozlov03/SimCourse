#include "CPU.h"

#include <cassert>
#include <cstring>
#include <iomanip>
#include <iostream>

void dump_registers(CpuState *cpu_state, const std::string &description)
{
    std::cout << "=== " << description << " ===" << std::endl;
    std::cout << "PC: 0x" << std::hex << std::setw(8) << std::setfill('0') << cpu_state->pc << std::dec << std::endl;
    std::cout << " REGISTERS:" << std::endl;

    for (int row = 0; row < 8; row++)
    {
        for (int col = 0; col < 4; col++)
        {
            int reg_num = row + col * 8;
            std::cout << std::setw(2) << std::setfill('0') << reg_num << ": " << std::setw(10) << std::setfill(' ')
                      << cpu_state->gpr_regs[reg_num];
            if (col < 3)
                std::cout << " | ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void ctor(CpuState *cpu_state, size_t number_of_instructions)
{
    for (size_t i = 0; i < registr; i++)
    {
        cpu_state->gpr_regs[i] = 0;
    }

    cpu_state->pc = 0;
    cpu_state->data = (uint8_t *)calloc(number_of_instructions * sizeof(Instruction), 1);
}

uint32_t get_bits(uint32_t num, size_t high_bit, size_t low_bit)
{
    assert(high_bit >= low_bit);
    assert(high_bit < 32);

    uint32_t mask = ((1u << (high_bit - low_bit + 1)) - 1) << low_bit;
    return (num & mask) >> low_bit;
}

int32_t sign_extend(int value, int original_bits)
{
    int shift = 32 - original_bits;
    return (value << shift) >> shift;
}

uint32_t rotate_right_32(uint32_t value, int imm5)
{
    return (value >> imm5) | (value << (32 - imm5));
}

uint32_t saturate_unsigned(uint32_t value, int imm)
{
    uint32_t max_value = (1 << imm) - 1;
    return (value > max_value) ? max_value : value;
}

uint32_t swap_endian(uint32_t value)
{
    return ((value & 0xFF000000) >> 24) | ((value & 0x00FF0000) >> 8) | ((value & 0x0000FF00) << 8) |
           ((value & 0x000000FF) << 24);
}
