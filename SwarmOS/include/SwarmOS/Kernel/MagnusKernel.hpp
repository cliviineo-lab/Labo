#pragma once

#include <array>
#include <cmath>
#include <concepts>

namespace SwarmOS::Kernel {

struct Vector3 {
    float x{0.0f};
    float y{0.0f};
    float z{0.0f};

    constexpr Vector3 operator+(const Vector3& rhs) const noexcept {
        return {x + rhs.x, y + rhs.y, z + rhs.z};
    }

    constexpr Vector3 operator-(const Vector3& rhs) const noexcept {
        return {x - rhs.x, y - rhs.y, z - rhs.z};
    }

    constexpr Vector3 operator*(float scalar) const noexcept {
        return {x * scalar, y * scalar, z * scalar};
    }
};

template <typename F>
concept VectorField = requires(F f, Vector3 v) {
    { f(v) } -> std::same_as<Vector3>;
};

class Magnus4Integrator {
public:
    template <VectorField F>
    static constexpr Vector3 compute_next_state(const Vector3& current_state, float dt, F&& field) noexcept {
        // Gauss-Legendre quadrature points
        constexpr float c1 = 0.5f - 0.28867513459481287f; // 0.5 - sqrt(3)/6
        constexpr float c2 = 0.5f + 0.28867513459481287f; // 0.5 + sqrt(3)/6

        Vector3 k1 = field(current_state + field(current_state) * (c1 * dt));
        Vector3 k2 = field(current_state + field(current_state) * (c2 * dt));

        // Integration step O(dt^4)
        Vector3 omega = (k1 + k2) * (0.5f * dt);
        return current_state + omega;
    }
};

} // namespace SwarmOS::Kernel
