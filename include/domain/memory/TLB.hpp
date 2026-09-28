#pragma once

#include "domain/config/MemoryConfig.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>

struct TLBEntry {
    uint32_t vpn{0};
    uint32_t frame{0};
    bool valid{false};
    uint64_t last_accessed{0}; // Para LRU dentro de la TLB
};

class TLB {
public:
    static constexpr std::size_t CAPACITY = MemoryConfig::TLB_SIZE;

    std::optional<uint32_t> lookup(uint32_t vpn);
    void insert(uint32_t vpn, uint32_t frame);
    bool update(uint32_t vpn, uint32_t frame);
    void invalidate(uint32_t vpn);
    void clear();

private:
    std::array<TLBEntry, MemoryConfig::TLB_SIZE> entries_{};
    uint64_t access_counter_{0};
};