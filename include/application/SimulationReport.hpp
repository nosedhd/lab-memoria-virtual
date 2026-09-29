#pragma once

#include <cstdint>
#include <string>
#include <vector>

struct SimulationReport {
    std::string policy_name;
    uint32_t page_size{0};
    uint32_t physical_memory_size{0};
    uint32_t frame_count{0};

    uint64_t instructions_executed{0};
    uint64_t total_accesses{0};
    uint64_t page_faults{0};
    uint64_t replacements{0};
    uint64_t tlb_hits{0};
    double hit_rate{0.0};

    uint64_t elapsed_ticks{0};
    std::vector<std::string> errors;
};
