#include "CPU.h"
#include <iostream>

void decode(CpuState *cpu_state, Decode *decode_)
{
    uint32_t num = 0;
    memcpy(&num, cpu_state->data + cpu_state->pc, sizeof(uint32_t));

    decode_->num = swap_endian(num);
    decode_->opcode = get_bits(decode_->num, 31, 26);
    decode_->funct = get_bits(decode_->num, 5, 0);

    std::cout << "DECODE: PC=0x" << std::hex << cpu_state->pc << " Instruction=0x" << decode_->num << " Opcode=0x"
              << decode_->opcode << std::dec << std::endl;
}