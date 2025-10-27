#include "CPU.h"

#include <cassert>
#include <iostream>

void execute_J(CpuState *cpu_state, uint32_t instr)
{
    uint32_t current_pc = cpu_state->pc;
    uint32_t pc_high = (current_pc & 0xF0000000);
    uint32_t instr_index = get_bits(instr, 25, 0) << 2;

    uint32_t new_pc = pc_high | instr_index;
    cpu_state->pc = new_pc;
}

void execute_CLS(CpuState *cpu_state, uint32_t num)
{
    uint32_t rd = get_bits(num, 25, 21);
    uint32_t rs = get_bits(num, 20, 16);
    uint32_t value = cpu_state->gpr_regs[rs];

    uint32_t leading_ones = 0;
    for (int i = 31; i >= 0; i--)
    {
        if ((value >> i) & 1)
        {
            leading_ones++;
        }
        else
        {
            break;
        }
    }
    cpu_state->gpr_regs[rd] = leading_ones;
}

void execute_BEQ(CpuState *cpu_state, uint32_t num)
{
    uint32_t offset = get_bits(num, 15, 0);
    int32_t target = sign_extend(offset << 2, 18);

    uint32_t rs = get_bits(num, 25, 21);
    uint32_t rt = get_bits(num, 20, 16);

    bool cond = (cpu_state->gpr_regs[rs] == cpu_state->gpr_regs[rt]);

    if (cond)
    {
        cpu_state->pc = cpu_state->pc + target;
    }
}

void execute_OR(CpuState *cpu_state, uint32_t num)
{
    uint32_t rs = get_bits(num, 25, 21);
    uint32_t rt = get_bits(num, 20, 16);
    uint32_t rd = get_bits(num, 15, 11);

    cpu_state->gpr_regs[rd] = cpu_state->gpr_regs[rs] | cpu_state->gpr_regs[rt];
}

void execute_RORI(CpuState *cpu_state, uint32_t num)
{
    uint32_t rd = get_bits(num, 25, 21);
    uint32_t rs = get_bits(num, 20, 16);
    uint32_t value = cpu_state->gpr_regs[rs];
    int imm5 = get_bits(num, 15, 11);

    cpu_state->gpr_regs[rd] = rotate_right_32(value, imm5);
}

void execute_SELC(CpuState *cpu_state, uint32_t num)
{
    uint32_t rs1 = get_bits(num, 20, 16);
    uint32_t rs2 = get_bits(num, 15, 11);
    uint32_t rd = get_bits(num, 25, 21);

    uint32_t a = cpu_state->gpr_regs[rs1];
    uint32_t b = cpu_state->gpr_regs[rs2];

    uint32_t diff = a - b;
    uint32_t sign_bit = diff >> 31;

    cpu_state->gpr_regs[rd] = (a & ~sign_bit) | (b & sign_bit);
}

void execute_LDP(CpuState *cpu_state, uint32_t num)
{
    uint32_t rt1 = get_bits(num, 20, 16);
    uint32_t rt2 = get_bits(num, 15, 11);
    uint32_t offset = get_bits(num, 10, 0);
    uint32_t base = get_bits(num, 25, 21);

    uint32_t addr = cpu_state->gpr_regs[base] + sign_extend(offset << 2, 13);

    memcpy(&cpu_state->gpr_regs[rt1], cpu_state->data + addr, sizeof(uint32_t));
    memcpy(&cpu_state->gpr_regs[rt2], cpu_state->data + addr + 4, sizeof(uint32_t));
}

void execute_LD(CpuState *cpu_state, uint32_t instruction)
{
    const uint32_t base = get_bits(instruction, 25, 21);
    const uint32_t rt = get_bits(instruction, 20, 16);
    const uint32_t offset = get_bits(instruction, 15, 0);

    uint32_t address = cpu_state->gpr_regs[base] + sign_extend(offset, 16);

    memcpy(&cpu_state->gpr_regs[rt], cpu_state->data + address, sizeof(uint32_t));
}

void execute_SLTI(CpuState *cpu_state, uint32_t num)
{
    uint32_t rt = get_bits(num, 20, 16);
    uint32_t rs = get_bits(num, 25, 21);
    int32_t imm = sign_extend(get_bits(num, 15, 0), 16);
    int32_t rs_value = (cpu_state->gpr_regs[rs]);

    cpu_state->gpr_regs[rt] = (rs_value < imm) ? 1 : 0;
}

void execute_ST(CpuState *cpu_state, uint32_t num)
{
    uint32_t base = get_bits(num, 25, 21);
    uint32_t offset = get_bits(num, 15, 0);
    uint32_t rt = get_bits(num, 20, 16);

    uint32_t address = cpu_state->gpr_regs[base] + sign_extend(offset, 16);
    uint32_t value = cpu_state->gpr_regs[rt];

    memcpy(cpu_state->data + address, &value, sizeof(uint32_t));
}

void execute_BNE(CpuState *cpu_state, uint32_t instruction)
{
    const uint32_t rs = get_bits(instruction, 25, 21);
    const uint32_t rt = get_bits(instruction, 20, 16);
    const uint32_t offset = get_bits(instruction, 15, 0);

    const int32_t target = sign_extend(offset << 2, 18);

    const bool cond = (cpu_state->gpr_regs[rs] != cpu_state->gpr_regs[rt]);

    if (cond)
    {
        cpu_state->pc = cpu_state->pc + target;
    }
}

void execute_ADD(CpuState *cpu_state, uint32_t num)
{
    uint32_t rd = get_bits(num, 15, 11);
    uint32_t rs = get_bits(num, 25, 21);
    uint32_t rt = get_bits(num, 20, 16);

    std::cout << "  rs=" << rs << "(" << cpu_state->gpr_regs[rs] << "), rt=" << rt << "(" << cpu_state->gpr_regs[rt]
              << "), rd=" << rd << std::endl;

    cpu_state->gpr_regs[rd] = cpu_state->gpr_regs[rs] + cpu_state->gpr_regs[rt];

    std::cout << "  Result: reg[" << rd << "] = " << cpu_state->gpr_regs[rd] << std::endl;
}

void execute_USAT(CpuState *cpu_state, uint32_t num)
{
    uint32_t rd = get_bits(num, 25, 21);
    uint32_t rs = get_bits(num, 20, 16);
    uint32_t imm5 = get_bits(num, 15, 11);

    cpu_state->gpr_regs[rd] = saturate_unsigned(cpu_state->gpr_regs[rs], imm5);
}

void execute_SYSCALL(CpuState *cpu_state, uint32_t num)
{
    uint32_t syscall_num = cpu_state->gpr_regs[2];

    std::cout << "SYSCALL: num=" << syscall_num << std::endl;

    switch (syscall_num)
    {
    case SYSCALL_EXIT: {
        std::cout << "END" << std::endl;
        cpu_state->pc = 0xFFFFFFFF;
        break;
    }

    case SYSCALL_PRINT_INT: {
        int32_t value = cpu_state->gpr_regs[4];
        std::cout << value;
        break;
    }

    case SYSCALL_PRINT_CHAR: {
        char c = cpu_state->gpr_regs[4] & 0xFF;
        std::cout << c;
        break;
    }

    default: {
        break;
    }
    }
}