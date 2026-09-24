#include "domain/paging/DirectoryTableEntry.hpp"

#include <stdexcept>
#include <string>

DirectoryTableEntry::DirectoryTableEntry()
    : ptn_(0),
      valid_bit_(false) {}

uint32_t DirectoryTableEntry::getPtn() const {
    return ptn_;
}

bool DirectoryTableEntry::getValidBit() const {
    return valid_bit_;
}

void DirectoryTableEntry::allocate() {
    if (valid_bit_) {
        throw std::logic_error("La tabla de paginas ya esta asignada");
    }

    ptn_ = 0;
    valid_bit_ = true;
}

void DirectoryTableEntry::load(uint32_t ptn) {
    if (valid_bit_) {
        throw std::logic_error(
            "La tabla de paginas ya esta cargada en la entrada " +
            std::to_string(ptn_));
    }

    ptn_ = ptn;
    valid_bit_ = true;
}

void DirectoryTableEntry::invalidate() {
    ptn_ = 0;
    valid_bit_ = false;
}
