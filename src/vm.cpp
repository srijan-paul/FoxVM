#include "vm.hpp"

#ifdef FOX_MODE_DEBUG

#include "debug.hpp"
#include <cstdio>
#include <iostream>

#endif

namespace fox {

VM::VM(CodeBlock* block) : block{block} {
    for (u8 i = 0; i < NUM_REGISTERS; i++) {
        registers[i] = 0;
    }
}

ExitCode VM::run(bool runTillEnd) {

    // if running then debug mode, and
    // this is a complete run of the VM (and not a single step)
    // then print a disassembly of the current codeblock.
    do {
        const instruction instr = fetch();
        const u8 code           = opCode(instr);

#ifdef FOX_MODE_DEBUG
        for (u8 i = 0; i < NUM_REGISTERS; i++) {
            auto value = registers[i];
            if (value)
                printf("R%d[%.2f]  ", i, value);
        }
        printf("\n\n");
        printInstruction(instr, *block);
#endif

        switch (code) {
        case Op::halt:
            return (pc == block->instructions.size()) ? ExitCode::success
                                                      : ExitCode::error;
        case Op::load_const: {
            u8 reg         = rA(instr);
            registers[reg] = block->constant_pool[get_constant_index(instr)];
            break;
        }
        case Op::add: {
            Value a = registers[rA(instr)];
            Value b = registers[rB(instr)];

            registers[rC(instr)] = a + b;
            break;
        }
        default:
            std::cout << "Error: Unknown Opcode " << (int)instr << std::endl;
            return ExitCode::error;
        }
    } while (runTillEnd);

    return ExitCode::paused;
}

ExitCode VM::step() {
    return run(false);
}

Value VM::readRegister(u8 i) const {
    return registers[i];
}

} // namespace fox