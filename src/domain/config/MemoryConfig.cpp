#include "MemoryConfig.hpp"
#include <string>
#include <cmath>

MemoryConfig::MemoryConfig(uint32_t page_size, uint32_t physical_memory_size)
    :page_size_(page_size), physical_memory_size_(physical_memory_size)
    {
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
    }

uint32_t MemoryConfig::getPageSize() const { return page_size_; }
uint32_t MemoryConfig::getPhysicalMemorySize() const { return physical_memory_size_; }
uint32_t MemoryConfig::getFrameCount() const { return frame_count_; }
uint32_t MemoryConfig::getOffsetBits() const { return offset_bits_; }
uint32_t MemoryConfig::getDirectoryIndexBits() const { return directory_index_bits_; }
uint32_t MemoryConfig::getPageIndexBits() const { return page_index_bits_; }


bool MemoryConfig::isPowerOfTwo(uint32_t value) {
    //to do
    return true;
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
    //to do
    return 1;
}

uint32_t MemoryConfig::computeDirectoryIndexBits(uint32_t vpn_bits) {
   //to do
   return 1;
}