#include "CPU.h"
#include <iostream>

void execute(CpuState *cpu_state, Decode *decode_)
{
    uint32_t original_pc = cpu_state->pc;

    switch (decode_->opcode)
    {
    case Opcodes::J: {
        std::cout << "  Calling execute_J" << std::endl;
        execute_J(cpu_state, decode_->num);
        break;
    }
    case Opcodes::CLS: {
        decode_->funct = get_bits(decode_->num, 5, 0);
        std::cout << "  CLS opcode, funct=0x" << std::hex << decode_->funct << std::dec << std::endl;

        if (decode_->funct == CLS_LAST)
        {
            std::cout << "  Calling execute_CLS" << std::endl;
            execute_CLS(cpu_state, decode_->num);
        }
        else if (decode_->funct == OR_LAST)
        {
            std::cout << "  Calling execute_OR" << std::endl;
            execute_OR(cpu_state, decode_->num);
        }
        else if (decode_->funct == SELC_LAST)
        {
            std::cout << "  Calling execute_SELC" << std::endl;
            execute_SELC(cpu_state, decode_->num);
        }
        else if (decode_->funct == SYSCALL_LAST)
        {
            std::cout << "  SYSCALL detected" << std::endl;
            execute_SYSCALL(cpu_state, decode_->num);
        }
        else if (decode_->funct == ADD_LAST)
        {
            std::cout << "  Calling execute_ADD" << std::endl;
            execute_ADD(cpu_state, decode_->num);
        }
        else
        {
            std::cout << "  Unknown funct code: 0x" << std::hex << decode_->funct << std::dec << std::endl;
        }
        break;
    }
    case Opcodes::BEQ: {
        std::cout << "  Calling execute_BEQ" << std::endl;
        execute_BEQ(cpu_state, decode_->num);
        break;
    }
    case Opcodes::RORI: {
        std::cout << "  Calling execute_RORI" << std::endl;
        execute_RORI(cpu_state, decode_->num);
        break;
    }
    case Opcodes::LDP: {
        std::cout << "  Calling execute_LDP" << std::endl;
        execute_LDP(cpu_state, decode_->num);
        break;
    }
    case Opcodes::LD: {
        std::cout << "  Calling execute_LD" << std::endl;
        execute_LD(cpu_state, decode_->num);
        break;
    }
    case Opcodes::SLTI: {
        std::cout << "  Calling execute_SLTI" << std::endl;
        execute_SLTI(cpu_state, decode_->num);
        break;
    }
    case Opcodes::ST: {
        std::cout << "  Calling execute_ST" << std::endl;
        execute_ST(cpu_state, decode_->num);
        break;
    }
    case Opcodes::BNE: {
        std::cout << "  Calling execute_BNE" << std::endl;
        execute_BNE(cpu_state, decode_->num);
        break;
    }
    case Opcodes::USAT: {
        std::cout << "  Calling execute_USAT" << std::endl;
        execute_USAT(cpu_state, decode_->num);
        break;
    }
    default: {
        std::cout << "  Unknown opcode: 0x" << std::hex << decode_->opcode << std::dec << std::endl;
        break;
    }
    }
    if (cpu_state->pc == original_pc)
    {
        cpu_state->pc += 4;
    }
    std::cout << "  New PC: 0x" << std::hex << cpu_state->pc << std::dec << std::endl;
}
