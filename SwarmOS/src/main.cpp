cat << 'EOF' > src/main.cpp
#include <iostream>
#include "SwarmOS/Kernel/MagnusKernel.hpp"

int main() {
    using namespace SwarmOS::Kernel;

    Vector3 state{0.0f, 0.0f, 0.0f};
    float dt = 0.001f; // 1 kHz

    auto potential_field = [](const Vector3& pos) noexcept -> Vector3 {
        return {10.0f - pos.x, 10.0f - pos.y, 5.0f - pos.z};
    };

    std::cout << "=== SwarmOS Boot ===" << std::endl;
    std::cout << "Running Magnus-4 Lie Integrator @ 1 kHz..." << std::endl;

    for (int i = 0; i < 5; ++i) {
        state = Magnus4Integrator::compute_next_state(state, dt, potential_field);
        std::cout << "Step " << i + 1 << " -> Pos: (" 
                  << state.x << ", " << state.y << ", " << state.z << ")" << std::endl;
    }

    std::cout << "Kernel OK!" << std::endl;
    return 0;
}
EOF
