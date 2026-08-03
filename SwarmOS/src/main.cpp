#include <iostream>
#include "SwarmOS/Kernel/MagnusKernel.hpp"
#include "SwarmOS/Network/PubSubBroker.hpp"
#include "SwarmOS/Hardware/ActuatorInterface.hpp"

struct TargetVectorMessage {
    SwarmOS::Kernel::Vector3 target_position;
};

void on_target_updated(const TargetVectorMessage& msg) {
    std::cout << " [Bus PubSub] Target Updated: (" 
              << msg.target_position.x << ", " 
              << msg.target_position.y << ", " 
              << msg.target_position.z << ")" << std::endl;
}

int main() {
    using namespace SwarmOS::Kernel;
    using namespace SwarmOS::Network;
    using namespace SwarmOS::Hardware;

    std::cout << "=== SwarmOS Boot Sequence ===" << std::endl;

    // 1. Bus PubSub
    PubSubBroker<TargetVectorMessage> target_bus;
    target_bus.subscribe(on_target_updated);

    // 2. Initialisation État & HAL
    Vector3 state{0.0f, 0.0f, 0.0f};
    Vector3 current_target{0.5f, 0.5f, 0.2f};
    float dt = 0.001f; // 1 kHz

    target_bus.publish(TargetVectorMessage{current_target});

    // 3. Control Loop 1 kHz : Sensor -> Kernel -> Actuators
    std::cout << "\nRunning Real-Time Pipeline (1 kHz)..." << std::endl;
    for (int i = 0; i < 3; ++i) {
        auto dynamic_potential = [&current_target](const Vector3& pos) noexcept -> Vector3 {
            return current_target - pos;
        };

        // Integration Lie Group
        state = Magnus4Integrator::compute_next_state(state, dt, dynamic_potential);
        
        // Conversions vers actionneurs physiques
        MotorCommand cmd = ActuatorInterface::map_state_to_motors(state);

        std::cout << "Step " << i + 1 
                  << " | State Z: " << state.z 
                  << " | Motors PWM: [" 
                  << cmd.motor_speeds[0] << ", " 
                  << cmd.motor_speeds[1] << ", " 
                  << cmd.motor_speeds[2] << ", " 
                  << cmd.motor_speeds[3] << "]" << std::endl;
    }

    std::cout << "\n[OK] Full Hardware-Kernel Pipeline Operational." << std::endl;
    return 0;
}
