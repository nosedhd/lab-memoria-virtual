#pragma once

#include <cstdint>

#include "domain/config/MemoryConfig.hpp"
#include "domain/paging/DirectoryTable.hpp"

class VirtualAllocator {
public:
    VirtualAllocator(DirectoryTable& directory, const MemoryConfig& config);

    uint32_t allocate(uint32_t bytes);

private:
    void reservePage(uint32_t page_address);

    DirectoryTable& directory_;
    const MemoryConfig& config_;
    uint64_t next_address_{0};
};
