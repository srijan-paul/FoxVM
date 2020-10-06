#include "debug.hpp"
#include "opcodes.hpp"
#include <cstdio>

namespace fox {

static void print_iABC(instruction i) {
    printf("%-16s    %03d    %03d    %03d\n", OP2CS(opCode(i)), rA(i), rB(i),
           rC(i));
}

static void print_iAx(instruction i, const CodeBlock& block) {
    u16 index = get_constant_index(i);
    printf("%-16s    %03d    %03d(%.2f)\n", OP2CS(opCode(i)), rA(i), index,
           block.constant_pool[index]);
}

void printInstruction(instruction i, const CodeBlock& block) {
    u8 op = opCode(i);

    // TODO: remove magic numbers
    switch (op) {
    case Op::load_const:
        print_iAx(i, block);
        return;
    case Op::add:
        print_iABC(i);
        return;
    case Op::halt:
        printf("HALT\n");
        return;
    }
}

void disassembleBlock(const CodeBlock& block) {
    printf("\n\n ==== %s ==== \n\n", block.name.c_str());
    for (auto instr : block.instructions) {
        printInstruction(instr, block);
    }
    printf("==== /%s ==== \n\n", block.name.c_str());
}

} // namespace fox