#include "application/RunSimulation.hpp"

#include <exception>
#include <string>
#include <utility>

RunSimulation::RunSimulation(MemoryManager& memory_manager, IDocumentInput& input,
                             IReportExporter& exporter)
    : memory_manager_(memory_manager), input_(input), exporter_(exporter) {}

SimulationReport RunSimulation::execute() {
    const std::vector<Instruction> instructions = input_.readInstructions();
    std::vector<std::string> errors;

    for (std::size_t index = 0; index < instructions.size(); ++index) {
        try {
            executeInstruction(instructions[index]);
        } catch (const std::exception& error) {
            errors.push_back("Instruccion " + std::to_string(index + 1) + " (" +
                             toString(instructions[index]) + "): " + error.what());
        }
    }
    const SimulationReport report = buildReport(instructions.size(), std::move(errors));
    exporter_.exportReport(report);
    return report;
}

void RunSimulation::executeInstruction(const Instruction& instruction) {
    switch (instruction.type) {
        case InstructionType::Alloc:
            memory_manager_.allocate(instruction.operand);
            break;
        case InstructionType::Write:
            memory_manager_.write(instruction.operand, instruction.value);
            break;
        case InstructionType::Read:
            memory_manager_.read(instruction.operand);
            break;
        case InstructionType::Free:
            memory_manager_.free(instruction.operand);
            break;
    }
}

SimulationReport RunSimulation::buildReport(uint64_t instructions_executed, std::vector<std::string> errors) const {
    const Stats& stats = memory_manager_.getStats();
    const MemoryConfig& config = memory_manager_.getConfig();

    SimulationReport report;
    report.policy_name = memory_manager_.getPolicyName();
    report.page_size = config.getPageSize();
    report.physical_memory_size = config.getPhysicalMemorySize();
    report.frame_count = config.getFrameCount();
    report.instructions_executed = instructions_executed;
    report.total_accesses = stats.getTotalAccesses();
    report.page_faults = stats.getPageFaults();
    report.replacements = stats.getReplacements();
    report.tlb_hits = stats.getTlbHits();
    report.hit_rate = stats.getHitRate();
    report.elapsed_ticks = memory_manager_.getClock().now();
    report.errors = std::move(errors);
    return report;
}
