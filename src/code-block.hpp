#pragma once
#include "opcodes.hpp"
#include <string>
#include <vector>

namespace fox {

using Value = double;

struct CodeBlock {
    // name of the block is usually the 
    // same as the name of it's owning function.
    // so we just use a reference here.
    const std::string& name;
    std::vector<instruction> instructions{};
    std::vector<u32> lines{};
    std::vector<Value> constant_pool{};

    // adds the cosntant to the constant pool of the block
    // and returns the index at which it was added.
    u16 addConstant(Value constant);

    void addInstruction(instruction instr, u32 line);

    CodeBlock(const std::string& name) : name(name) {
    }
};
} // namespace fox