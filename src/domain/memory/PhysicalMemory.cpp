#include "domain/memory/PhysicalMemory.hpp"
#include <algorithm>
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

void PhysicalMemory::clearRange(uint32_t start_address, uint32_t length) {
    const uint64_t end_address = static_cast<uint64_t>(start_address) + length;
    if (end_address > storage_.size()) {
        throw std::out_of_range(
            "Rango fuera de la memoria fisica: [" + std::to_string(start_address) +
            ", " + std::to_string(end_address) + ") excede " +
            std::to_string(storage_.size()) + " bytes");
    }
    std::fill(storage_.begin() + start_address, storage_.begin() + end_address, 0);
}

uint32_t PhysicalMemory::getSize() const {
    return storage_.size();
}
