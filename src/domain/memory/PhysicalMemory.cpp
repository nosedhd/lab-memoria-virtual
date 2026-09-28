#include "domain/memory/PhysicalMemory.hpp"
#include <stdexcept>
#include <string>


PhysicalMemory::PhysicalMemory(uint32_t size_in_bytes)
    : storage_(size_in_bytes, 0) {
        if (size_in_bytes == 0) {
            throw std::invalid_argument("El tamaño de la memoria fisica debe ser mayor a 0");
        }
        
    }


uint8_t PhysicalMemory::readByte(uint32_t physical_address) const {
    if (physical_address >= storage_.size()) {
        throw std::out_of_range("Direccion fisica fuera de rango: " + std::to_string(physical_address));
    }
    return storage_[physical_address];
}

void PhysicalMemory::writeByte(uint32_t physical_address, uint8_t value) {
    if (physical_address >= storage_.size()) {
        throw std::out_of_range("Direccion fisica fuera de rango: " + std::to_string(physical_address));
    }
    storage_[physical_address] = value;
}

uint32_t PhysicalMemory::getSize() const {
    return storage_.size();
}
