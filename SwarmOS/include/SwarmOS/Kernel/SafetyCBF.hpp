#pragma once

#include "MagnusKernel.hpp"
#include <array>
#include <algorithm>
#include <cmath>

namespace SwarmOS::Kernel {

struct Obstacle {
    Vector3 position;
    double radius;
};

template <std::size_t MaxObstacles = 8>
class SafetyCBF {
public:
    constexpr SafetyCBF(double safety_margin = 0.5, double gamma = 1.0) noexcept
        : margin_(safety_margin), gamma_(gamma), obstacle_count_(0) {}

    constexpr bool add_obstacle(const Vector3& pos, double radius) noexcept {
        if (obstacle_count_ >= MaxObstacles) return false;
        obstacles_[obstacle_count_++] = Obstacle{pos, radius};
        return true;
    }

    constexpr void clear_obstacles() noexcept {
        obstacle_count_ = 0;
    }

    [[nodiscard]] constexpr Vector3 filter(const Vector3& state, const Vector3& u_des) const noexcept {
        Vector3 u_safe = u_des;

        for (std::size_t i = 0; i < obstacle_count_; ++i) {
            const auto& obs = obstacles_[i];

            Vector3 r{state.x - obs.position.x, state.y - obs.position.y, state.z - obs.position.z};
            double dist_sq = r.x * r.x + r.y * r.y + r.z * r.z;
            double min_dist = obs.radius + margin_;

            double h = dist_sq - (min_dist * min_dist);
            double dot_grad_u = 2.0 * (r.x * u_safe.x + r.y * u_safe.y + r.z * u_safe.z);
            double constraint_val = dot_grad_u + gamma_ * h;

            if (constraint_val < 0.0) {
                double r_norm_sq = (dist_sq > 1e-6) ? dist_sq : 1e-6;
                double correction_factor = -constraint_val / (4.0 * r_norm_sq);

                u_safe.x += 2.0 * r.x * correction_factor;
                u_safe.y += 2.0 * r.y * correction_factor;
                u_safe.z += 2.0 * r.z * correction_factor;
            }
        }

        return u_safe;
    }

private:
    double margin_;
    double gamma_;
    std::array<Obstacle, MaxObstacles> obstacles_{};
    std::size_t obstacle_count_{0};
};

} // namespace SwarmOS::Kernel
