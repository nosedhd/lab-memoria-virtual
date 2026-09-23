#pragma once
#include "domain/config/MemoryConfig.hpp"
#include <cstdint>

class VirtualAddress {
public:
    VirtualAddress(uint32_t raw_address, const MemoryConfig& config);

    uint32_t getRaw() const;
    uint32_t getDirectoryIndex() const; 
    uint32_t getPageTableIndex() const;     
    uint32_t getOffset() const;         
    uint32_t getVpn() const;           
    
private:
    static bool isValidVirtualAddress(uint32_t raw_address, const MemoryConfig& config);
    static uint32_t computeDirectoryIndex(uint32_t raw_address, const MemoryConfig& config);
    static uint32_t computePageTableIndex(uint32_t raw_address, const MemoryConfig& config);
    static uint32_t computeOffset(uint32_t raw_address, const MemoryConfig& config);
    static uint32_t computeVpn(uint32_t raw_address, const MemoryConfig& config);

    uint32_t raw_address_;
    uint32_t directory_index_;
    uint32_t page_table_index_;
    uint32_t offset_;
    uint32_t vpn_;
};