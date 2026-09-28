#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include "domain/config/MemoryConfig.hpp"
#include "domain/memory/FrameTable.hpp"
#include "domain/memory/PhysicalMemory.hpp"
#include "domain/paging/DirectoryTable.hpp"
#include "domain/paging/VirtualAddress.hpp"
#include "domain/replacement/IReplacementPolicy.hpp"
#include "domain/stats/Stats.hpp"
#include "domain/translation/AddressTranslator.hpp"

class MemoryManager {
public:
    MemoryManager(const MemoryConfig& config,
                  std::unique_ptr<domain::IReplacementPolicy> policy);

    // Operaciones principales del simulador
    uint32_t allocate(uint32_t bytes);
    void write(uint32_t virtual_address, uint8_t value);
    uint8_t read(uint32_t virtual_address);
    void free(uint32_t virtual_address);

    // Consultas y observadores
    const Stats& getStats() const;
    const MemoryConfig& getConfig() const;
    std::string getPolicyName() const;
    const DirectoryTable& getDirectoryTable() const;
    const FrameTable& getFrameTable() const;
    const PhysicalMemory& getPhysicalMemory() const;

private:
    uint32_t handlePageFault(const VirtualAddress& va);

    MemoryConfig config_;
    DirectoryTable directory_;
    PhysicalMemory physical_memory_;
    FrameTable frame_table_;
    AddressTranslator translator_;
    std::unique_ptr<domain::IReplacementPolicy> policy_;
    Stats stats_;

    uint32_t next_allocated_virtual_address_{0};
};
