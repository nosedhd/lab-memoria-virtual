#include "domain/stats/Stats.hpp"

void Stats::recordAccess() {
    ++total_accesses_;
}

void Stats::recordPageFault() {
    ++page_faults_;
}

void Stats::recordReplacement() {
    ++replacements_;
}

void Stats::recordTlbHit() {
    ++tlb_hits_;
}

uint64_t Stats::getTotalAccesses() const {
    return total_accesses_;
}

uint64_t Stats::getPageFaults() const {
    return page_faults_;
}

uint64_t Stats::getReplacements() const {
    return replacements_;
}

uint64_t Stats::getTlbHits() const {
    return tlb_hits_;
}

double Stats::getHitRate() const {
    if (total_accesses_ == 0) {
        return 0.0;
    }
    return static_cast<double>(total_accesses_ - page_faults_) / total_accesses_ * 100.0;
}