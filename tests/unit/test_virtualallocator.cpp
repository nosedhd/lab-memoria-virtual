#include <cassert>
#include <stdexcept>

#include "domain/paging/VirtualAllocator.hpp"
#include "domain/config/MemoryConfig.hpp"
#include "domain/paging/DirectoryTable.hpp"
#include "domain/paging/VirtualAddress.hpp"

bool isAllocated(const DirectoryTable& directory, uint32_t address, const MemoryConfig& config) {
    const VirtualAddress va(address, config);
    const PageTable* table = directory.getPageTable(va.getDirectoryIndex());
    return table != nullptr && table->getEntry(va.getPageTableIndex()).getAllocatedBit();
}

void test_blocks_are_consecutive_and_page_aligned() {
    const MemoryConfig config(4096, 256 * 1024);
    DirectoryTable directory(config.getDirectoryEntryCount());
    VirtualAllocator allocator(directory, config);

    assert(allocator.allocate(8192) == 0);
    assert(allocator.allocate(1) == 8192);
    assert(allocator.allocate(4097) == 12288);
    assert(allocator.allocate(10) == 20480);
}

void test_every_page_of_the_block_is_reserved() {
    const MemoryConfig config(4096, 256 * 1024);
    DirectoryTable directory(config.getDirectoryEntryCount());
    VirtualAllocator allocator(directory, config);

    allocator.allocate(4097);

    assert(isAllocated(directory, 0, config));
    assert(isAllocated(directory, 4096, config));
    assert(!isAllocated(directory, 8192, config));
}

void test_block_can_cross_page_tables() {
    const MemoryConfig config(4096, 256 * 1024);
    DirectoryTable directory(config.getDirectoryEntryCount());
    VirtualAllocator allocator(directory, config);
    const uint32_t pages_per_table = config.getPageTableEntryCount();

    allocator.allocate((pages_per_table + 1) * config.getPageSize());

    assert(directory.hasPageTable(0));
    assert(directory.hasPageTable(1));
    assert(isAllocated(directory, pages_per_table * config.getPageSize(), config));
}

void test_rejects_invalid_sizes() {
    const MemoryConfig config(4096, 256 * 1024);
    DirectoryTable directory(config.getDirectoryEntryCount());
    VirtualAllocator allocator(directory, config);

    bool zero_thrown = false;
    try {
        allocator.allocate(0);
    } catch (const std::invalid_argument&) {
        zero_thrown = true;
    }
    assert(zero_thrown);

    allocator.allocate(0xFFFFF000u);
    bool overflow_thrown = false;
    try {
        allocator.allocate(8192);
    } catch (const std::overflow_error&) {
        overflow_thrown = true;
    }
    assert(overflow_thrown);
}

int main() {
    test_blocks_are_consecutive_and_page_aligned();
    test_every_page_of_the_block_is_reserved();
    test_block_can_cross_page_tables();
    test_rejects_invalid_sizes();
    return 0;
}
