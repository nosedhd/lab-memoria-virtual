int main(int argc, char** argv) {
    /*
#include <memory>
#include "application/MMU.hpp"
#include "application/FifoPolicy.hpp"
#include "infrastructure/FileCommandSource.hpp"
#include "infrastructure/ConsoleStatsReporter.hpp"

    domain::PhysicalMemory mem(config::PHYSICAL_FRAMES);

    std::unique_ptr<domain::IReplacementPolicy> policy =
        std::make_unique<application::FifoPolicy>();

    std::unique_ptr<application::ICommandSource> source =
        std::make_unique<infrastructure::FileCommandSource>(argv[argc - 1]);

    std::unique_ptr<application::IStatsReporter> reporter =
        std::make_unique<infrastructure::ConsoleStatsReporter>();

    application::MMU mmu(mem, std::move(policy));
    for (const auto& cmd : source->loadCommands())
        mmu.execute(cmd);

    reporter->report(mmu.getStats());
    return 0;*/
    (void)argc;
    (void)argv;
    return 1;
}