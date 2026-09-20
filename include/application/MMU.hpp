#pragma once

#include <memory>
#include "domain/PhysicalMemory.hpp"
#include "domain/IReplacementPolicy.hpp"
#include "domain/Command.hpp"
#include "domain/Stats.hpp"
#include "domain/PageTable.hpp"

namespace application {

class MMU {
public:
    MMU(domain::PhysicalMemory& mem,
        std::unique_ptr<domain::IReplacementPolicy> policy);
    void execute(const domain::Command& cmd);
    const domain::Stats& getStats() const;
private:
    domain::PageTableL1 pageTable_;
    domain::PhysicalMemory& memory_;
    std::unique_ptr<domain::IReplacementPolicy> policy_;
    domain::Stats stats_;
};

} // namespace application