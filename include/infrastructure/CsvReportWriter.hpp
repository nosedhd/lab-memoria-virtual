#pragma once

#include <string>

#include "application/ports/IReportExporter.hpp"

class CsvReportWriter : public IReportExporter {
public:
    CsvReportWriter(std::string file_path, std::string input_name);

    void exportReport(const SimulationReport& report) override;

    static std::string header();
    static std::string row(const SimulationReport& report, const std::string& input_name);

private:
    std::string file_path_;
    std::string input_name_;
};
