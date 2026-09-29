#include <exception>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "application/RunSimulation.hpp"
#include "domain/MemoryManager.hpp"
#include "domain/config/MemoryConfig.hpp"
#include "domain/replacement/FifoPolicy.hpp"
#include "infrastructure/CommandLineOptions.hpp"
#include "infrastructure/CsvReportWriter.hpp"
#include "infrastructure/DocumentReader.hpp"

int main(int argc, char** argv) {
    const std::string program_name = argc > 0 ? argv[0] : "simulador";
    const std::vector<std::string> arguments(argv + 1, argv + argc);

    CommandLineOptions options;
    try {
        options = parseCommandLine(arguments);
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << "\n\n" << usageText(program_name);
        return 1;
    }

    try {
        const MemoryConfig config(options.page_size, options.physical_memory_size);
        MemoryManager memory_manager(config, std::make_unique<FifoPolicy>());
        DocumentReader input(options.input_path);
        CsvReportWriter csv_writer(options.csv_path, options.input_path);

        RunSimulation(memory_manager, input, csv_writer).execute();
        std::cout << "Reporte CSV guardado en: " << options.csv_path << "\n";
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << "\n";
        return 1;
    }
    return 0;
}
