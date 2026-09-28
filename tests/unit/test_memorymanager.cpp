#include <cassert>
#include <memory>

#include "domain/MemoryManager.hpp"
#include "domain/replacement/FifoPolicy.hpp"

void test_write_read_and_statistics() {
    const MemoryConfig config(4096, 256 * 1024);
    auto policy = std::make_unique<application::FifoPolicy>();
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

int main() {
    test_write_read_and_statistics();
    return 0;
}