#include "application/Instruction.hpp"

std::string toString(const Instruction& instruction) {
    const std::string operand = std::to_string(instruction.operand);
    switch (instruction.type) {
        case InstructionType::Alloc:
            return "alloc " + operand;
        case InstructionType::Write:
            return "write " + operand + " " + std::to_string(instruction.value);
        case InstructionType::Read:
            return "read " + operand;
        case InstructionType::Free:
            return "free " + operand;
    }
    return "instruccion desconocida";
}
