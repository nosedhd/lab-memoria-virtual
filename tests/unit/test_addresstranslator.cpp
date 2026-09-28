#include <cassert>

#include "domain/config/MemoryConfig.hpp"
#include "domain/paging/DirectoryTable.hpp"
#include "domain/paging/VirtualAddress.hpp"
#include "domain/translation/AddressTranslator.hpp"

void test_page_fault_when_page_is_not_loaded() {
    const MemoryConfig config(4096, 256 * 1024);
    DirectoryTable directory(config.getDirectoryEntryCount());
    AddressTranslator translator(directory, config);
    const VirtualAddress address(0x00C07004, config);

    const TranslationResult result = translator.translate(address);

    assert(result.page_fault);
    assert(!result.tlb_hit);
}

void test_translation_and_tlb_hit() {
    const MemoryConfig config(4096, 256 * 1024);
    DirectoryTable directory(config.getDirectoryEntryCount());
    const VirtualAddress address(0x00C07004, config);

    PageTable& page_table = directory.getOrCreatePageTable(
        address.getDirectoryIndex(), config.getPageTableEntryCount());
    page_table.getEntry(address.getPageTableIndex()).load(5);

    AddressTranslator translator(directory, config);
    const TranslationResult first_result = translator.translate(address);
    const TranslationResult second_result = translator.translate(address);

    assert(!first_result.page_fault);
    assert(!first_result.tlb_hit);
    assert(first_result.frame == 5);
    assert(first_result.physical_address == 5 * 4096 + 4);

    assert(!second_result.page_fault);
    assert(second_result.tlb_hit);
    assert(second_result.frame == 5);
    assert(second_result.physical_address == 5 * 4096 + 4);
}

int main() {
    test_page_fault_when_page_is_not_loaded();
    test_translation_and_tlb_hit();
    return 0;
}