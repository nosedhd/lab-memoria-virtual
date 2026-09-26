#include <cassert>
#include <stdexcept>

#include "domain/memory/FrameTable.hpp"

void test_initial_state() {
    FrameTable table(4);

    assert(table.getFrameCount() == 4);
    assert(table.getFreeFrameCount() == 4);
    assert(table.hasFreeFrame());

    for (uint32_t i = 0; i < 4; ++i) {
        assert(!table.isAllocated(i));
        const FrameInfo& info = table.getFrameInfo(i);
        assert(!info.is_allocated);
        assert(info.vpn == 0);
        assert(info.directory_index == 0);
        assert(info.page_table_index == 0);
    }
}

void test_allocate_and_free_frames() {
    FrameTable table(3);

    uint32_t f0 = table.allocateFreeFrame(100, 1, 10);
    uint32_t f1 = table.allocateFreeFrame(200, 2, 20);
    uint32_t f2 = table.allocateFreeFrame(300, 3, 30);

    assert(f0 == 0);
    assert(f1 == 1);
    assert(f2 == 2);

    assert(table.getFreeFrameCount() == 0);
    assert(!table.hasFreeFrame());

    for (uint32_t i = 0; i < 3; ++i) {
        assert(table.isAllocated(i));
    }

    assert(table.getFrameInfo(0).vpn == 100);
    assert(table.getFrameInfo(0).directory_index == 1);
    assert(table.getFrameInfo(0).page_table_index == 10);

    // Intentar asignar cuando esta lleno debe lanzar excepcion
    bool thrown = false;
    try {
        table.allocateFreeFrame(400, 4, 40);
    } catch (const std::runtime_error&) {
        thrown = true;
    }
    assert(thrown);

    // Liberar el marco 1
    table.freeFrame(1);
    assert(!table.isAllocated(1));
    assert(table.getFreeFrameCount() == 1);
    assert(table.hasFreeFrame());

    // Asignar de nuevo debe entregar el marco 1
    uint32_t recycled = table.allocateFreeFrame(500, 5, 50);
    assert(recycled == 1);
    assert(table.isAllocated(1));
    assert(table.getFrameInfo(1).vpn == 500);

    // Intentar liberar un marco ya libre debe lanzar logic_error
    table.freeFrame(0);
    thrown = false;
    try {
        table.freeFrame(0);
    } catch (const std::logic_error&) {
        thrown = true;
    }
    assert(thrown);
}

void test_map_frame() {
    FrameTable table(2);

    uint32_t frame = table.allocateFreeFrame(10, 0, 1);
    assert(table.getFrameInfo(frame).vpn == 10);

    // Remapear por reemplazo de victima
    table.mapFrame(frame, 999, 15, 30);
    assert(table.isAllocated(frame));
    assert(table.getFrameInfo(frame).vpn == 999);
    assert(table.getFrameInfo(frame).directory_index == 15);
    assert(table.getFrameInfo(frame).page_table_index == 30);
}

void test_rejects_invalid_arguments() {
    bool thrown = false;
    try {
        FrameTable table(0);
    } catch (const std::invalid_argument&) {
        thrown = true;
    }
    assert(thrown);

    FrameTable table(2);

    thrown = false;
    try {
        table.isAllocated(2);
    } catch (const std::out_of_range&) {
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try {
        table.getFrameInfo(2);
    } catch (const std::out_of_range&) {
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try {
        table.freeFrame(2);
    } catch (const std::out_of_range&) {
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try {
        table.mapFrame(2, 0, 0, 0);
    } catch (const std::out_of_range&) {
        thrown = true;
    }
    assert(thrown);
}

int main() {
    test_initial_state();
    test_allocate_and_free_frames();
    test_map_frame();
    test_rejects_invalid_arguments();
    return 0;
}
