#pragma once

#include "domain/replacement/IReplacementPolicy.hpp"
#include <vector>
#include <cstdint>
#include <string>


class LRUPolicy : public IReplacementPolicy {
public:
    explicit LRUPolicy(unsigned int capacity);
    ~LRUPolicy() override = default;

    void onLoad(unsigned int index) override;
    void onAccess(unsigned int index) override;
    void onFree(unsigned int index) override;
    unsigned int selectVictim() override;
    std::string name() const override;

private:
    std::vector<uint64_t> last_accessed_;
    std::vector<bool> active_;
    uint64_t global_counter_;

    void validateIndex(unsigned int index) const;
};

