#pragma once
#include "code-block.hpp"
#include "fox.hpp"
#include "opcodes.hpp"


namespace fox {

using Value = double;

enum class ExitCode {
    success,
    error,
    paused,
};

class VM {
  public:
    VM(CodeBlock& block);

    // if runTillEnd is false, exectures only the next instruction and then
    // stops

    ExitCode run(bool runTillEnd = true);

    // Execute the next instruction and stop
    ExitCode step();

    Value readRegister(u8 i) const;

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