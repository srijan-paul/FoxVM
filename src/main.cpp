#include "fox.hpp"
#include <array>
#include <cassert>
#include <iostream>
#include <stdint.h>
#include <string>
#include <vector>

namespace fox {

// types

using s32 = int32_t;
using u32 = uint32_t;
using u8 = uint8_t;

using Value = double;

// opcode.cpp

enum Op {
    halt = 0x00,
    load = 0x01,
    op__count,
};

static const std::array<std::string, op__count> opMap = {
    "HALT",
    "LOAD",
};

#define OPCODE(i) ((i) >> 24)
#define RA(i) ((i) >> 16) & 0xff

// vm.cpp

using instruction = u32;

struct CodeBlock {
    const std::vector<instruction> instructions;
    const std::vector<Value> constants;
};

enum class ExitCode {
    success,
    error,
};

class VM {
  public:
    VM(CodeBlock& block) : block(block) {
        for (u8 i = 0; i < REGISTER_COUNT; i++) {
            registers[i] = 0;
        }
    }

    ExitCode run() {
        while (true) {
            const instruction instr = next();
            const u8 code = OPCODE(instr);

            switch (code) {
            case Op::halt:
                std::cout << "HALT\n";
                return ((ip == block.instructions.size()) ? ExitCode::success
                                                          : ExitCode::error);
            case Op::load: {
                u8 reg = RA(instr);
                std::cout << "LOAD R[" << (int)reg << "]" << std::endl;
                break;
            }
            default:
                std::cout << "Error: Unknown Opcode " << (int)instr
                          << std::endl;
                return ExitCode::error;
            }
        }
    }

  private:
    static u8 constexpr REGISTER_COUNT = 32;
    std::array<Value, REGISTER_COUNT> registers;
    const CodeBlock& block;
    u32 ip = 0;

    inline instruction next() {
        return block.instructions[ip++];
    }
};

} // namespace fox

using namespace fox;

void testOpcode() {
    assert(OPCODE(1 << 24) == Op::load);
    assert(OPCODE(0) == Op::halt);
}

void testVM() {
    CodeBlock block{{1 << 24, 0}, {12}};
    VM vm{block};
    ExitCode result = vm.run();
    assert(result == ExitCode::success);
}

int main() {
    std::cout << "--Fox VM--" << std::endl;
    testOpcode();
    testVM();
    return 0;
}