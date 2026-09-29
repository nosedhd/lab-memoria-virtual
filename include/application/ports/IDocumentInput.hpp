#pragma once
#include <vector>
#include "application/Instruction.hpp"

class IDocumentInput {
public:
    virtual ~IDocumentInput() = default;
    virtual std::vector<Instruction> readInstructions() = 0;
};
