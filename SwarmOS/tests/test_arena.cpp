#include <iostream>
#include <array>
#include <cmath>
#include <algorithm>
#include "SwarmOS/Bridge/ROS2Bridge.hpp"
#include "SwarmOS/Safety/SafetyCBF.hpp"

struct Vector3 {
    double x, y, z;
    double norm() const { return std::sqrt(x*x + y*y + z*z); }
    Vector3 operator-(const Vector3& o) const { return {x - o.x, y - o.y, z - o.z}; }
    Vector3 operator+(const Vector3& o) const { return {x + o.x, y + o.y, z + o.z}; }
    Vector3 operator*(double scalar) const { return {x * scalar, y * scalar, z * scalar}; }
};

int main() {
    std::cout << "=== SWARMOS ARENA 3D: GRADIENT + CBF ESCAPE ===" << std::endl;

    Vector3 pos_A{0.0, 0.0, 1.0}, target_A{10.0, 0.0, 1.0};
    Vector3 pos_B{10.0, 0.0, 1.0}, target_B{0.0, 0.0, 1.0};

    double dt = 0.1;
    double k_p = 1.0; 

    for (int tick = 0; tick < 60; ++tick) {
        Vector3 grad_A = (target_A - pos_A);
        Vector3 v_nom_A = grad_A * (k_p / std::max(1.0, grad_A.norm()));

        Vector3 grad_B = (target_B - pos_B);
        Vector3 v_nom_B = grad_B * (k_p / std::max(1.0, grad_B.norm()));

        Vector3 rel_pos = pos_A - pos_B;
        double dist = rel_pos.norm();

        Vector3 v_safe_A = v_nom_A;
        Vector3 v_safe_B = v_nom_B;

        if (dist < 4.0) {
            double evasion_factor = (4.0 - dist) * 0.8;
            Vector3 evasion_A = {0.0, 1.2 * evasion_factor, 0.5 * evasion_factor};
            Vector3 evasion_B = {0.0, -1.2 * evasion_factor, -0.5 * evasion_factor};
            
            v_safe_A = v_nom_A + evasion_A;
            v_safe_B = v_nom_B + evasion_B;
        }

        pos_A = pos_A + v_safe_A * dt;
        pos_B = pos_B + v_safe_B * dt;

        std::cout << "[Tick " << tick << "] "
                  << "A: (" << pos_A.x << ", " << pos_A.y << ", " << pos_A.z << ") | "
                  << "B: (" << pos_B.x << ", " << pos_B.y << ", " << pos_B.z << ") | "
                  << "Dist: " << dist << "m" << std::endl;
    }

    std::cout << "=== FIN DE COMBAT 3D ===" << std::endl;
    return 0;
}
