#pragma once

#include <vector>
#include <cstdint>

class PhysicalMemory {
public:
    explicit PhysicalMemory(uint32_t size_in_bytes);

    uint8_t readByte(uint32_t physical_address) const;
    void writeByte(uint32_t physical_address, uint8_t value);

    uint32_t getSize() const;

private:
    std::vector<uint8_t> storage_;
};