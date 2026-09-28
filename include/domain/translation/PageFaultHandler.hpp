#pragma once

#include <cstdint>

#include "domain/config/MemoryConfig.hpp"
#include "domain/memory/FrameTable.hpp"
#include "domain/memory/PhysicalMemory.hpp"
#include "domain/paging/DirectoryTable.hpp"
#include "domain/paging/VirtualAddress.hpp"
#include "domain/replacement/IReplacementPolicy.hpp"
#include "domain/translation/AddressTranslator.hpp"

struct PageFaultResult {
    uint32_t frame{0};
    bool replaced{false};
};

class PageFaultHandler {
public:
    PageFaultHandler(DirectoryTable& directory, FrameTable& frame_table, PhysicalMemory& physical_memory,
                     AddressTranslator& translator, IReplacementPolicy& policy, const MemoryConfig& config);

    PageFaultResult handle(const VirtualAddress& va);

private:
    uint32_t takeFreeFrame(const VirtualAddress& va);
    uint32_t evictVictim(const VirtualAddress& va);
    void loadPage(const VirtualAddress& va, uint32_t frame);

    DirectoryTable& directory_;
    FrameTable& frame_table_;
    PhysicalMemory& physical_memory_;
    AddressTranslator& translator_;
    IReplacementPolicy& policy_;
    const MemoryConfig& config_;
};
