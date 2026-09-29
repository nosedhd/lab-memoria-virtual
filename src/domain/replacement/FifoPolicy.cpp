#include "domain/replacement/FifoPolicy.hpp"

#include <stdexcept>
#include <string>

void FifoPolicy::onLoad(unsigned int frame) {
    if (queuedFrames_.count(frame) > 0) {
        throw std::logic_error(
            "El marco " + std::to_string(frame) +
            " ya esta en la cola FIFO; debio liberarse o elegirse como victima antes");
    }
    loadOrder_.push(frame);
    queuedFrames_.insert(frame);
}

void FifoPolicy::onAccess(unsigned int) {
    // FIFO no modifica el orden cuando se accede a un frame. Esto existe por si se quiere cambiar a LRU
}

void FifoPolicy::onFree(unsigned int frame) {
    if (queuedFrames_.count(frame) == 0) {
        throw std::logic_error(
            "El marco " + std::to_string(frame) + " no esta en la cola FIFO");
    }

    const std::size_t queued_count = loadOrder_.size();
    for (std::size_t i = 0; i < queued_count; ++i) {
        const unsigned int current = loadOrder_.front();
        loadOrder_.pop();
        if (current != frame) {
            loadOrder_.push(current);
        }
    }
    queuedFrames_.erase(frame);
}

unsigned int FifoPolicy::selectVictim() {
    if (loadOrder_.empty()) {
        throw std::runtime_error(
            "No hay marcos disponibles para seleccionar una victima FIFO");
    }

    const unsigned int victim = loadOrder_.front();
    loadOrder_.pop();
    queuedFrames_.erase(victim);
    return victim;
}
