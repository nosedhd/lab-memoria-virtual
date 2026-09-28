#include <cassert>
#include <stdexcept>

#include "domain/replacement/FifoPolicy.hpp"

void test_victims_follow_load_order() {
    application::FifoPolicy policy;
    policy.onLoad(3);
    policy.onLoad(1);
    policy.onLoad(2);

    assert(policy.selectVictim() == 3);
    assert(policy.selectVictim() == 1);
    assert(policy.selectVictim() == 2);
}

void test_access_does_not_change_order() {
    application::FifoPolicy policy;
    policy.onLoad(0);
    policy.onLoad(1);
    policy.onAccess(0);

    assert(policy.selectVictim() == 0);
}

void test_free_removes_frame_from_the_middle() {
    application::FifoPolicy policy;
    policy.onLoad(0);
    policy.onLoad(1);
    policy.onLoad(2);

    policy.onFree(1);
    policy.onLoad(1);

    assert(policy.selectVictim() == 0);
    assert(policy.selectVictim() == 2);
    assert(policy.selectVictim() == 1);
}

void test_rejects_duplicate_load_and_unknown_free() {
    application::FifoPolicy policy;
    policy.onLoad(5);

    bool duplicate_thrown = false;
    try {
        policy.onLoad(5);
    } catch (const std::logic_error&) {
        duplicate_thrown = true;
    }
    assert(duplicate_thrown);

    bool unknown_thrown = false;
    try {
        policy.onFree(9);
    } catch (const std::logic_error&) {
        unknown_thrown = true;
    }
    assert(unknown_thrown);
}

void test_empty_policy_has_no_victim() {
    application::FifoPolicy policy;

    bool thrown = false;
    try {
        policy.selectVictim();
    } catch (const std::runtime_error&) {
        thrown = true;
    }
    assert(thrown);
}

int main() {
    test_victims_follow_load_order();
    test_access_does_not_change_order();
    test_free_removes_frame_from_the_middle();
    test_rejects_duplicate_load_and_unknown_free();
    test_empty_policy_has_no_victim();
    return 0;
}
