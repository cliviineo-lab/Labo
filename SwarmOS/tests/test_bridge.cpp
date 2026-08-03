#include <iostream>
#include <cassert>
#include "SwarmOS/Bridge/ROS2Bridge.hpp"
#include "SwarmOS/Safety/SafetyCBF.hpp"

int main() {
    SwarmOS::Bridge::ROS2Bridge<5> bridge;
    SwarmOS::Safety::SafetyCBF<5> cbf(0.5, 1.0); // Safety margin 0.5m, gamma 1.0

    // Simulation d'une commande ROS 2 reçue (ex: avancer à 2.0 m/s sur X)
    SwarmOS::Bridge::ROSCommand incoming_cmd{{{2.0, 0.0, 0.0}}, 1000};
    bridge.push_ros_command(incoming_cmd);

    // État du drone (en 0,0,0)
    SwarmOS::Bridge::ROSState state{{{0.0, 0.0, 0.0}}, {{0.0, 0.0, 0.0}}, 1000};
    bridge.push_ros_state(state);

    // Obstacle sur le chemin en (1.0, 0.0, 0.0)
    std::array<SwarmOS::Bridge::ROSState, 5> obstacles{};
    obstacles[0].position = {1.0, 0.0, 0.0};

    // Exécution de la boucle temps réel
    auto safe_output = bridge.step_realtime_loop(cbf, obstacles, 1);

    std::cout << "[ROS2Bridge Test] Desired X: " << incoming_cmd.velocity[0] 
              << " -> Safe Output X: " << safe_output.velocity[0] << std::endl;

    // La vitesse safe doit être réduite pour éviter l'obstacle
    assert(safe_output.velocity[0] < 2.0);
    std::cout << "✅ ROS2Bridge Lock-free & CBF Filter Test Passed!" << std::endl;

    return 0;
}
