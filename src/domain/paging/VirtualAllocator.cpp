#include "domain/paging/VirtualAllocator.hpp"

#include <stdexcept>
#include <string>

#include "domain/paging/VirtualAddress.hpp"

VirtualAllocator::VirtualAllocator(DirectoryTable& directory, const MemoryConfig& config)
    : directory_(directory), config_(config) {}

uint32_t VirtualAllocator::allocate(uint32_t bytes) {
    if (bytes == 0) {
        throw std::invalid_argument(
            "La cantidad de bytes a reservar debe ser mayor a 0");
    }

    const uint64_t page_size = config_.getPageSize();
    const uint64_t pages = (static_cast<uint64_t>(bytes) + page_size - 1) / page_size;
    const uint64_t reserved_bytes = pages * page_size;
    const uint64_t address_limit = uint64_t{1} << MemoryConfig::VIRTUAL_ADDRESS_BITS;

    if (next_address_ + reserved_bytes > address_limit) {
        throw std::overflow_error(
            "No hay suficiente espacio virtual para reservar " +
            std::to_string(bytes) + " bytes");
    }

    const uint32_t start_address = static_cast<uint32_t>(next_address_);
    for (uint64_t page = 0; page < pages; ++page) {
        reservePage(static_cast<uint32_t>(start_address + page * page_size));
    }

    next_address_ += reserved_bytes;
    return start_address;
}

void VirtualAllocator::reservePage(uint32_t page_address) {
    const VirtualAddress va(page_address, config_);
    PageTable& page_table = directory_.getOrCreatePageTable(
        va.getDirectoryIndex(), config_.getPageTableEntryCount());
    page_table.getEntry(va.getPageTableIndex()).allocate();
}