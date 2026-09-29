#include <cassert>

#include "domain/paging/VirtualAllocator.hpp"
#include "domain/config/MemoryConfig.hpp"
#include "domain/translation/PageFaultHandler.hpp"
#include "domain/memory/FrameTable.hpp"
#include "domain/memory/PhysicalMemory.hpp"
#include "domain/paging/DirectoryTable.hpp"
#include "domain/paging/VirtualAddress.hpp"
#include "domain/replacement/FifoPolicy.hpp"
#include "domain/translation/AddressTranslator.hpp"

struct Fixture {
    MemoryConfig config{4096, 256 * 1024};
    DirectoryTable directory{config.getDirectoryEntryCount()};
    FrameTable frame_table{config.getFrameCount()};
    PhysicalMemory physical_memory{config.getPhysicalMemorySize()};
    AddressTranslator translator{directory, config};
    FifoPolicy policy;
    VirtualAllocator allocator{directory, config};
    PageFaultHandler handler{directory, frame_table, physical_memory,
                             translator, policy, config};

    const PageTableEntry& entryOf(uint32_t address) const {
        const VirtualAddress va(address, config);
        return directory.getPageTable(va.getDirectoryIndex())->getEntry(va.getPageTableIndex());
    }
};

void test_uses_free_frame_without_replacement() {
    Fixture fixture;
    fixture.allocator.allocate(fixture.config.getPageSize());

    const PageFaultResult result = fixture.handler.handle(VirtualAddress(0, fixture.config));

    assert(!result.replaced);
    assert(fixture.entryOf(0).getValidBit());
    assert(fixture.entryOf(0).getPfn() == result.frame);
    assert(fixture.frame_table.getFreeFrameCount() == fixture.config.getFrameCount() - 1);
}

void test_evicts_oldest_page_when_memory_is_full() {
    Fixture fixture;
    const uint32_t page_size = fixture.config.getPageSize();
    const uint32_t frame_count = fixture.config.getFrameCount();
    fixture.allocator.allocate((frame_count + 1) * page_size);

    for (uint32_t page = 0; page < frame_count; ++page) {
        fixture.handler.handle(VirtualAddress(page * page_size, fixture.config));
    }
    const uint32_t first_frame = fixture.entryOf(0).getPfn();

    const PageFaultResult result =
        fixture.handler.handle(VirtualAddress(frame_count * page_size, fixture.config));

    assert(result.replaced);
    assert(result.frame == first_frame);
    assert(!fixture.entryOf(0).getValidBit());
    assert(fixture.entryOf(0).getAllocatedBit());
    assert(fixture.entryOf(frame_count * page_size).getPfn() == first_frame);
}

void test_loaded_frame_is_clean() {
    Fixture fixture;
    fixture.allocator.allocate(fixture.config.getPageSize());
    fixture.physical_memory.writeByte(5, 99);

    const PageFaultResult result = fixture.handler.handle(VirtualAddress(0, fixture.config));

    assert(fixture.physical_memory.readByte(result.frame * fixture.config.getPageSize() + 5) == 0);
}

int main() {
    test_uses_free_frame_without_replacement();
    test_evicts_oldest_page_when_memory_is_full();
    test_loaded_frame_is_clean();
    return 0;
}
