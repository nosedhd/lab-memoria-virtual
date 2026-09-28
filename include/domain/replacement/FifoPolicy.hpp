#pragma once

#include <list>
#include <string>
#include "IReplacementPolicy.hpp"

namespace application {
class FifoPolicy : public domain::IReplacementPolicy {
public:
    ~FifoPolicy() override = default;
    void onLoad(unsigned int frame) override;
    void onAccess(unsigned int frame) override;   // no hace nada en FIFO
    void onFree(unsigned int frame) override;
    unsigned int selectVictim() override;
    std::string name() const override { return "FIFO"; }
private:
    bool contains(unsigned int frame) const;

    std::list<unsigned int> loadOrder_;
};
}
