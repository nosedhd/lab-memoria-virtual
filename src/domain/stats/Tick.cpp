#include "domain/stats/Tick.hpp"

void Tick::tick() {
    ++ticks_;
}

uint64_t Tick::now() const {
    return ticks_;
}
