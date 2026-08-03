#include <cassert>
#include <iostream>
#include "SwarmOS/Kernel/MagnusKernel.hpp"
#include "SwarmOS/Network/PubSubBroker.hpp"

// Test 1: Validation static / constexpr des opérations vectorielles
constexpr bool test_vector_ops() {
    using namespace SwarmOS::Kernel;
    Vector3 a{1.0f, 2.0f, 3.0f};
    Vector3 b{4.0f, 5.0f, 6.0f};
    Vector3 c = a + b;
    return (c.x == 5.0f && c.y == 7.0f && c.z == 9.0f);
}
static_assert(test_vector_ops(), "Vector operations static check failed!");

int main() {
    using namespace SwarmOS::Kernel;
    using namespace SwarmOS::Network;

    std::cout << "[TEST] Running SwarmOS Unit Tests..." << std::endl;

    // Test 2: Intégration Magnus-4 (Convergence vers cible)
    Vector3 state{0.0f, 0.0f, 0.0f};
    Vector3 target{1.0f, 1.0f, 1.0f};
    float dt = 0.1f;

    auto field = [&target](const Vector3& pos) noexcept -> Vector3 {
        return target - pos;
    };

    state = Magnus4Integrator::compute_next_state(state, dt, field);
    assert(state.x > 0.0f && "Magnus-4 integration failed to step forward");
    std::cout << "  [PASS] Magnus-4 Integrator Step test" << std::endl;

    // Test 3: PubSub Broker Delivery
    PubSubBroker<int> test_bus;
    bool received = false;
    test_bus.subscribe([&received](const int& val) {
        if (val == 42) received = true;
    });
    test_bus.publish(42);
    assert(received && "PubSub Broker failed message delivery");
    std::cout << "  [PASS] Zero-alloc PubSub Broker test" << std::endl;

    std::cout << "[ALL TESTS PASSED SUCCESSFULLY]" << std::endl;
    return 0;
}
