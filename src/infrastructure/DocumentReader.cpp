#include "infrastructure/DocumentReader.hpp"

#include <cctype>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace {

struct Token {
    std::string text;
    std::size_t line;
};

std::vector<Token> tokenize(const std::string& content) {
    std::vector<Token> tokens;
    std::istringstream lines(content);
    std::string line;
    std::size_t line_number = 0;

    while (std::getline(lines, line)) {
        ++line_number;
        const std::size_t comment_start = line.find('#');
        if (comment_start != std::string::npos) {
            line.erase(comment_start);
        }

        std::istringstream words(line);
        std::string word;
        while (words >> word) {
            tokens.push_back({word, line_number});
        }
    }
    return tokens;
}

std::string lineLabel(std::size_t line) {
    return "Linea " + std::to_string(line) + ": ";
}

uint64_t parseUnsigned(const Token& token, const std::string& what) {
    const std::string& text = token.text;
    const bool is_hex = text.size() >= 2 && text[0] == '0' &&
                        (text[1] == 'x' || text[1] == 'X');
    const std::string digits = is_hex ? text.substr(2) : text;

    if (digits.empty()) {
        throw std::invalid_argument(
            lineLabel(token.line) + "se esperaba " + what + " y se encontro '" + text + "'");
    }
    for (char digit : digits) {
        const bool valid = is_hex ? std::isxdigit(static_cast<unsigned char>(digit)) != 0
                                  : std::isdigit(static_cast<unsigned char>(digit)) != 0;
        if (!valid) {
            throw std::invalid_argument(
                lineLabel(token.line) + what + " invalido: '" + text +
                "' (use un entero no negativo, decimal o 0x hexadecimal)");
        }
    }

    if (digits.size() > 16) {
        throw std::invalid_argument(
            lineLabel(token.line) + what + " demasiado grande: '" + text + "'");
    }
    return std::stoull(digits, nullptr, is_hex ? 16 : 10);
}

uint32_t parseUint32(const Token& token, const std::string& what) {
    const uint64_t value = parseUnsigned(token, what);
    if (value > UINT32_MAX) {
        throw std::invalid_argument(
            lineLabel(token.line) + what + " fuera del espacio de 32 bits: '" +
            token.text + "' (maximo 4294967295)");
    }
    return static_cast<uint32_t>(value);
}

uint8_t parseByte(const Token& token) {
    const uint64_t value = parseUnsigned(token, "valor");
    if (value > UINT8_MAX) {
        throw std::invalid_argument(
            lineLabel(token.line) + "valor fuera de rango: '" + token.text +
            "' (la memoria guarda bytes, de 0 a 255)");
    }
    return static_cast<uint8_t>(value);
}

std::string toLower(std::string text) {
    for (char& character : text) {
        character = static_cast<char>(std::tolower(static_cast<unsigned char>(character)));
    }
    return text;
}

}  

DocumentReader::DocumentReader(std::string file_path)
    : file_path_(std::move(file_path)) {}

std::vector<Instruction> DocumentReader::readInstructions() {
    std::ifstream file(file_path_);
    if (!file) {
        throw std::runtime_error("No se pudo abrir el archivo de entrada: " + file_path_);
    }

    std::ostringstream content;
    content << file.rdbuf();
    return parse(content.str());
}

std::vector<Instruction> DocumentReader::parse(const std::string& content) {
    const std::vector<Token> tokens = tokenize(content);
    std::vector<Instruction> instructions;
    std::size_t position = 0;

    auto nextArgument = [&](const Token& command, const std::string& what) -> const Token& {
        if (position >= tokens.size()) {
            throw std::invalid_argument(
                lineLabel(command.line) + "a '" + command.text + "' le falta " + what);
        }
        return tokens[position++];
    };

    while (position < tokens.size()) {
        const Token& command = tokens[position++];
        const std::string keyword = toLower(command.text);
        Instruction instruction;

        if (keyword == "alloc") {
            instruction.type = InstructionType::Alloc;
            instruction.operand = parseUint32(nextArgument(command, "la cantidad de bytes"), "cantidad de bytes");
        } else if (keyword == "write") {
            instruction.type = InstructionType::Write;
            instruction.operand = parseUint32(nextArgument(command, "la direccion"), "direccion");
            instruction.value = parseByte(nextArgument(command, "el valor"));
        } else if (keyword == "read") {
            instruction.type = InstructionType::Read;
            instruction.operand = parseUint32(nextArgument(command, "la direccion"), "direccion");
        } else if (keyword == "free") {
            instruction.type = InstructionType::Free;
            instruction.operand = parseUint32(nextArgument(command, "la direccion"), "direccion");
        } else {
            throw std::invalid_argument(
                lineLabel(command.line) + "instruccion desconocida '" + command.text +
                "' (se esperaba alloc, write, read o free)");
        }

        instructions.push_back(instruction);
    }
    return instructions;
}
