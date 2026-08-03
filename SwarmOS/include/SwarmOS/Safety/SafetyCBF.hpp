#ifndef SWARMOS_SAFETY_CBF_HPP
#define SWARMOS_SAFETY_CBF_HPP

#include <array>
#include <cmath>
#include <algorithm>

namespace SwarmOS::Safety {

template <size_t MaxObstacles = 10>
class SafetyCBF {
public:
    struct State {
        std::array<double, 3> position{0.0, 0.0, 0.0};
        std::array<double, 3> velocity{0.0, 0.0, 0.0};
    };

    struct Command {
        std::array<double, 3> desired_velocity{0.0, 0.0, 0.0};
    };

    struct OutputCommand {
        std::array<double, 3> safe_velocity{0.0, 0.0, 0.0};
        bool cbf_active{false};
    };

    constexpr explicit SafetyCBF(double min_dist = 1.0, double gamma = 1.0) noexcept
        : min_dist_(min_dist), gamma_(gamma) {}

    [[nodiscard]] OutputCommand filter(
        const State& state,
        const Command& cmd,
        const std::array<State, MaxObstacles>& obstacles,
        size_t obstacle_count) const noexcept 
    {
        OutputCommand out;
        out.safe_velocity = cmd.desired_velocity;
        out.cbf_active = false;

        const size_t count = std::min(obstacle_count, MaxObstacles);

        for (size_t i = 0; i < count; ++i) {
            double dx = state.position[0] - obstacles[i].position[0];
            double dy = state.position[1] - obstacles[i].position[1];
            double dz = state.position[2] - obstacles[i].position[2];

            double dist_sq = dx * dx + dy * dy + dz * dz;
            double dist = std::sqrt(dist_sq);

            if (dist < 1e-6) continue;

            double h = dist - min_dist_;

            if (h < 0.5) {
                out.cbf_active = true;
                double nx = dx / dist;
                double ny = dy / dist;
                double nz = dz / dist;

                double v_rel = out.safe_velocity[0] * nx + 
                               out.safe_velocity[1] * ny + 
                               out.safe_velocity[2] * nz;

                if (v_rel < gamma_ * h) {
                    double correction = (gamma_ * h) - v_rel;
                    out.safe_velocity[0] += correction * nx;
                    out.safe_velocity[1] += correction * ny;
                    out.safe_velocity[2] += correction * nz;
                }
            }
        }

        return out;
    }

private:
    double min_dist_{1.0};
    double gamma_{1.0};
};

} // namespace SwarmOS::Safety

#endif // SWARMOS_SAFETY_CBF_HPP
