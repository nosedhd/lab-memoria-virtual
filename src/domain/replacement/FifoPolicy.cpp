#include "domain/replacement/FifoPolicy.hpp"

#include <algorithm>
#include <stdexcept>
#include <string>

namespace application {

void FifoPolicy::onLoad(unsigned int frame) {
    if (contains(frame)) {
        throw std::logic_error(
            "El marco " + std::to_string(frame) +
            " ya esta en la cola FIFO; debio liberarse o elegirse como victima antes");
    }
    loadOrder_.push_back(frame);
}

void FifoPolicy::onAccess(unsigned int) {
    // FIFO no modifica el orden cuando se accede a un frame. Esto existe por si se quiere cambiar a LRU
}

void FifoPolicy::onFree(unsigned int frame) {
    if (!contains(frame)) {
        throw std::logic_error(
            "El marco " + std::to_string(frame) + " no esta en la cola FIFO");
    }
    loadOrder_.remove(frame);
}

unsigned int FifoPolicy::selectVictim() {
    if (loadOrder_.empty()) {
        throw std::runtime_error(
            "No hay marcos disponibles para seleccionar una victima FIFO");
    }

    const unsigned int victim = loadOrder_.front();
    loadOrder_.pop_front();
    return victim;
}

bool FifoPolicy::contains(unsigned int frame) const {
    return std::find(loadOrder_.begin(), loadOrder_.end(), frame) != loadOrder_.end();
}

}  
