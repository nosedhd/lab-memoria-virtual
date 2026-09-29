#include <cassert>
#include <stdexcept>
#include <string>

#include "infrastructure/DocumentReader.hpp"

bool rejects(const std::string& content, const std::string& expected_fragment) {
    try {
        DocumentReader::parse(content);
    } catch (const std::invalid_argument& error) {
        return std::string(error.what()).find(expected_fragment) != std::string::npos;
    }
    return false;
}

void test_statement_example_in_one_line() {
    const auto instructions =
        DocumentReader::parse("alloc 8192 write 0 42 write 4096 99 read 0 read 4096");

    assert(instructions.size() == 5);
    assert(instructions[0].type == InstructionType::Alloc && instructions[0].operand == 8192);
    assert(instructions[1].type == InstructionType::Write && instructions[1].operand == 0 &&
           instructions[1].value == 42);
    assert(instructions[2].type == InstructionType::Write && instructions[2].operand == 4096 &&
           instructions[2].value == 99);
    assert(instructions[3].type == InstructionType::Read && instructions[3].operand == 0);
    assert(instructions[4].type == InstructionType::Read && instructions[4].operand == 4096);
}

void test_multiple_lines_comments_hex_and_case() {
    const auto instructions = DocumentReader::parse(
        "# programa de prueba\n"
        "ALLOC 0x2000   # dos paginas\n"
        "\n"
        "Write 0x1000 255\n"
        "free 0\n");

    assert(instructions.size() == 3);
    assert(instructions[0].operand == 8192);
    assert(instructions[1].operand == 4096 && instructions[1].value == 255);
    assert(instructions[2].type == InstructionType::Free);
}

void test_empty_input() {
    assert(DocumentReader::parse("").empty());
    assert(DocumentReader::parse("# solo comentarios\n\n").empty());
}

void test_invalid_input_has_clear_messages() {
    assert(rejects("alloc 10\nmove 5", "Linea 2: instruccion desconocida 'move'"));
    assert(rejects("write 0", "le falta el valor"));
    assert(rejects("read", "le falta la direccion"));
    assert(rejects("write 0 256", "de 0 a 255"));
    assert(rejects("read 4294967296", "fuera del espacio de 32 bits"));
    assert(rejects("read -5", "direccion invalido"));
    assert(rejects("read 12abc", "direccion invalido"));
    assert(rejects("read 0x", "se esperaba direccion"));
}

void test_missing_file() {
    DocumentReader reader("no_existe_este_archivo.txt");
    bool thrown = false;
    try {
        reader.readInstructions();
    } catch (const std::runtime_error&) {
        thrown = true;
    }
    assert(thrown);
}

int main() {
    test_statement_example_in_one_line();
    test_multiple_lines_comments_hex_and_case();
    test_empty_input();
    test_invalid_input_has_clear_messages();
    test_missing_file();
    return 0;
}
