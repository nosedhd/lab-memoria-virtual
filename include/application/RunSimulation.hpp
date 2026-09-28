#pragma once

#include "application/Instruction.hpp"
#include "application/SimulationReport.hpp"
#include "application/ports/IDocumentInput.hpp"
#include "application/ports/IReportExporter.hpp"
#include "domain/MemoryManager.hpp"

class RunSimulation {
public:
    RunSimulation(MemoryManager& memory_manager, IDocumentInput& input, IReportExporter& exporter);

    SimulationReport execute();

private:
    void executeInstruction(const Instruction& instruction);
    SimulationReport buildReport(uint64_t instructions_executed,
                                 std::vector<std::string> errors) const;

    MemoryManager& memory_manager_;
    IDocumentInput& input_;
    IReportExporter& exporter_;
};
