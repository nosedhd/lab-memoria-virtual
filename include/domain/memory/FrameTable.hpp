#pragma once

#include <cstdint>
#include <queue>
#include <vector>

struct FrameInfo {
    bool is_allocated{false};
    uint32_t vpn{0};                 
    uint32_t directory_index{0};     
    uint32_t page_table_index{0};    
};

class FrameTable {
public:
    explicit FrameTable(uint32_t frame_count);

    // Consultas de estado
    uint32_t getFrameCount() const;
    uint32_t getFreeFrameCount() const;
    bool hasFreeFrame() const;
    bool isAllocated(uint32_t frame) const;

    uint32_t allocateFreeFrame(uint32_t vpn, uint32_t dir_index, uint32_t page_table_index);

    void mapFrame(uint32_t frame, uint32_t vpn, uint32_t dir_index, uint32_t page_table_index);

    void freeFrame(uint32_t frame);

    const FrameInfo& getFrameInfo(uint32_t frame) const;

private:
    void validateFrameIndex(uint32_t frame) const;

    uint32_t frame_count_;
    std::vector<FrameInfo> frames_;         
    std::queue<uint32_t> free_frames_pool_; 
};
