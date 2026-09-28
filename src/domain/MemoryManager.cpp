#include "domain/MemoryManager.hpp"

#include <stdexcept>

MemoryManager::MemoryManager(const MemoryConfig& config, std::unique_ptr<IReplacementPolicy> policy)
    : config_(config), directory_(config.getDirectoryEntryCount()),
      physical_memory_(config.getPhysicalMemorySize()), frame_table_(config.getFrameCount()),
      translator_(directory_, config_), policy_(requirePolicy(std::move(policy))),
      allocator_(directory_, config_),
      fault_handler_(directory_, frame_table_, physical_memory_, translator_, *policy_, config_) {}

std::unique_ptr<IReplacementPolicy> MemoryManager::requirePolicy(
    std::unique_ptr<IReplacementPolicy> policy) {
    if (policy == nullptr) {
        throw std::invalid_argument(
            "MemoryManager requiere una politica de reemplazo");
    }
    return policy;
}

uint32_t MemoryManager::allocate(uint32_t bytes) {
    return allocator_.allocate(bytes);
}

void MemoryManager::write(uint32_t virtual_address, uint8_t value) {
    const uint32_t physical_address = resolvePhysicalAddress(virtual_address, AccessType::Write);
    physical_memory_.writeByte(physical_address, value);
}

uint8_t MemoryManager::read(uint32_t virtual_address) {
    const uint32_t physical_address = resolvePhysicalAddress(virtual_address, AccessType::Read);
    return physical_memory_.readByte(physical_address);
}

uint32_t MemoryManager::resolvePhysicalAddress(uint32_t virtual_address, AccessType access_type) {
    const VirtualAddress va(virtual_address, config_);
    TranslationResult translation = translator_.translate(va);
    stats_.recordAccess();
    clock_.tick();
    if (translation.tlb_hit) {
        stats_.recordTlbHit();
    }

    if (translation.page_fault) {
        stats_.recordPageFault();
        const PageFaultResult fault = fault_handler_.handle(va);
        if (fault.replaced) {
            stats_.recordReplacement();
        }
        
        translation.frame = fault.frame;
        translation.physical_address =
            translation.frame * config_.getPageSize() + va.getOffset();
    }

    PageTable* page_table = directory_.getPageTable(va.getDirectoryIndex());
    PageTableEntry& entry = page_table->getEntry(va.getPageTableIndex());
    entry.recordAccess(access_type == AccessType::Write);
    policy_->onAccess(translation.frame);
    return translation.physical_address;
}

void MemoryManager::free(uint32_t virtual_address) {
    const VirtualAddress va(virtual_address, config_);
    PageTable* page_table = directory_.getPageTable(va.getDirectoryIndex());
    if (page_table == nullptr) {
        throw std::invalid_argument(
            "La direccion virtual no esta asignada");
    }

    PageTableEntry& entry = page_table->getEntry(va.getPageTableIndex());
    if (!entry.getValidBit()) {
        throw std::invalid_argument(
            "La pagina virtual no esta cargada en memoria");
    }

    const uint32_t frame = entry.getPfn();
    entry.invalidate();
    translator_.invalidateTlb(va.getVpn());
    policy_->onFree(frame);
    frame_table_.freeFrame(frame);
}

const Stats& MemoryManager::getStats() const {
    return stats_;
}

const Tick& MemoryManager::getClock() const {
    return clock_;
}

const MemoryConfig& MemoryManager::getConfig() const {
    return config_;
}

std::string MemoryManager::getPolicyName() const {
    return policy_->name();
}

const DirectoryTable& MemoryManager::getDirectoryTable() const {
    return directory_;
}

const FrameTable& MemoryManager::getFrameTable() const {
    return frame_table_;
}

const PhysicalMemory& MemoryManager::getPhysicalMemory() const {
    return physical_memory_;
}
