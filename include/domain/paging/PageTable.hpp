#pragma once
#include "domain/paging/PageTableEntry.hpp"
#include <cstdint>
#include <vector>
 
class PageTable {
public:
    explicit PageTable(uint32_t entry_count);
 
    PageTableEntry& getEntry(uint32_t index);

private:
    void validateIndex(uint32_t index) const;
    std::vector<PageTableEntry> entries_;
};