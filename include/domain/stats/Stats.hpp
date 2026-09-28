#pragma once

#include <cstdint>

class Stats {
public:
    void recordAccess();
    void recordPageFault();
    void recordReplacement();
    void recordTlbHit();

    uint64_t getTotalAccesses() const;
    uint64_t getPageFaults() const;
    uint64_t getReplacements() const;
    uint64_t getTlbHits() const;

    double getHitRate() const; // (N - M) / N * 100%

private:
    uint64_t total_accesses_{0};
    uint64_t page_faults_{0};
    uint64_t replacements_{0};
    uint64_t tlb_hits_{0};
};