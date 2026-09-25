#pragma once
#include "domain/paging/PageTableEntry.hpp"
#include <cstdint>
#include <vector>
 
class PageTable {
public:
    explicit PageTable(uint32_t page_table_entry_count);
 
    PageTableEntry& getEntry(uint32_t page_table_index);

private:
    void validateIndex(uint32_t page_table_index) const;
    std::vector<PageTableEntry> entries_;
};