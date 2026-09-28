#pragma once

#include <cstdint>

class Tick {
public:
    void tick();
    uint64_t now() const;

private:
    uint64_t ticks_{0};
};
