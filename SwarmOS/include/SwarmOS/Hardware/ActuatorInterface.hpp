#pragma once

#include <array>
#include <cstdint>
#include "SwarmOS/Kernel/MagnusKernel.hpp"

namespace SwarmOS::Hardware {

// Représente les commandes physiques envoyées aux rotors / moteurs
struct MotorCommand {
    std::array<float, 4> motor_speeds{0.0f, 0.0f, 0.0f, 0.0f}; // PWM / Vitesse normalisée [0.0 - 1.0]
    uint64_t timestamp_us{0};
};

class ActuatorInterface {
public:
    constexpr ActuatorInterface() noexcept = default;

    // Convertit le vecteur d'état Magnus SO(3) en commandes moteurs (Mixing matrix 4x4)
    static constexpr MotorCommand map_state_to_motors(const Kernel::Vector3& state) noexcept {
        MotorCommand cmd{};
        
        // Algorithme de mixage statique O(1) pour configuration quadri-rotor
        cmd.motor_speeds[0] = state.x + state.y + state.z;
        cmd.motor_speeds[1] = state.x - state.y - state.z;
        cmd.motor_speeds[2] = state.x - state.y + state.z;
        cmd.motor_speeds[3] = state.x + state.y - state.z;

        // Normalisation / Clamping sans allocation
        for (auto& speed : cmd.motor_speeds) {
            if (speed < 0.0f) speed = 0.0f;
            if (speed > 1.0f) speed = 1.0f;
        }

        return cmd;
    }
};

} // namespace SwarmOS::Hardware
