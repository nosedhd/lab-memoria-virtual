#pragma once

#include <cstdint>
#include <string>
#include <vector>

struct CommandLineOptions {
    std::string input_path;
    uint32_t page_size{4096};
    uint32_t physical_memory_size{256 * 1024};
    std::string csv_path{"reporte.csv"};
};

CommandLineOptions parseCommandLine(const std::vector<std::string>& arguments);

std::string usageText(const std::string& program_name);
