#include "domain/config/MemoryConfig.hpp"
#include <string>
#include <cmath>

MemoryConfig::MemoryConfig(uint32_t page_size, uint32_t physical_memory_size)
        : page_size_(page_size),
            physical_memory_size_(physical_memory_size),
            frame_count_(0),
            offset_bits_(0),
            directory_index_bits_(0),
            page_table_index_bits_(0) {
        if (!isPowerOfTwo(page_size)) {
            throw std::invalid_argument("El tamaño de la pagina debe ser potencia de dos");
        }
        if (!isPageSizeInRange(page_size)) {
            throw std::invalid_argument("El tamaño de la pagina no esta dentro del rango permitido");
        }
        if (!isPhysicalMemoryInRange(physical_memory_size)) {
            throw std::invalid_argument("El tamaño de la memoria no esta dentro del rango permitido");
        }
        if (!isMultipleOfPageSize(physical_memory_size, page_size)) {
            throw std::invalid_argument("La memoria fisica debe ser multiplo del tamaño de la pagina");   
        }

        frame_count_ = physical_memory_size / page_size;
        offset_bits_ = computeOffsetBits(page_size);

        const uint32_t vpn_bits = VIRTUAL_ADDRESS_BITS - offset_bits_;
        directory_index_bits_ = computeDirectoryIndexBits(vpn_bits);
        page_table_index_bits_ = vpn_bits - directory_index_bits_;
    }

uint32_t MemoryConfig::getPageSize() const { return page_size_; }
uint32_t MemoryConfig::getPhysicalMemorySize() const { return physical_memory_size_; }
uint32_t MemoryConfig::getFrameCount() const { return frame_count_; }
uint32_t MemoryConfig::getOffsetBits() const { return offset_bits_; }
uint32_t MemoryConfig::getDirectoryIndexBits() const { return directory_index_bits_; }
uint32_t MemoryConfig::getPageTableIndexBits() const { return page_table_index_bits_; }


bool MemoryConfig::isPowerOfTwo(uint32_t value) {
    return value != 0 && (value & (value - 1)) == 0;
}

bool MemoryConfig::isPageSizeInRange(uint32_t page_size) {
    return page_size >= MIN_PAGE_SIZE &&
    page_size <= MAX_PAGE_SIZE;
}

bool MemoryConfig::isPhysicalMemoryInRange(uint32_t physical_memory_size) {
    return physical_memory_size >= MIN_PHYSICAL_MEMORY_SIZE &&
           physical_memory_size <= MAX_PHYSICAL_MEMORY_SIZE;
}

bool MemoryConfig::isMultipleOfPageSize(uint32_t physical_memory_size, uint32_t page_size) {
    return physical_memory_size % page_size == 0;
}

uint32_t MemoryConfig::computeOffsetBits(uint32_t page_size) {
    uint32_t bits = 0;
    while (page_size > 1) {
        page_size >>= 1;
        ++bits;
    }
    return bits;
}

uint32_t MemoryConfig::computeDirectoryIndexBits(uint32_t vpn_bits) {
    return (vpn_bits + 1) / 2;
}

uint32_t MemoryConfig::getDirectoryEntryCount() const{
    return static_cast<uint32_t>(std::pow(2, getDirectoryIndexBits()));
}

uint32_t MemoryConfig::getPageTableEntryCount() const{
    return static_cast<uint32_t>(std::pow(2, getPageTableIndexBits()));
}