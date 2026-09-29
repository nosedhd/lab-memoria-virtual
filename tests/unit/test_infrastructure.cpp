#include <cassert>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "infrastructure/CommandLineOptions.hpp"
#include "infrastructure/CsvReportWriter.hpp"

#include <cstdio>
#include <fstream>

bool contains(const std::string& text, const std::string& fragment) {
    return text.find(fragment) != std::string::npos;
}

bool rejectsArguments(const std::vector<std::string>& arguments) {
    try {
        parseCommandLine(arguments);
    } catch (const std::invalid_argument&) {
        return true;
    }
    return false;
}

void test_command_line_defaults_and_options() {
    const CommandLineOptions defaults = parseCommandLine({"entrada.txt"});
    assert(defaults.input_path == "entrada.txt");
    assert(defaults.page_size == 4096);
    assert(defaults.physical_memory_size == 262144);
    assert(defaults.csv_path == "reporte.csv");

    const CommandLineOptions custom = parseCommandLine(
        {"--page-size", "8192", "entrada.txt", "--memory", "524288", "--csv", "salida.csv"});
    assert(custom.input_path == "entrada.txt");
    assert(custom.page_size == 8192);
    assert(custom.physical_memory_size == 524288);
    assert(custom.csv_path == "salida.csv");
}

void test_command_line_errors() {
    assert(rejectsArguments({}));
    assert(rejectsArguments({"entrada.txt", "--page-size"}));
    assert(rejectsArguments({"entrada.txt", "--csv"}));
    assert(rejectsArguments({"entrada.txt", "--page-size", "4k"}));
    assert(rejectsArguments({"entrada.txt", "--memory", "0"}));
    assert(rejectsArguments({"entrada.txt", "--verbose"}));
    assert(rejectsArguments({"entrada.txt", "--policy", "fifo"}));
    assert(rejectsArguments({"a.txt", "b.txt"}));
}


SimulationReport sampleReport() {
    SimulationReport report;
    report.policy_name = "FIFO";
    report.page_size = 4096;
    report.physical_memory_size = 262144;
    report.frame_count = 64;
    report.instructions_executed = 5;
    report.total_accesses = 4;
    report.page_faults = 2;
    report.hit_rate = 50.0;
    report.tlb_hits = 2;
    report.elapsed_ticks = 4;
    return report;
}

void test_csv_row_format() {
    assert(CsvReportWriter::row(sampleReport(), "tests/test1_basico.txt") ==
           "tests/test1_basico.txt,FIFO,4096,262144,64,5,4,2,50.00,0,2,4,0,");
    assert(CsvReportWriter::row(sampleReport(), "mi,archivo.txt").rfind("\"mi,archivo.txt\",", 0) == 0);

    SimulationReport with_errors = sampleReport();
    with_errors.errors = {"Instruccion 2 (read 9): fallo", "Instruccion 5 (free 0): otro"};
    const std::string line = CsvReportWriter::row(with_errors, "a.txt");
    assert(line.find(",2,Instruccion 2 (read 9): fallo | Instruccion 5 (free 0): otro") != std::string::npos);
}

void test_csv_writes_header_once_and_appends_rows() {
    const std::string path = "test_reporte_temporal.csv";
    std::remove(path.c_str());

    CsvReportWriter writer(path, "entrada.txt");
    writer.exportReport(sampleReport());
    writer.exportReport(sampleReport());

    std::ifstream file(path);
    std::string line;
    std::vector<std::string> lines;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    file.close();
    std::remove(path.c_str());

    assert(lines.size() == 3);
    assert(lines[0] == CsvReportWriter::header());
    assert(lines[1] == lines[2]);
}

int main() {
    test_command_line_defaults_and_options();
    test_command_line_errors();
    test_csv_row_format();
    test_csv_writes_header_once_and_appends_rows();
    return 0;
}
