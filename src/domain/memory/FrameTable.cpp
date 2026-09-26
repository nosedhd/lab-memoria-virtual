#include "domain/memory/FrameTable.hpp"

#include <stdexcept>
#include <string>

FrameTable::FrameTable(uint32_t frame_count)
    : frame_count_(frame_count),
      frames_(frame_count) {
    if (frame_count == 0) {
        throw std::invalid_argument("El numero de marcos debe ser mayor a 0");
    }
    for (uint32_t frame = 0; frame < frame_count_; ++frame) {
        free_frames_pool_.push(frame);
    }
}


uint32_t FrameTable::getFrameCount() const {
    return frame_count_;
}

uint32_t FrameTable::getFreeFrameCount() const {
    return static_cast<uint32_t>(free_frames_pool_.size());
}

bool FrameTable::hasFreeFrame() const {
    return !free_frames_pool_.empty();
}

bool FrameTable::isAllocated(uint32_t frame) const {
    validateFrameIndex(frame);
    return frames_[frame].is_allocated;
}

uint32_t FrameTable::allocateFreeFrame(
    uint32_t vpn, uint32_t dir_index, uint32_t page_table_index) {
    if (!hasFreeFrame()) {
        throw std::runtime_error("No hay marcos fisicos libres");
    }

    const uint32_t frame = free_frames_pool_.front();
    free_frames_pool_.pop();
    
    frames_[frame] = FrameInfo{true, vpn, dir_index, page_table_index};
    return frame;
}

void FrameTable::mapFrame(
    uint32_t frame, uint32_t vpn, uint32_t dir_index, uint32_t page_table_index) {
    validateFrameIndex(frame);

    frames_[frame] = FrameInfo{true, vpn, dir_index, page_table_index};
}


void FrameTable::freeFrame(uint32_t frame) {
    validateFrameIndex(frame);

    if (!frames_[frame].is_allocated) {
        throw std::logic_error("El marco ya esta libre");
    }

    frames_[frame] = FrameInfo{};
    free_frames_pool_.push(frame);
}

const FrameInfo& FrameTable::getFrameInfo(uint32_t frame) const {
    validateFrameIndex(frame);
    return frames_[frame];
}

void FrameTable::validateFrameIndex(uint32_t frame) const {
    if (frame >= frame_count_) {
        throw std::out_of_range(
            "Indice de marco fuera de rango: " + std::to_string(frame));
    }
}
