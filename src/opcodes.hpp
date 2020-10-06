#pragma once
#include "fox.hpp"
#include <array>
#include <string>

namespace fox {

enum Op {
    halt       = 0,
    load_const = 1,
    add        = 2,
    op__count,
};

static const std::array<std::string, op__count> opMap = {
    "HALT",
    "LOAD_CONST",
    "ADDR",
};

#define OP2S(o)   opMap[o]
#define OP2CS(o)  opMap[o].c_str()
#define OPNAME(o) opMap[opCode(o)]

using instruction = u32;

// decodes the opcode from the instruction
inline u8 opCode(instruction i) {
    return i >> 24;
}

// get register A
inline u8 rA(u32 i) {
    return (i & 0x00ff0000) >> 16;
}

// get register B
inline u8 rB(u32 i) {
    return (i & 0x0000ff00) >> 8;
}

// get register C
inline u8 rC(u32 i) {
    return (i & 0x000000ff);
}

// returns the constant index in the constant pool from an instruction of the
// format iAx (opcode-dest_register-index)

inline u16 get_constant_index(u32 i) {
    return (i & 0x0000ffff);
}

} // namespace fox