#include "domain/replacement/FifoPolicy.hpp"

#include <stdexcept>

namespace application {

void FifoPolicy::onLoad(unsigned int frame) {
    loadOrder_.push(frame);
}

void FifoPolicy::onAccess(unsigned int) {
    // FIFO no modifica el orden cuando se accede a un frame. Esto existe por si se quiere cambiar a LRU
}

unsigned int FifoPolicy::selectVictim() {
    if (loadOrder_.empty()) {
        throw std::runtime_error(
            "No hay marcos disponibles para seleccionar una victima FIFO");
    }

    const unsigned int victim = loadOrder_.front();
    loadOrder_.pop();
    return victim;
}

}  // namespace application
