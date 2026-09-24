#include "domain/paging/PageTableEntry.hpp"
#include <stdexcept>
#include <string>

PageTableEntry::PageTableEntry()
    :pfn_(0),
    valid_bit_(false),
    accessed_bit_(false),
    dirty_bit_(false)
{

}

uint32_t PageTableEntry::getPfn() const{
   return pfn_;
}

bool PageTableEntry::getValidBit() const{
    return valid_bit_;
};

bool PageTableEntry::getAccessedBit() const{
    return accessed_bit_;
};

bool PageTableEntry::getDirtyBit() const{
    return dirty_bit_;
};

void PageTableEntry::load(uint32_t pfn){
    if (valid_bit_) {
        throw std::logic_error("La pagina ya esta cargada en el marco " + std::to_string(pfn_));
    }
    pfn_ = pfn;
    valid_bit_ = true;
    accessed_bit_ = false;
    dirty_bit_ = false;
}

void PageTableEntry::recordAccess(bool is_write) {
    if (!valid_bit_) {
        throw std::logic_error("No se puede registrar un acceso a una pagina que no esta en memoria");
    }
    accessed_bit_ = true;
    if (is_write) {
        dirty_bit_ = true;
    }
}

void PageTableEntry::invalidate() {
    pfn_ = 0;
    valid_bit_ = false;
    accessed_bit_ = false;
    dirty_bit_ = false;
}
 