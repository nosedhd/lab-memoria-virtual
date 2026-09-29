#include "infrastructure/CommandLineOptions.hpp"

#include <cctype>
#include <stdexcept>

namespace {

uint32_t parsePositiveNumber(const std::string& option, const std::string& text) {
    if (text.empty() || text.size() > 10) {
        throw std::invalid_argument(
            "Valor invalido para " + option + ": '" + text + "'");
    }
    for (char digit : text) {
        if (!std::isdigit(static_cast<unsigned char>(digit))) {
            throw std::invalid_argument(
                "Valor invalido para " + option + ": '" + text + "' (debe ser un entero positivo)");
        }
    }
    const unsigned long long value = std::stoull(text);
    if (value == 0 || value > UINT32_MAX) {
        throw std::invalid_argument(
            "Valor fuera de rango para " + option + ": '" + text + "'");
    }
    return static_cast<uint32_t>(value);
}

}  

CommandLineOptions parseCommandLine(const std::vector<std::string>& arguments) {
    CommandLineOptions options;

    for (std::size_t index = 0; index < arguments.size(); ++index) {
        const std::string& argument = arguments[index];

        if (argument == "--page-size" || argument == "--memory" || argument == "--csv") {
            if (index + 1 >= arguments.size()) {
                throw std::invalid_argument("Falta el valor de la opcion " + argument);
            }
            const std::string& value = arguments[++index];

            if (argument == "--page-size") {
                options.page_size = parsePositiveNumber(argument, value);
            } else if (argument == "--memory") {
                options.physical_memory_size = parsePositiveNumber(argument, value);
            } else {
                options.csv_path = value;
            }
        } else if (argument.rfind("--", 0) == 0) {
            throw std::invalid_argument("Opcion desconocida: " + argument);
        } else if (options.input_path.empty()) {
            options.input_path = argument;
        } else {
            throw std::invalid_argument("Se recibio mas de un archivo de entrada: " + argument);
        }
    }

    if (options.input_path.empty()) {
        throw std::invalid_argument("Falta el archivo de entrada");
    }
    return options;
}

std::string usageText(const std::string& program_name) {
    return "Uso: " + program_name + " <archivo> [opciones]\n"
           "Opciones:\n"
           "  --page-size <bytes>   Tamano de pagina (por defecto 4096)\n"
           "  --memory <bytes>      Memoria fisica, minimo 262144 (por defecto 262144)\n"
           "  --csv <archivo>       Archivo CSV donde se agrega el resultado (por defecto reporte.csv)\n";
}
