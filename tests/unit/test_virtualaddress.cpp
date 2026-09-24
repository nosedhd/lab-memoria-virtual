#include <cassert>

#include "domain/config/MemoryConfig.hpp"
#include "domain/paging/VirtualAddress.hpp"

void test_address_decomposition() {
    const MemoryConfig config(4096, 256 * 1024);
    const uint32_t directory_index = 3;
    const uint32_t table_index = 7;
    const uint32_t offset = 0xABC;
    const uint32_t raw_address = (directory_index << 22) |
                                 (table_index << 12) |
                                 offset;

    const VirtualAddress address(raw_address, config);

    assert(address.getRaw() == raw_address);
    assert(address.getDirectoryIndex() == directory_index);
    assert(address.getPageTableIndex() == table_index);
    assert(address.getOffset() == offset);
    assert(address.getVpn() == ((directory_index << 10) | table_index));
}

void test_address_boundaries() {
    const MemoryConfig config(4096, 256 * 1024);
    const VirtualAddress address(0xFFFFFFFFU, config);

    assert(address.getDirectoryIndex() == 1023);
    assert(address.getPageTableIndex() == 1023);
    assert(address.getOffset() == 4095);
    assert(address.getVpn() == 0xFFFFF);
}

int main() {
    test_address_decomposition();
    test_address_boundaries();
    return 0;
}