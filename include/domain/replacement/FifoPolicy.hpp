#pragma once

#include <queue>
#include <string>
<<<<<<< HEAD
#include <unordered_set>
#include "IReplacementPolicy.hpp"
=======
#include "domain/replacement/IReplacementPolicy.hpp"
>>>>>>> c614b7905b4a2c4d0c2d90c25fa1f99ef56fb78e

class FifoPolicy : public IReplacementPolicy {
public:
    ~FifoPolicy() override = default;
    void onLoad(unsigned int frame) override;
    void onAccess(unsigned int frame) override;
    void onFree(unsigned int frame) override;
    unsigned int selectVictim() override;
    std::string name() const override { return "FIFO"; }
private:
    std::queue<unsigned int> loadOrder_;
    std::unordered_set<unsigned int> queuedFrames_;
};
