#pragma once
#include "code-block.hpp"
#include "opcodes.hpp"


namespace fox {

void printInstruction(instruction i, const CodeBlock& block);
void disassembleBlock(const CodeBlock& block);
} // namespace fox