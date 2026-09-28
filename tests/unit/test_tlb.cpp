#include <cassert>

#include "domain/memory/TLB.hpp"

void test_lookup_and_insert() {
    TLB tlb;

    assert(!tlb.lookup(7).has_value());

    tlb.insert(7, 3);
    const std::optional<uint32_t> frame = tlb.lookup(7);

    assert(frame.has_value());
    assert(frame.value() == 3);
}

void test_update_and_invalidate() {
    TLB tlb;
    tlb.insert(7, 3);

    assert(tlb.update(7, 9));
    assert(tlb.lookup(7).value() == 9);
    assert(!tlb.update(8, 10));

    tlb.invalidate(7);
    assert(!tlb.lookup(7).has_value());
}

void test_lru_replacement() {
    TLB tlb;

    for (uint32_t vpn = 0; vpn < TLB::CAPACITY; ++vpn) {
        tlb.insert(vpn, vpn + 100);
    }

    assert(tlb.lookup(0).value() == 100);
    tlb.insert(TLB::CAPACITY, 200);

    assert(!tlb.lookup(1).has_value());
    assert(tlb.lookup(0).value() == 100);
    assert(tlb.lookup(TLB::CAPACITY).value() == 200);
}

void test_clear() {
    TLB tlb;
    tlb.insert(7, 3);
    tlb.clear();

    assert(!tlb.lookup(7).has_value());
}

int main() {
    test_lookup_and_insert();
    test_update_and_invalidate();
    test_lru_replacement();
    test_clear();
    return 0;
}