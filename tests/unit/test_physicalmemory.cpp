#include <cassert>
#include <stdexcept>

#include "domain/memory/PhysicalMemory.hpp"

void test_initialization() {
    PhysicalMemory mem(1024);

    assert(mem.getSize() == 1024);

    // Debe inicializar toda la memoria en 0
    assert(mem.readByte(0) == 0);
    assert(mem.readByte(512) == 0);
    assert(mem.readByte(1023) == 0);
}

void test_read_and_write() {
    PhysicalMemory mem(4096);

    // Escribir bytes en distintas posiciones
    mem.writeByte(0, 42);
    mem.writeByte(100, 255);
    mem.writeByte(4095, 99);

    // Leer y comprobar integridad
    assert(mem.readByte(0) == 42);
    assert(mem.readByte(100) == 255);
    assert(mem.readByte(4095) == 99);

    // Posiciones no tocadas deben seguir en 0
    assert(mem.readByte(1) == 0);
    assert(mem.readByte(101) == 0);
    assert(mem.readByte(4094) == 0);

    // Sobreescritura
    mem.writeByte(0, 77);
    assert(mem.readByte(0) == 77);
}

void test_clear_range() {
    PhysicalMemory mem(8192);
    mem.writeByte(4095, 11);
    mem.writeByte(4096, 22);
    mem.writeByte(5000, 33);
    mem.writeByte(8191, 44);

    mem.clearRange(4096, 4096);

    assert(mem.readByte(4095) == 11);
    assert(mem.readByte(4096) == 0);
    assert(mem.readByte(5000) == 0);
    assert(mem.readByte(8191) == 0);

    bool thrown = false;
    try {
        mem.clearRange(4096, 4097);
    } catch (const std::out_of_range&) {
        thrown = true;
    }
    assert(thrown);
}

void test_rejects_invalid_arguments() {
    // Tamaño 0 debe lanzar invalid_argument
    bool thrown = false;
    try {
        PhysicalMemory mem(0);
    } catch (const std::invalid_argument&) {
        thrown = true;
    }
    assert(thrown);

    PhysicalMemory mem(256);

    // Lectura fuera de rango
    thrown = false;
    try {
        mem.readByte(256);
    } catch (const std::out_of_range&) {
        thrown = true;
    }
    assert(thrown);

    // Escritura fuera de rango
    thrown = false;
    try {
        mem.writeByte(256, 10);
    } catch (const std::out_of_range&) {
        thrown = true;
    }
    assert(thrown);
}

int main() {
    test_clear_range();
    test_initialization();
    test_read_and_write();
    test_rejects_invalid_arguments();
    return 0;
}
