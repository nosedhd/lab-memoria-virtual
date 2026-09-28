#pragma once
#include "domain/config/MemoryConfig.hpp"
#include "domain/paging/DirectoryTable.hpp"
#include "domain/paging/VirtualAddress.hpp"
#include "domain/memory/TLB.hpp"

struct TranslationResult {
    uint32_t physical_address{0};
    bool page_fault{false};
    bool tlb_hit{false};
    uint32_t frame{0};
};

class AddressTranslator {
public:
    AddressTranslator(DirectoryTable& directory, const MemoryConfig& config);

    TranslationResult translate(const VirtualAddress& va);
    void cacheTranslation(uint32_t vpn, uint32_t frame);
    void invalidateTlb(uint32_t vpn);
    void clearTlb();

private:
    [[noreturn]] static void throwSegmentationFault(const VirtualAddress& va);

    DirectoryTable& directory_;
    const MemoryConfig& config_;
    TLB tlb_;
};