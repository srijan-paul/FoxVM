#include "code-block.hpp"

namespace fox {

u16 CodeBlock::addConstant(Value constant) {
    constant_pool.push_back(constant);
    return constant_pool.size() - 1;
}

void CodeBlock::addInstruction(instruction instr, u32 line) {
    instructions.push_back(instr);
    lines.push_back(line);
}

} // namespace fox