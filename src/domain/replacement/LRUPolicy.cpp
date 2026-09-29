#include "domain/replacement/LRUPolicy.hpp"
#include <stdexcept>
#include <limits>


LRUPolicy::LRUPolicy(unsigned int capacity)
    : last_accessed_(capacity, 0),
      active_(capacity, false),
      global_counter_(0) {
    if (capacity == 0) {
        throw std::invalid_argument("La capacidad de la política debe ser mayor a 0.");
    }
}

void LRUPolicy::onLoad(unsigned int index) {
    validateIndex(index);
    active_[index] = true;
    last_accessed_[index] = ++global_counter_;
}

void LRUPolicy::onAccess(unsigned int index) {
    validateIndex(index);
    if (active_[index]) {
        last_accessed_[index] = ++global_counter_;
    }
}

void LRUPolicy::onFree(unsigned int index) {
    validateIndex(index);
    active_[index] = false;
    last_accessed_[index] = 0;
}

unsigned int LRUPolicy::selectVictim() {
    unsigned int victim = 0;
    uint64_t min_access = std::numeric_limits<uint64_t>::max();
    bool found = false;

    for (size_t i = 0; i < last_accessed_.size(); ++i) {
        if (active_[i] && last_accessed_[i] < min_access) {
            min_access = last_accessed_[i];
            victim = static_cast<unsigned int>(i);
            found = true;
        }
    }

    if (!found) {
        throw std::runtime_error("No hay entradas activas para seleccionar una víctima.");
    }

    return victim;
}

std::string LRUPolicy::name() const {
    return "LRU";
}

void LRUPolicy::validateIndex(unsigned int index) const {
    if (index >= last_accessed_.size()) {
        throw std::out_of_range("Índice fuera del rango de capacidad de la política.");
    }
}

