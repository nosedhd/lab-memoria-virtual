#include <cassert>
#include <stdexcept>

#include "domain/paging/DirectoryTable.hpp"

void test_creates_and_reuses_page_table() {
    DirectoryTable directory(4);

    assert(!directory.hasPageTable(2));
    assert(!directory.getEntry(2).getValidBit());

    PageTable& first_table = directory.getOrCreatePageTable(2, 8);

    assert(directory.hasPageTable(2));
    assert(directory.getEntry(2).getValidBit());
    assert(directory.getEntry(2).getPtn() == 2);

    PageTable& same_table = directory.getOrCreatePageTable(2, 16);
    assert(&first_table == &same_table);
}

void test_rejects_invalid_arguments() {
    DirectoryTable directory(4);

    bool thrown = false;
    try {
        directory.getEntry(4);
    } catch (const std::out_of_range&) {
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try {
        directory.hasPageTable(4);
    } catch (const std::out_of_range&) {
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try {
        directory.getOrCreatePageTable(0, 0);
    } catch (const std::invalid_argument&) {
        thrown = true;
    }
    assert(thrown);
}

int main() {
    test_creates_and_reuses_page_table();
    test_rejects_invalid_arguments();
    return 0;
}