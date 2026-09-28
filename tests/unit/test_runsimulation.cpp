#include <cassert>
#include <memory>
#include <string>
#include <vector>

#include "application/RunSimulation.hpp"
#include "domain/replacement/FifoPolicy.hpp"

class FakeInput : public IDocumentInput {
public:
    explicit FakeInput(std::vector<Instruction> instructions)
        : instructions_(std::move(instructions)) {}

    std::vector<Instruction> readInstructions() override {
        return instructions_;
    }

private:
    std::vector<Instruction> instructions_;
};

class CapturingExporter : public IReportExporter {
public:
    void exportReport(const SimulationReport& report) override {
        ++export_count;
        last_report = report;
    }

    int export_count{0};
    SimulationReport last_report;
};

Instruction alloc(uint32_t bytes) { return {InstructionType::Alloc, bytes, 0}; }
Instruction write(uint32_t address, uint8_t value) { return {InstructionType::Write, address, value}; }
Instruction read(uint32_t address) { return {InstructionType::Read, address, 0}; }

void test_statement_example() {
    const MemoryConfig config(4096, 256 * 1024);
    MemoryManager manager(config, std::make_unique<FifoPolicy>());
    FakeInput input({alloc(8192), write(0, 42), write(4096, 99), read(0), read(4096)});
    CapturingExporter exporter;

    const SimulationReport report = RunSimulation(manager, input, exporter).execute();

    assert(exporter.export_count == 1);
    assert(report.instructions_executed == 5);
    assert(report.total_accesses == 4);
    assert(report.page_faults == 2);
    assert(report.replacements == 0);
    assert(report.hit_rate == 50.0);
    assert(report.policy_name == "FIFO");
    assert(report.frame_count == 64);
    assert(report.errors.empty());
    assert(report.elapsed_ticks == 4);
    assert(manager.read(0) == 42);
}

void test_error_is_reported_and_simulation_continues() {
    const MemoryConfig config(4096, 256 * 1024);
    MemoryManager manager(config, std::make_unique<FifoPolicy>());
    FakeInput input({alloc(4096), write(900000, 5), write(0, 7), read(0)});
    CapturingExporter exporter;

    const SimulationReport report = RunSimulation(manager, input, exporter).execute();

    assert(report.errors.size() == 1);
    assert(report.errors[0].find("Instruccion 2") != std::string::npos);
    assert(report.errors[0].find("write 900000 5") != std::string::npos);
    assert(report.total_accesses == 2);
    assert(report.elapsed_ticks == 2);
    assert(manager.read(0) == 7);
}

void test_empty_input_produces_empty_report() {
    const MemoryConfig config(4096, 256 * 1024);
    MemoryManager manager(config, std::make_unique<FifoPolicy>());
    FakeInput input({});
    CapturingExporter exporter;

    const SimulationReport report = RunSimulation(manager, input, exporter).execute();

    assert(exporter.export_count == 1);
    assert(report.instructions_executed == 0);
    assert(report.total_accesses == 0);
    assert(report.hit_rate == 0.0);
    assert(report.elapsed_ticks == 0);
}

int main() {
    test_statement_example();
    test_error_is_reported_and_simulation_continues();
    test_empty_input_produces_empty_report();
    return 0;
}
