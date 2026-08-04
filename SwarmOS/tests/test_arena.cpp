#include <iostream>
#include <array>
#include <cmath>
#include "SwarmOS/Bridge/ROS2Bridge.hpp"
#include "SwarmOS/Safety/SafetyCBF.hpp"

int main() {
    std::cout << "=== SWARMOS ARENA: AGENT A vs AGENT B ===" << std::endl;

    // Ponts ROS2 pour les 2 noyaux
    SwarmOS::Bridge::ROS2Bridge<1> bridge_A;
    SwarmOS::Bridge::ROS2Bridge<1> bridge_B;

    // CBF A (Agressif : alpha fort, distance de sécurité plus serrée)
    SwarmOS::Safety::SafetyCBF<1> cbf_A(2.5, 0.8);

    // CBF B (Prudent : alpha doux, distance de sécurité large)
    SwarmOS::Safety::SafetyCBF<1> cbf_B(0.8, 1.5);

    // Positions initiales
    SwarmOS::Bridge::ROSState state_A{{{0.0, 0.0, 0.0}}, {{1.0, 0.0, 0.0}}, 0};
    SwarmOS::Bridge::ROSState state_B{{{5.0, 0.0, 0.0}}, {{-1.0, 0.0, 0.0}}, 0};

    // Consignes de commande (A fonce vers +X, B fonce vers -X -> Trajectoire de collision !)
    SwarmOS::Bridge::ROSCommand cmd_A{{{2.0, 0.0, 0.0}}, 0};
    SwarmOS::Bridge::ROSCommand cmd_B{{{-2.0, 0.0, 0.0}}, 0};

    double dt = 0.1; // 100ms par tick
    for (int tick = 0; tick < 30; ++tick) {
        bridge_A.push_ros_state(state_A);
        bridge_A.push_ros_command(cmd_A);

        bridge_B.push_ros_state(state_B);
        bridge_B.push_ros_command(cmd_B);

        // Préparation des obstacles mutuels
        std::array<SwarmOS::Bridge::ROSState, 1> obs_for_A = {state_B};
        std::array<SwarmOS::Bridge::ROSState, 1> obs_for_B = {state_A};

        // Calcul des commandes filtrées par chaque CBF
        auto safe_cmd_A = bridge_A.step_realtime_loop(cbf_A, obs_for_A, 1);
        auto safe_cmd_B = bridge_B.step_realtime_loop(cbf_B, obs_for_B, 1);

        // Intégration physique basique (Position = Position + Vitesse * dt)
        state_A.position[0] += safe_cmd_A.velocity[0] * dt;
        state_B.position[0] += safe_cmd_B.velocity[0] * dt;

        double dist = std::abs(state_A.position[0] - state_B.position[0]);

        std::cout << "[Tick " << tick << "] "
                  << "Pos A: " << state_A.position[0] << " (Vx: " << safe_cmd_A.velocity[0] << ") | "
                  << "Pos B: " << state_B.position[0] << " (Vx: " << safe_cmd_B.velocity[0] << ") | "
                  << "Dist: " << dist << "m" << std::endl;
    }

    std::cout << "=== COMBAT TERMINE AVEC SUCCES ===" << std::endl;
    return 0;
}
