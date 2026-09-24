#pragma once
#include <cstdint>

class DirectoryTableEntry {
public:
    DirectoryTableEntry();

    uint32_t getPtn() const;
    bool getValidBit() const;

    void allocate();
    void load(uint32_t ptn);
    void invalidate();

private:
    uint32_t ptn_;
    bool valid_bit_;
};