#pragma once
#include <cstdint>

//revisar pq creo que acá debe ser un &
class DirectoryTableEntry{
    public:
    DirectoryTableEntry();

    uint32_t getPfn() const;
    bool getValidBit() const;

    void allocate();
    void load(uint32_t ptn);
    void invalidate();

    private:
    uint32_t ptn_;
    bool valid_bit_;
};