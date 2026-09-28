#include "domain/MemoryManager.hpp"
#include "domain/replacement/IReplacementPolicy.hpp"

#include <limits>
#include <stdexcept>

MemoryManager::MemoryManager(
    const MemoryConfig& config,
    std::unique_ptr<IReplacementPolicy> policy)
    : config_(config),
      directory_(config.getDirectoryEntryCount()),
      physical_memory_(config.getPhysicalMemorySize()),
      frame_table_(config.getFrameCount()),
      translator_(directory_, config_),
      policy_(std::move(policy)) {
    if (policy_ == nullptr) {
        throw std::invalid_argument(
            "MemoryManager requiere una politica de reemplazo");
    }
}

uint32_t MemoryManager::allocate(uint32_t bytes) {
    if (bytes == 0) {
        throw std::invalid_argument(
            "La cantidad de bytes a reservar debe ser mayor a 0");
    }

    const uint64_t page_size = config_.getPageSize();
    const uint64_t pages = (static_cast<uint64_t>(bytes) + page_size - 1) /
                           page_size;
    const uint64_t reserved_bytes = pages * page_size;
    const uint64_t address_limit =
        uint64_t{1} << MemoryConfig::VIRTUAL_ADDRESS_BITS; 

    if (next_allocated_virtual_address_ + reserved_bytes > address_limit) {
        throw std::overflow_error(
            "No hay suficiente espacio virtual para la reserva");
    }

    const uint32_t start_address = next_allocated_virtual_address_;

    for (uint64_t page = 0; page < pages; ++page) {
        const VirtualAddress page_address(
            static_cast<uint32_t>(start_address + page * page_size), config_);
        PageTable& page_table = directory_.getOrCreatePageTable(
            page_address.getDirectoryIndex(), config_.getPageTableEntryCount());
        page_table.getEntry(page_address.getPageTableIndex()).allocate();
    }

    next_allocated_virtual_address_ +=
        static_cast<uint32_t>(reserved_bytes);
    return start_address;
}

void MemoryManager::write(uint32_t virtual_address, uint8_t value) {
    const uint32_t physical_address =
        resolvePhysicalAddress(virtual_address, AccessType::Write);
    physical_memory_.writeByte(physical_address, value);
}

uint8_t MemoryManager::read(uint32_t virtual_address) {
    const uint32_t physical_address =
        resolvePhysicalAddress(virtual_address, AccessType::Read);
    return physical_memory_.readByte(physical_address);
}

uint32_t MemoryManager::resolvePhysicalAddress(
    uint32_t virtual_address, AccessType access_type) {
    const VirtualAddress va(virtual_address, config_);
    TranslationResult translation = translator_.translate(va);
    stats_.recordAccess();
    if (translation.tlb_hit) {
        stats_.recordTlbHit();
    }
    if (translation.page_fault) {
        stats_.recordPageFault();
        translation.frame = handlePageFault(va);
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

uint32_t MemoryManager::handlePageFault(const VirtualAddress& va) {
    uint32_t frame = 0;

    if (frame_table_.hasFreeFrame()) {
        frame = frame_table_.allocateFreeFrame(
            va.getVpn(), va.getDirectoryIndex(), va.getPageTableIndex());
    } else {
        frame = policy_->selectVictim();
        const FrameInfo& victim = frame_table_.getFrameInfo(frame);

        PageTable* victim_table =
            directory_.getPageTable(victim.directory_index);
        if (victim_table == nullptr) {
            throw std::logic_error(
                "El marco victima no tiene una tabla de paginas asociada");
        }

        PageTableEntry& victim_entry =
            victim_table->getEntry(victim.page_table_index);
        victim_entry.invalidate();
        translator_.invalidateTlb(victim.vpn);
        frame_table_.mapFrame(
            frame, va.getVpn(), va.getDirectoryIndex(),
            va.getPageTableIndex());
        stats_.recordReplacement();
    }

    physical_memory_.clearRange(
        frame * config_.getPageSize(), config_.getPageSize());

    PageTable* page_table = directory_.getPageTable(va.getDirectoryIndex());
    PageTableEntry& entry = page_table->getEntry(va.getPageTableIndex());
    entry.load(frame);
    policy_->onLoad(frame);
    translator_.cacheTranslation(va.getVpn(), frame);
    return frame;
}
