#ifndef CPU_H
#define CPU_H

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <string>

typedef uint32_t Register;
typedef uint32_t Instruction;

const size_t registr = 32;
const size_t memory_size = 1024 * 1024;

enum Opcodes
{
    J = 0b010000,
    CLS = 0b000000,
    BEQ = 0b001011,
    OR = 0b000000,
    RORI = 0b100011,
    SELC = 0b000000,
    SYSCALL = 0b000000,
    LDP = 0b000010,
    LD = 0b101100,
    SLTI = 0b110011,
    ST = 0b000110,
    BNE = 0b001100,
    ADD = 0b000000,
    USAT = 0b100110,
};

enum Opcodes_Last_5Bits
{
    CLS_LAST = 0b101010,
    OR_LAST = 0b111001,
    SELC_LAST = 0b011010,
    SYSCALL_LAST = 0b110001,
    ADD_LAST = 0b110010,
};

enum Syscalls
{
    SYSCALL_EXIT = 0,
    SYSCALL_PRINT_INT = 1,
    SYSCALL_PRINT_STRING = 2,
    SYSCALL_READ_INT = 3,
    SYSCALL_READ_STRING = 4,
    SYSCALL_PRINT_CHAR = 5
};

struct CpuState
{
    Register pc;
    Register gpr_regs[registr];
    uint8_t *data;
};

struct Decode
{
    uint32_t opcode;
    uint32_t num;
    uint32_t funct;
};

void dump_registers(CpuState *cpu_state, const std::string &description);
void ctor(CpuState *cpu_state, size_t number_of_instructions);
uint32_t get_bits(uint32_t num, size_t high_bit, size_t low_bit);
int32_t sign_extend(int value, int original_bits);
uint32_t rotate_right_32(uint32_t value, int imm5);
uint32_t saturate_unsigned(uint32_t value, int imm);
uint32_t swap_endian(uint32_t value);

void execute_J(CpuState *cpu_state, uint32_t instr);
void execute_CLS(CpuState *cpu_state, uint32_t num);
void execute_BEQ(CpuState *cpu_state, uint32_t num);
void execute_OR(CpuState *cpu_state, uint32_t num);
void execute_RORI(CpuState *cpu_state, uint32_t num);
void execute_SELC(CpuState *cpu_state, uint32_t num);
void execute_LDP(CpuState *cpu_state, uint32_t num);
void execute_LD(CpuState *cpu_state, uint32_t instruction);
void execute_SLTI(CpuState *cpu_state, uint32_t num);
void execute_ST(CpuState *cpu_state, uint32_t num);
void execute_BNE(CpuState *cpu_state, uint32_t instruction);
void execute_ADD(CpuState *cpu_state, uint32_t num);
void execute_USAT(CpuState *cpu_state, uint32_t num);
void execute_SYSCALL(CpuState *cpu_state, uint32_t num);

void decode(CpuState *cpu_state, Decode *decode_);
void execute(CpuState *cpu_state, Decode *decode_);

#endif
