#include "domain/translation/PageFaultHandler.hpp"

#include <stdexcept>

PageFaultHandler::PageFaultHandler(DirectoryTable& directory,FrameTable& frame_table, PhysicalMemory& physical_memory,
                                   AddressTranslator& translator, IReplacementPolicy& policy, const MemoryConfig& config)
    : directory_(directory), frame_table_(frame_table),
      physical_memory_(physical_memory), translator_(translator),
      policy_(policy), config_(config) {}

PageFaultResult PageFaultHandler::handle(const VirtualAddress& va) {
    PageFaultResult result;

    if (frame_table_.hasFreeFrame()) {
        result.frame = takeFreeFrame(va);
    } else {
        result.frame = evictVictim(va);
        result.replaced = true;
    }

    loadPage(va, result.frame);
    return result;
}

uint32_t PageFaultHandler::takeFreeFrame(const VirtualAddress& va) {
    return frame_table_.allocateFreeFrame( va.getVpn(), va.getDirectoryIndex(), va.getPageTableIndex());
}

uint32_t PageFaultHandler::evictVictim(const VirtualAddress& va) {
    const uint32_t frame = policy_.selectVictim();
    const FrameInfo& victim = frame_table_.getFrameInfo(frame);

    PageTable* victim_table = directory_.getPageTable(victim.directory_index);
    if (victim_table == nullptr) {
        throw std::logic_error(
            "El marco victima no tiene una tabla de paginas asociada");
    }

    victim_table->getEntry(victim.page_table_index).invalidate();
    translator_.invalidateTlb(victim.vpn);

    frame_table_.mapFrame(frame, va.getVpn(), va.getDirectoryIndex(), va.getPageTableIndex());
    return frame;
}

void PageFaultHandler::loadPage(const VirtualAddress& va, uint32_t frame) {
    physical_memory_.clearRange(frame * config_.getPageSize(), config_.getPageSize());

    PageTable* page_table = directory_.getPageTable(va.getDirectoryIndex());
    page_table->getEntry(va.getPageTableIndex()).load(frame);
    policy_.onLoad(frame);
    translator_.cacheTranslation(va.getVpn(), frame);
}
