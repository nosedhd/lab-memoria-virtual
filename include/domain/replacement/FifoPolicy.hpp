#pragma once

#include <queue>
#include <string>
#include "domain/replacement/IReplacementPolicy.hpp"

namespace application {
class FifoPolicy : public domain::IReplacementPolicy {
public:
    ~FifoPolicy() override = default;
    void onLoad(unsigned int frame) override;
    void onAccess(unsigned int frame) override;   // no hace nada en FIFO
    unsigned int selectVictim() override;
    std::string name() const override { return "FIFO"; }
private:
    std::queue<unsigned int> loadOrder_;
};
}