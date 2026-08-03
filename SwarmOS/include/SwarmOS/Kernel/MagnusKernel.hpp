cd Kernel
cat << 'EOF' > MagnusKernel.hpp
#pragma once

#include <array>
#include <cmath>

namespace SwarmOS::Kernel {

// ReprC)sentation minimale SO(3) / AlgC(bre de Lie (vecteur de rotation 3D)
struct Vector3 {
    float x{0.0f}, y{0.0f}, z{0.0f};

    constexpr Vector3 operator+(const Vector3& v) const noexcept { return {x + v.x, y + v.y, z + v.z}; }
    constexpr Vector3 operator-(const Vector3& v) const noexcept { return {x - v.x, y - v.y, z - v.z}; }
    constexpr Vector3 operator*(float s) const noexcept { return {x * s, y * s, z * s}; }
};

// Lie Bracket (Crochet de Lie [A, B] = Produit Vectoriel dans so(3))
constexpr Vector3 lie_bracket(const Vector3& a, const Vector3& b) noexcept {
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

class Magnus4Integrator {
public:
    // Constantes de Gauss-Legendre pour Magnus d'Ordre 4
    static constexpr float c1 = 0.5f - 0.28867513459f; // 0.5 - sqrt(3)/6
    static constexpr float c2 = 0.5f + 0.28867513459f; // 0.5 + sqrt(3)/6

    template <typename PotentialEvaluator>
    static Vector3 compute_next_state(const Vector3& current_state, float dt, PotentialEvaluator&& eval_potential) noexcept {
        // Guess initial dans l'algC(bre de Lie
        Vector3 omega1 = eval_potential(current_state) * (dt * c1);
        Vector3 omega2 = eval_potential(current_state) * (dt * c2);

        // Boucle Newton-Raphson dC)terministe k = 2 itC)rations max (1 kHz safe)
        for (int k = 0; k < 2; ++k) {
            Vector3 A1 = eval_potential(current_state + omega1);
            Vector3 A2 = eval_potential(current_state + omega2);
            Vector3 comm = lie_bracket(omega1, omega2);

            Vector3 res1 = omega1 - (A1 * c1 + comm * 0.125f) * dt;
            Vector3 res2 = omega2 - (A2 * c2 - comm * 0.125f) * dt;

            // Pas de mise C  jour O(1)
            omega1 = omega1 - res1;
            omega2 = omega2 - res2;
        }

        // Assemblage du gC)nC)rateur effectif Magnus 4
        Vector3 A_final1 = eval_potential(current_state + omega1);
        Vector3 A_final2 = eval_potential(current_state + omega2);
        Vector3 final_comm = lie_bracket(omega1, omega2);

        Vector3 omega_eff = (A_final1 + A_final2) * (dt * 0.5f) + final_comm * ((dt * dt) / 6.92820323f);

        // Projection par mise C  jour
        return current_state + omega_eff;
    }
};

} // namespace SwarmOS::Kernel
EOF
