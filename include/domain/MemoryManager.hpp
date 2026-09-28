#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include "domain/paging/VirtualAllocator.hpp"
#include "domain/config/MemoryConfig.hpp"
#include "domain/translation/PageFaultHandler.hpp"
#include "domain/memory/FrameTable.hpp"
#include "domain/memory/PhysicalMemory.hpp"
#include "domain/paging/DirectoryTable.hpp"
#include "domain/paging/VirtualAddress.hpp"
#include "domain/replacement/IReplacementPolicy.hpp"
#include "domain/stats/Stats.hpp"
#include "domain/translation/AddressTranslator.hpp"

class MemoryManager {
public:
    MemoryManager(const MemoryConfig& config, std::unique_ptr<IReplacementPolicy> policy);

    MemoryManager(const MemoryManager&) = delete;
    MemoryManager& operator=(const MemoryManager&) = delete;
    MemoryManager(MemoryManager&&) = delete;
    MemoryManager& operator=(MemoryManager&&) = delete;

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
    enum class AccessType { Read, Write };

    static std::unique_ptr<IReplacementPolicy> requirePolicy(
        std::unique_ptr<IReplacementPolicy> policy);

    uint32_t resolvePhysicalAddress(uint32_t virtual_address, AccessType access_type);

    MemoryConfig config_;
    DirectoryTable directory_;
    PhysicalMemory physical_memory_;
    FrameTable frame_table_;
    AddressTranslator translator_;
    std::unique_ptr<IReplacementPolicy> policy_;
    Stats stats_;
    VirtualAllocator allocator_;
    PageFaultHandler fault_handler_;
};
