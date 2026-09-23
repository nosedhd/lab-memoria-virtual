#pragma once
#include <cstdint>
#include <stdexcept>

class MemoryConfig {
public:
    MemoryConfig(uint32_t page_size, uint32_t physical_memory_size);

    uint32_t getPageSize() const;
    uint32_t getPhysicalMemorySize() const;
    uint32_t getFrameCount() const;
    uint32_t getOffsetBits() const;
    uint32_t getDirectoryIndexBits() const;
    uint32_t getPageIndexBits() const;

    static constexpr uint32_t VIRTUAL_ADDRESS_BITS = 32;
    static constexpr uint32_t TLB_SIZE = 16;

    static constexpr uint32_t MIN_PAGE_SIZE = 1024;
    static constexpr uint32_t MAX_PAGE_SIZE = 1024 * 1024;

    static constexpr uint32_t MIN_PHYSICAL_MEMORY_SIZE = 256 * 1024;
    static constexpr uint32_t MAX_PHYSICAL_MEMORY_SIZE = 64 * 1024 * 1024;

private:
    static bool isPowerOfTwo(uint32_t value);
    static bool isPageSizeInRange(uint32_t page_size);
    static bool isPhysicalMemoryInRange(uint32_t physical_memory_size);
    static bool isMultipleOfPageSize(uint32_t physical_memory_size, uint32_t page_size);

    static uint32_t computeOffsetBits(uint32_t page_size);

    static uint32_t computeVpnBits(uint32_t vpn_bits);

    uint32_t page_size_;
    uint32_t physical_memory_size_;
    uint32_t frame_count_;
    uint32_t offset_bits_;
    uint32_t directory_index_bits_;
    uint32_t page_index_bits_;
};