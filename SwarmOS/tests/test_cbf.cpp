#include "SwarmOS/Kernel/SafetyCBF.hpp"
#include <cassert>
#include <iostream>

int main() {
    using namespace SwarmOS;

    Kernel::SafetyCBF<4> cbf(0.5, 2.0);

    Vector3 drone_pos{0.0, 0.0, 0.0};
    cbf.add_obstacle(Vector3{2.0, 0.0, 0.0}, 0.5);

    Vector3 unsafe_cmd{5.0, 0.0, 0.0};
    Vector3 safe_cmd = cbf.filter(drone_pos, unsafe_cmd);

    std::cout << "Commande desiree  : X=" << unsafe_cmd.x << "\n";
    std::cout << "Commande securisee : X=" << safe_cmd.x << "\n";

    assert(safe_cmd.x < unsafe_cmd.x);
    assert(safe_cmd.x <= 1.5);

    std::cout << "Test SafetyCBF valide (0-Alloc, Inviolable) !\n";
    return 0;
}
