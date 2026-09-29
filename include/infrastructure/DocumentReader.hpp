#pragma once
#include <string>
#include <vector>
#include "application/ports/IDocumentInput.hpp"

// Formato: "alloc <bytes>", "write <dir> <valor>", "read <dir>", "free <dir>".
// Una linea puede tener varias instrucciones, y todo lo que sigue a '#' es
// comentario. Los numeros pueden ser decimales o hexadecimales (0x...).
class DocumentReader : public IDocumentInput {
public:
    explicit DocumentReader(std::string file_path);

    std::vector<Instruction> readInstructions() override;

    static std::vector<Instruction> parse(const std::string& content);

private:
    std::string file_path_;
};
