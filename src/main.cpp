#include "fox.hpp"

#include "code-block.hpp"
#include "debug.hpp"
#include "opcodes.hpp"
#include "vm.hpp"

#include <cassert>
#include <iostream>

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
    std::string name = "<test-codeblock>";
    CodeBlock block{name};

    u16 index = block.addConstant(5);
    block.addInstruction(0x01010000 /* load 01 00     */, 1);
    block.addInstruction(0x01020000 /* load 02 00     */, 1);
    block.addInstruction(0x02010203 /* add  01 02 03  */, 1);
    block.addInstruction(0 /* halt */, 1);

    disassembleBlock(block);

    VM vm{block};

    ExitCode state = vm.step(); // load 5 into R1

    assert_reg_value(vm, 1, 5);
    assert(state == ExitCode::paused);

    vm.step(); // load 5 into R2
    assert_reg_value(vm, 2, 5);
    vm.step(); // execute add R1 R2 R3
    assert_reg_value(vm, 3, 10);

    state = vm.run();
    ASSERT(state == ExitCode::success, "VM tests failed.");
}

int main() {
    std::cout << "--Fox VM--" << std::endl;
    testOpcode();
    testVM();
    return 0;
}