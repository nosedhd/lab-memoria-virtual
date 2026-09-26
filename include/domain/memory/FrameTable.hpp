#pragma once

#include <cstdint>
#include <queue>
#include <vector>

struct FrameInfo {
    bool is_allocated{false};
    uint32_t vpn{0};                 // Número de página virtual mapeada
    uint32_t directory_index{0};     // PT1 (para llegar a la tabla correcta)
    uint32_t page_table_index{0};    // PT2 (para llegar a la PTE exacta)
};

class FrameTable {
public:
    explicit FrameTable(uint32_t frame_count);

    // Consultas de estado
    uint32_t getFrameCount() const;
    uint32_t getFreeFrameCount() const;
    bool hasFreeFrame() const;
    bool isAllocated(uint32_t frame) const;

    // Asignación y mapeo
    // Extrae un marco libre del pool y lo asocia a una página virtual
    uint32_t allocateFreeFrame(uint32_t vpn, uint32_t dir_index, uint32_t page_table_index);

    // Asocia o reasocia un marco (útil tanto para asignación como tras un reemplazo)
    void mapFrame(uint32_t frame, uint32_t vpn, uint32_t dir_index, uint32_t page_table_index);

    // Libera un marco y lo regresa al pool de marcos libres (comando free)
    void freeFrame(uint32_t frame);

    // Consulta de metadatos del marco (usado durante el reemplazo)
    const FrameInfo& getFrameInfo(uint32_t frame) const;

private:
    void validateFrameIndex(uint32_t frame) const;

    uint32_t frame_count_;
    std::vector<FrameInfo> frames_;         // Arreglo de tamaño frame_count_
    std::queue<uint32_t> free_frames_pool_; // Cola con marcos libres (0, 1, 2, ..., frame_count - 1)
};
