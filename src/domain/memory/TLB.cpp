#include "domain/memory/TLB.hpp"

#include <algorithm>

std::optional<uint32_t> TLB::lookup(uint32_t vpn) {
    for (auto& entry : entries_) {
        if (entry.valid && entry.vpn == vpn) {
            entry.last_accessed = ++access_counter_;
            return entry.frame;
        }
    }

    return std::nullopt;
}

void TLB::insert(uint32_t vpn, uint32_t frame) {
    // PASO 1 (UPDATE): Si el VPN ya existe en la TLB, actualizar su marco y tiempo:
    for (auto& entry : entries_) {
        if (entry.valid && entry.vpn == vpn) {
            entry.frame = frame;
            entry.last_accessed = ++access_counter_;
            return;
        }
    }

    // PASO 2: Si no existía, buscar una entrada vacía:
    for (auto& entry : entries_) {
        if (!entry.valid) {
            entry.vpn = vpn;
            entry.frame = frame;
            entry.valid = true;
            entry.last_accessed = ++access_counter_;
            return;
        }
    }

    // PASO 3: Si la TLB está llena, desalojar la víctima LRU:
    auto lru_entry = std::min_element(
        entries_.begin(), entries_.end(),
        [](const TLBEntry& a, const TLBEntry& b) {
            return a.last_accessed < b.last_accessed;
        });

    lru_entry->vpn = vpn;
    lru_entry->frame = frame;
    lru_entry->valid = true;
    lru_entry->last_accessed = ++access_counter_;
}

bool TLB::update(uint32_t vpn, uint32_t frame) {
    for (auto& entry : entries_) {
        if (entry.valid && entry.vpn == vpn) {
            entry.frame = frame;
            entry.last_accessed = ++access_counter_;
            return true;
        }
    }
    return false; 
}

void TLB::invalidate(uint32_t vpn) {
    for (auto& entry : entries_) {
        if (entry.valid && entry.vpn == vpn) {
            entry.valid = false;
            return;
        }
    }
}

void TLB::clear() {
    for (auto& entry : entries_) {
        entry.valid = false;
    }
    access_counter_ = 0;
}

