#pragma once
#include <cstdint>
#include <string>

enum class InstructionType { Alloc, Write, Read, Free };

struct Instruction {
    InstructionType type{InstructionType::Read};
    uint32_t operand{0};
    uint8_t value{0};
};

std::string toString(const Instruction& instruction);
