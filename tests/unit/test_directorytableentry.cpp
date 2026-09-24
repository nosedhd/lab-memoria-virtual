#include <cassert>
#include <stdexcept>

#include "domain/paging/DirectoryTableEntry.hpp"

void test_default_state() {
    const DirectoryTableEntry entry;

    assert(entry.getPtn() == 0);
    assert(!entry.getValidBit());
}

void test_load_and_invalidate() {
    DirectoryTableEntry entry;
    entry.load(37);

    assert(entry.getPtn() == 37);
    assert(entry.getValidBit());

    bool thrown = false;
    try {
        entry.load(42);
    } catch (const std::logic_error&) {
        thrown = true;
    }
    assert(thrown);

    entry.invalidate();
    assert(entry.getPtn() == 0);
    assert(!entry.getValidBit());
}

void test_allocate() {
    DirectoryTableEntry entry;
    entry.allocate();

    assert(entry.getPtn() == 0);
    assert(entry.getValidBit());

    bool thrown = false;
    try {
        entry.allocate();
    } catch (const std::logic_error&) {
        thrown = true;
    }
    assert(thrown);
}

int main() {
    test_default_state();
    test_load_and_invalidate();
    test_allocate();
    return 0;
}