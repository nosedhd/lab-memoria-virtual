#include "domain/paging/PageTable.hpp"
#include <stdexcept>
#include <string>
 
PageTable::PageTable(uint32_t page_table_entry_count)
    : entries_(page_table_entry_count) {}
 
PageTableEntry& PageTable::getEntry(uint32_t page_table_index) {
    validateIndex(page_table_index);
    return entries_[page_table_index];
}
 
void PageTable::validateIndex(uint32_t page_table_index) const {
    if (page_table_index >= entries_.size()) {
        throw std::out_of_range("Indice de tabla de paginas fuera de rango: ");
    }
}