#include "infrastructure/CsvReportWriter.hpp"

#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <vector>

namespace {

std::string escapeCsv(const std::string& field) {
    if (field.find_first_of(",\"\n") == std::string::npos) {
        return field;
    }
    std::string escaped = "\"";
    for (char character : field) {
        if (character == '"') {
            escaped += '"';
        }
        escaped += character;
    }
    return escaped + "\"";
}

std::string joinErrors(const std::vector<std::string>& errors) {
    std::string joined;
    for (std::size_t index = 0; index < errors.size(); ++index) {
        if (index > 0) {
            joined += " | ";
        }
        joined += errors[index];
    }
    return joined;
}

bool isMissingOrEmpty(const std::string& file_path) {
    std::ifstream file(file_path);
    return !file || file.peek() == std::ifstream::traits_type::eof();
}

}  

CsvReportWriter::CsvReportWriter(std::string file_path, std::string input_name)
    : file_path_(std::move(file_path)),
      input_name_(std::move(input_name)) {}

void CsvReportWriter::exportReport(const SimulationReport& report) {
    const bool needs_header = isMissingOrEmpty(file_path_);

    std::ofstream file(file_path_, std::ios::app);
    if (!file) {
        throw std::runtime_error("No se pudo escribir el reporte CSV en: " + file_path_);
    }
    if (needs_header) {
        file << header() << "\n";
    }
    file << row(report, input_name_) << "\n";
}

std::string CsvReportWriter::header() {
    return "archivo,politica,tamano_pagina,memoria_fisica,marcos,instrucciones,"
           "accesos,fallos_pagina,hit_rate,reemplazos,hits_tlb,ticks,errores,detalle_errores";
}

std::string CsvReportWriter::row(const SimulationReport& report, const std::string& input_name) {
    std::ostringstream line;
    line << escapeCsv(input_name) << ','
         << escapeCsv(report.policy_name) << ','
         << report.page_size << ','
         << report.physical_memory_size << ','
         << report.frame_count << ','
         << report.instructions_executed << ','
         << report.total_accesses << ','
         << report.page_faults << ','
         << std::fixed << std::setprecision(2) << report.hit_rate << ','
         << report.replacements << ','
         << report.tlb_hits << ','
         << report.elapsed_ticks << ','
         << report.errors.size() << ','
         << escapeCsv(joinErrors(report.errors));
    return line.str();
}
