#include "domain/translation/AddressTranslator.hpp"

AddressTranslator::AddressTranslator(
        DirectoryTable& directory, const MemoryConfig& config)
        : directory_(directory),
            config_(config),
            tlb_() {}

TranslationResult AddressTranslator::translate(const VirtualAddress& va) {
    TranslationResult result;

    const std::optional<uint32_t> cached_frame = tlb_.lookup(va.getVpn());
    if (cached_frame.has_value()) {
        result.frame = cached_frame.value();
        result.physical_address =
            result.frame * config_.getPageSize() + va.getOffset();
        result.tlb_hit = true;
        return result;
    }

    const PageTable* page_table =
        directory_.getPageTable(va.getDirectoryIndex());
    if (page_table == nullptr) {
        result.page_fault = true;
        return result;
    }

    const PageTableEntry& page_table_entry =
        page_table->getEntry(va.getPageTableIndex());
    if (!page_table_entry.getValidBit()) {
        result.page_fault = true;
        return result;
    }

    result.frame = page_table_entry.getPfn();
    result.physical_address =
        result.frame * config_.getPageSize() + va.getOffset();
    tlb_.insert(va.getVpn(), result.frame);
    return result;
}

void AddressTranslator::cacheTranslation(uint32_t vpn, uint32_t frame) {
    tlb_.insert(vpn, frame);
}

void AddressTranslator::invalidateTlb(uint32_t vpn) {
    tlb_.invalidate(vpn);
}

void AddressTranslator::clearTlb() {
    tlb_.clear();
}

