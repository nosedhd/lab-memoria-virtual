#include "domain/config/MemoryConfig.hpp"
#include <cstdint>

class VirtualAddress {
public:
    explicit VirtualAddress(uint32_t raw_address, const MemoryConfig& config);
    uint32_t getRaw() const;
    uint32_t getDirectoryIndex() const; // PT1 (10 bits)
    uint32_t getTableIndex() const;     // PT2 (10 bits)
    uint32_t getOffset() const;         // Offset (12 bits)
    uint32_t getVpn() const;            // Número de página virtual completa (20 bits)
private:
    static bool isValidVirtualAddress(uint32_t raw_address, const MemoryConfig& config);
    static uint32_t computeDirectoryIndex(uint32_t raw_address, const MemoryConfig& config);
    static uint32_t computeTableIndex(uint32_t raw_address, const MemoryConfig& config);
    static uint32_t computeOffset(uint32_t raw_address, const MemoryConfig& config);
    static uint32_t computeVpn(uint32_t raw_address, const MemoryConfig& config);

    uint32_t raw_address_;
    uint32_t directory_index_;
    uint32_t table_index_;
    uint32_t offset_;
    uint32_t vpn_;
};