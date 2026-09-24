#include "domain/paging/DirectoryTable.hpp"

#include <stdexcept>
#include <string>

DirectoryTable::DirectoryTable(uint32_t entry_count)
    : entries_(entry_count),
      page_tables_(entry_count) {}

DirectoryTableEntry& DirectoryTable::getEntry(uint32_t index) {
    validateIndex(index);
    return entries_[index];
}

const DirectoryTableEntry& DirectoryTable::getEntry(uint32_t index) const {
    validateIndex(index);
    return entries_[index];
}

bool DirectoryTable::hasPageTable(uint32_t dir_index) const {
    validateIndex(dir_index);
    return page_tables_[dir_index] != nullptr;
}

PageTable& DirectoryTable::getOrCreatePageTable(
    uint32_t dir_index, uint32_t entries_per_table) {
    validateIndex(dir_index);

    if (entries_per_table == 0) {
        throw std::invalid_argument(
            "Una tabla de paginas debe tener al menos una entrada");
    }

    if (page_tables_[dir_index] == nullptr) {
        page_tables_[dir_index] =
            std::make_unique<PageTable>(entries_per_table);

        if (!entries_[dir_index].getValidBit()) {
            entries_[dir_index].load(dir_index);
        }
    }

    return *page_tables_[dir_index];
}

void DirectoryTable::validateIndex(uint32_t index) const {
    if (index >= entries_.size()) {
        throw std::out_of_range(
            "Indice del directorio fuera de rango: " +
            std::to_string(index));
    }
}
