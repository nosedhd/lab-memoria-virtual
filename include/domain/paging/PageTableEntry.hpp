#pragma once
#include <cstdint>

class PageTableEntry {
public:
    PageTableEntry();

    uint32_t getPfn() const;
    bool getValidBit() const;
    bool getAccessedBit() const;
    bool getDirtyBit() const;

    void allocate();
    void load(uint32_t pfn);
    void recordAccess(bool is_write);
    void invalidate();

private:
    uint32_t pfn_;
    bool valid_bit_;
    bool accessed_bit_;
    bool dirty_bit_;
};