#include <cassert>
#include <memory>
#include <stdexcept>

#include "domain/MemoryManager.hpp"
#include "domain/replacement/FifoPolicy.hpp"

void test_write_read_and_statistics() {
    const MemoryConfig config(4096, 256 * 1024);
    auto policy = std::make_unique<FifoPolicy>();
    MemoryManager manager(config, std::move(policy));

    const uint32_t base_address = manager.allocate(4096);
    manager.write(base_address + 12, 99);

    assert(manager.read(base_address + 12) == 99);
    assert(manager.getStats().getTotalAccesses() == 2);
    assert(manager.getStats().getPageFaults() == 1);
    assert(manager.getStats().getReplacements() == 0);
    assert(manager.getStats().getTlbHits() == 1);
    assert(manager.getStats().getHitRate() == 50.0);
}

void test_access_without_allocation_is_segmentation_fault() {
    const MemoryConfig config(4096, 256 * 1024);
    MemoryManager manager(config, std::make_unique<FifoPolicy>());

    bool thrown = false;
    try {
        manager.write(900000, 5);
    } catch (const std::runtime_error&) {
        thrown = true;
    }
    assert(thrown);
    assert(manager.getStats().getTotalAccesses() == 0);
}

void test_access_outside_allocated_block_is_segmentation_fault() {
    const MemoryConfig config(4096, 256 * 1024);
    MemoryManager manager(config, std::make_unique<FifoPolicy>());

    manager.allocate(8192);
    manager.write(0, 42);
    manager.write(4096, 99);

    bool thrown = false;
    try {
        manager.write(8192, 7);
    } catch (const std::runtime_error&) {
        thrown = true;
    }
    assert(thrown);
    assert(manager.read(0) == 42);
    assert(manager.read(4096) == 99);
    assert(manager.getStats().getTotalAccesses() == 4);
    assert(manager.getStats().getPageFaults() == 2);
}

void test_replaced_frame_does_not_leak_previous_data() {
    const MemoryConfig config(4096, 256 * 1024);
    MemoryManager manager(config, std::make_unique<FifoPolicy>());
    const uint32_t page_size = config.getPageSize();
    const uint32_t frame_count = config.getFrameCount();

    manager.allocate((frame_count + 1) * page_size);
    manager.write(1, 9);
    for (uint32_t page = 1; page < frame_count; ++page) {
        manager.write(page * page_size, 1);
    }
    manager.write(frame_count * page_size, 77);

    assert(manager.getStats().getReplacements() == 1);
    assert(manager.read(frame_count * page_size + 1) == 0);
}

void test_freed_frame_is_clean_when_reused() {
    const MemoryConfig config(4096, 256 * 1024);
    MemoryManager manager(config, std::make_unique<FifoPolicy>());
    const uint32_t page_size = config.getPageSize();
    const uint32_t frame_count = config.getFrameCount();

    manager.allocate((frame_count + 1) * page_size);
    for (uint32_t page = 0; page < frame_count; ++page) {
        manager.write(page * page_size + 1, 5);
    }
    manager.free(0);
    manager.write(frame_count * page_size, 7);

    assert(manager.getStats().getReplacements() == 0);
    assert(manager.read(frame_count * page_size + 1) == 0);
}

void test_free_removes_frame_from_fifo_order() {
    const MemoryConfig config(4096, 256 * 1024);
    MemoryManager manager(config, std::make_unique<FifoPolicy>());
    const uint32_t page_size = config.getPageSize();
    const uint32_t frame_count = config.getFrameCount();

    manager.allocate((frame_count + 2) * page_size);
    for (uint32_t page = 0; page < frame_count; ++page) {
        manager.write(page * page_size, 1);
    }
    manager.free(0);
    manager.write(frame_count * page_size, 2);
    manager.write((frame_count + 1) * page_size, 3);

    const uint64_t faults_before = manager.getStats().getPageFaults();
    manager.read(frame_count * page_size);
    assert(manager.getStats().getPageFaults() == faults_before);

    manager.read(1 * page_size);
    assert(manager.getStats().getPageFaults() == faults_before + 1);
}

void test_clock_ticks_once_per_valid_access() {
    const MemoryConfig config(4096, 256 * 1024);
    MemoryManager manager(config, std::make_unique<FifoPolicy>());

    manager.allocate(8192);
    assert(manager.getClock().now() == 0);

    manager.write(0, 1);
    manager.read(0);
    manager.write(4096, 2);
    assert(manager.getClock().now() == 3);

    try {
        manager.read(900000);
    } catch (const std::runtime_error&) {
    }
    assert(manager.getClock().now() == 3);
}

int main() {
    test_clock_ticks_once_per_valid_access();
    test_write_read_and_statistics();
    test_access_without_allocation_is_segmentation_fault();
    test_access_outside_allocated_block_is_segmentation_fault();
    test_replaced_frame_does_not_leak_previous_data();
    test_freed_frame_is_clean_when_reused();
    test_free_removes_frame_from_fifo_order();
    return 0;
}
