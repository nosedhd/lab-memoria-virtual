#include "domain/paging/PageTable.hpp"
#include <stdexcept>
#include <string>
 
PageTable::PageTable(uint32_t entry_count)
    : entries_(entry_count) {}
 
PageTableEntry& PageTable::getEntry(uint32_t index) {
    validateIndex(index);
    return entries_[index];
}
 
void PageTable::validateIndex(uint32_t index) const {
    if (index >= entries_.size()) {
        throw std::out_of_range("Indice de tabla de paginas fuera de rango: ");
    }
}