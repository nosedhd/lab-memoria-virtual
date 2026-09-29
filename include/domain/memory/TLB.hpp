#pragma once

#include "domain/config/MemoryConfig.hpp"
#include "domain/replacement/IReplacementPolicy.hpp"
#include <vector>
#include <memory>
#include <cstdint>
#include <optional>

struct TLBEntry {
    uint32_t vpn{0};
    uint32_t frame{0};
    bool valid{false};
};

class TLB {
public:
    static constexpr size_t CAPACITY = MemoryConfig::TLB_SIZE;

    explicit TLB(size_t capacity = CAPACITY);

    std::optional<uint32_t> lookup(uint32_t vpn);
    void insert(uint32_t vpn, uint32_t frame);
    bool update(uint32_t vpn, uint32_t frame);
    void invalidate(uint32_t vpn);
    void clear();

private:
    std::vector<TLBEntry> entries_;
    std::unique_ptr<IReplacementPolicy> policy_;
};
