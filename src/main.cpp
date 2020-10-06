#include "fox.hpp"
#include <array>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <stdint.h>
#include <string>
#include <vector>

namespace fox {

// types

using s32 = int32_t;
using u32 = uint32_t;
using u8  = uint8_t;
using u16 = uint16_t;

using Value = double;

// opcode.cpp

enum Op {
    halt       = 0,
    load_const = 1,
    add        = 2,
    op__count,
};

static const std::array<std::string, op__count> opMap = {"HALT", "LOAD_CONST",
                                                         "ADD"};

#define OP2S(o)   opMap[o]
#define OP2CS(o)  opMap[o].c_str()
#define OPNAME(o) opMap[opCode(o)]

using instruction = u32;

/** returns the Opcode from a given instruction
 * @param i 32 bit instruction to decode the Opcode from
 */

inline u8 opCode(instruction i) {
    return i >> 24;
}

/** returns the A register from an instruction.
 * @param i instruction
 */

inline u8 rA(u32 i) {
    return (i & 0x00ff0000) >> 16;
}

/** returns the B register from an instruction.
 * @param i instruction
 */

inline u8 rB(u32 i) {
    return (i & 0x0000ff00) >> 8;
}

/** returns the B register from an instruction.
 * @param i instruction
 */
inline u8 rC(u32 i) {
    return (i & 0x000000ff);
}

/** returns the constant index in the constant pool from an instruction of the
 * format iAx (opcode-dest_register-index)
 * @param i Instruction to draw the index from.
 */

inline u16 get_constant_index(u32 i) {
    return (i & 0x0000ffff);
}

// debug.cpp

#define FOX_DEBUG

void print_iABC(instruction i) {
    printf("%-16s    %03d    %3d    %03d\n", OP2CS(opCode(i)), rA(i), rB(i),
           rC(i));
}

void print_iAx(instruction i) {
    printf("%-16s    %03d    %03d    [<TODO>]\n", OP2CS(opCode(i)), rA(i),
           get_constant_index(i));
}

void printInstruction(instruction i) {
    u8 op = opCode(i);

    // TODO: remove magic numbers
    switch (op) {
    case Op::load_const:
        print_iAx(i);
        return;
    case Op::add:
        print_iABC(i);
        return;
    case Op::halt:
        printf("HALT");
        return;
    }
}

// vm.cpp

struct CodeBlock {
    std::vector<instruction> instructions;
    std::vector<u32> lines;
    std::vector<Value> constant_pool;

    /** adds a constant to the constant pool and returns the index of
     * the value added.
     *  @param constant value to add.
     */
    u16 addConstant(Value constant) {
        constant_pool.push_back(constant);
        return constant_pool.size() - 1;
    }

    void addInstruction(instruction instr, u32 line) {
        instructions.push_back(instr);
        lines.push_back(line);
    }
};

enum class ExitCode {
    success,
    error,
    paused,
};

class VM {
  public:
    VM(CodeBlock& block) : block(block) {
        for (u8 i = 0; i < NUM_REGISTERS; i++) {
            registers[i] = 0;
        }
    }

    /**
     * @param runTillEnd (bool) if false then only excutes one instruction.
     **/

    ExitCode run(bool runTillEnd = true) {
        do {
            const instruction instr = fetch();
            const u8 code           = opCode(instr);

#ifdef FOX_DEBUG
            printInstruction(instr);
#endif

            switch (code) {
            case Op::halt:
                return (pc == block.instructions.size()) ? ExitCode::success
                                                         : ExitCode::error;
            case Op::load_const: {
                u8 reg         = rA(instr);
                registers[reg] = block.constant_pool[get_constant_index(instr)];
                break;
            }
            case Op::add: {
                u8 reg = rA(instr);
                break;
            }
            default:
                std::cout << "Error: Unknown Opcode " << (int)instr
                          << std::endl;
                return ExitCode::error;
            }
        } while (runTillEnd);

        return ExitCode::paused;
    }

    // Execute the next instruction and stop
    ExitCode step() {
        return run(false);
    }

    Value readRegister(u8 i) const {
        return registers[i];
    }

  private:
    static u8 constexpr NUM_REGISTERS = 32;
    std::array<Value, NUM_REGISTERS> registers;
    const CodeBlock& block;
    u32 pc = 0;

    inline instruction fetch() {
        return block.instructions[pc++];
    }
};

} // namespace fox

using namespace fox;

#define ASSERT(x, s)                                                           \
    for (; !(x); assert(x)) {                                                  \
        std::cout << s << std::endl;                                           \
    }

void assert_op(instruction i, Op expected) {
    u8 o = opCode(i);
    ASSERT(o == expected,
           "Expected " << OPNAME(expected) << " got " << OPNAME(o));
}

void assert_ra(instruction i, u8 expected) {
    ASSERT(rA(i) == expected, "[LOAD] expected RA to be "
                                  << (int)expected << " got " << (int)rA(i));
}

void assert_rb(instruction i, u8 expected) {
    ASSERT(rB(i) == expected, "[LOAD] expected RA to be "
                                  << (int)expected << " got " << (int)rB(i));
}

void assert_rc(instruction i, u8 expected) {
    ASSERT(rC(i) == expected, "[LOAD] expected RC to be "
                                  << (int)expected << " got " << (int)rB(i));
}

void assert_reg_value(const VM& vm, u8 reg, Value expected) {
    ASSERT(vm.readRegister(reg) == expected,
           "Expected value at R[" << (int)reg << "] to be " << expected
                                  << ". Found " << vm.readRegister(reg));
}

void testOpcode() {

    // LOAD RA INDEX

    instruction load = 0x01080000;
    // test if decoding opcodes works as exepcted
    assert_op(load, Op::load_const);
    assert_ra(load, 8);

    // ADD RA RB RC

    instruction add = 0x02ef02ff;
    assert_op(add, Op::add);
    assert_ra(add, 239);
    assert_rb(add, 2);
    assert_rc(add, 255);

    // HALT
    assert_op(0, Op::halt);
}

void testVM() {
    CodeBlock block;

    u16 index = block.addConstant(123);
    block.addInstruction(0x01030000 /*-load-03-*/, 1);
    block.addInstruction(0x02010304, 1);
    block.addInstruction(0, 1);

    VM vm{block};

    ExitCode state = vm.step(); // execute load instruction

    assert_reg_value(vm, 3, 123);
    assert(state == ExitCode::paused);

    state = vm.run();
    ASSERT(state == ExitCode::success, "VM tests failed.");
}

int main() {
    std::cout << "--Fox VM--" << std::endl;
    testOpcode();
    testVM();
    return 0;
}