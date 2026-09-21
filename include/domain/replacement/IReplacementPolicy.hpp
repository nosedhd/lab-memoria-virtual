#pragma once

#include <string>

namespace domain {
class IReplacementPolicy {
public:
    virtual ~IReplacementPolicy() = default;
    virtual void onLoad(unsigned int frame) = 0;
    virtual void onAccess(unsigned int frame) = 0;
    virtual unsigned int selectVictim() = 0;
    virtual std::string name() const = 0;
};
}