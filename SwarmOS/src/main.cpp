#include <iostream>
#include "SwarmOS/Kernel/MagnusKernel.hpp"
#include "SwarmOS/Network/PubSubBroker.hpp"

// Structure de consigne de cible envoyée par le bus
struct TargetVectorMessage {
    SwarmOS::Kernel::Vector3 target_position;
};

// Callback d'écoute pour la télémétrie
void on_target_updated(const TargetVectorMessage& msg) {
    std::cout << " [Bus PubSub] Nouvelle cible reçue: (" 
              << msg.target_position.x << ", " 
              << msg.target_position.y << ", " 
              << msg.target_position.z << ")" << std::endl;
}

int main() {
    using namespace SwarmOS::Kernel;
    using namespace SwarmOS::Network;

    std::cout << "=== SwarmOS Boot Sequence ===" << std::endl;

    // 1. Instanciation du Bus PubSub Zéro-Alloc
    PubSubBroker<TargetVectorMessage> target_bus;
    target_bus.subscribe(on_target_updated);

    // 2. État initial & Cible par défaut
    Vector3 state{0.0f, 0.0f, 0.0f};
    Vector3 current_target{10.0f, 10.0f, 5.0f};
    float dt = 0.001f; // Loop 1 kHz

    // Publication d'une nouvelle consigne via le Bus
    TargetVectorMessage new_target{{12.0f, 8.0f, 4.0f}};
    target_bus.publish(new_target);
    current_target = new_target.target_position;

    // 3. Boucle d'intégration Magnus-4 basée sur la consigne
    std::cout << "\nExecuting 1 kHz Magnus-4 Control Loop..." << std::endl;
    for (int i = 0; i < 5; ++i) {
        auto dynamic_potential = [&current_target](const Vector3& pos) noexcept -> Vector3 {
            return current_target - pos;
        };

        state = Magnus4Integrator::compute_next_state(state, dt, dynamic_potential);
        std::cout << "Step " << i + 1 << " -> Pos: (" 
                  << state.x << ", " << state.y << ", " << state.z << ")" << std::endl;
    }

    std::cout << "\n[OK] PubSub + Magnus-4 Pipeline Operational." << std::endl;
    return 0;
}
