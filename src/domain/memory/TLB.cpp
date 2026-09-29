#include "domain/memory/TLB.hpp"
#include "domain/replacement/LRUPolicy.hpp"
#include <stdexcept>

TLB::TLB(size_t capacity)
    : entries_(capacity),
      policy_(std::make_unique<LRUPolicy>(static_cast<unsigned int>(capacity))) {
    if (capacity == 0) {
        throw std::invalid_argument("La capacidad de la TLB debe ser mayor a 0.");
    }
}

std::optional<uint32_t> TLB::lookup(uint32_t vpn) {
    for (size_t i = 0; i < entries_.size(); ++i) {
        if (entries_[i].valid && entries_[i].vpn == vpn) {
            policy_->onAccess(static_cast<unsigned int>(i)); // Registra el hit en LRU
            return entries_[i].frame;
        }
    }
    return std::nullopt; // TLB Miss
}

void TLB::insert(uint32_t vpn, uint32_t frame) {
    // PASO 1 (UPDATE): Si el VPN ya existe, actualizar marco y registrar acceso
    for (size_t i = 0; i < entries_.size(); ++i) {
        if (entries_[i].valid && entries_[i].vpn == vpn) {
            entries_[i].frame = frame;
            policy_->onAccess(static_cast<unsigned int>(i));
            return;
        }
    }

    // PASO 2: Buscar una entrada libre
    for (size_t i = 0; i < entries_.size(); ++i) {
        if (!entries_[i].valid) {
            entries_[i].vpn = vpn;
            entries_[i].frame = frame;
            entries_[i].valid = true;
            policy_->onLoad(static_cast<unsigned int>(i));
            return;
        }
    }

    // PASO 3: TLB llena — pedir víctima LRU a la política
    const unsigned int victim = policy_->selectVictim();
    policy_->onFree(victim);

    entries_[victim].vpn = vpn;
    entries_[victim].frame = frame;
    entries_[victim].valid = true;
    policy_->onLoad(victim);
}

bool TLB::update(uint32_t vpn, uint32_t frame) {
    for (size_t i = 0; i < entries_.size(); ++i) {
        if (entries_[i].valid && entries_[i].vpn == vpn) {
            entries_[i].frame = frame;
            policy_->onAccess(static_cast<unsigned int>(i));
            return true;
        }
    }
    return false; // No estaba en la TLB
}

void TLB::invalidate(uint32_t vpn) {
    for (size_t i = 0; i < entries_.size(); ++i) {
        if (entries_[i].valid && entries_[i].vpn == vpn) {
            entries_[i].valid = false;
            policy_->onFree(static_cast<unsigned int>(i));
            return;
        }
    }
}

void TLB::clear() {
    for (size_t i = 0; i < entries_.size(); ++i) {
        if (entries_[i].valid) {
            policy_->onFree(static_cast<unsigned int>(i));
        }
        entries_[i].valid = false;
    }
}
