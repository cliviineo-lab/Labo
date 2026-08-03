#ifndef SWARMOS_ROS2_BRIDGE_HPP
#define SWARMOS_ROS2_BRIDGE_HPP

#include <array>
#include <atomic>
#include "SwarmOS/Safety/SafetyCBF.hpp"

namespace SwarmOS::Bridge {

struct ROSCommand {
    std::array<double, 3> velocity{0.0, 0.0, 0.0};
    uint64_t timestamp_ns{0};
};

struct ROSState {
    std::array<double, 3> position{0.0, 0.0, 0.0};
    std::array<double, 3> velocity{0.0, 0.0, 0.0};
    uint64_t timestamp_ns{0};
};

template <size_t MaxObstacles = 10>
class ROS2Bridge {
public:
    constexpr ROS2Bridge() noexcept = default;

    void push_ros_command(const ROSCommand& cmd) noexcept {
        input_cmd_.store(cmd, std::memory_order_release);
        has_new_cmd_.store(true, std::memory_order_release);
    }

    void push_ros_state(const ROSState& st) noexcept {
        current_state_.store(st, std::memory_order_release);
    }

    [[nodiscard]] ROSCommand step_realtime_loop(
        SwarmOS::Safety::SafetyCBF<MaxObstacles>& cbf,
        const std::array<ROSState, MaxObstacles>& obstacles,
        size_t obstacle_count) noexcept 
    {
        ROSCommand raw_cmd = input_cmd_.load(std::memory_order_acquire);
        ROSState current_st = current_state_.load(std::memory_order_acquire);

        typename SwarmOS::Safety::SafetyCBF<MaxObstacles>::State state{};
        state.position = current_st.position;
        state.velocity = current_st.velocity;

        typename SwarmOS::Safety::SafetyCBF<MaxObstacles>::Command cmd{};
        cmd.desired_velocity = raw_cmd.velocity;

        auto safe_cmd = cbf.filter(state, cmd, obstacles, obstacle_count);

        ROSCommand filtered_ros_cmd;
        filtered_ros_cmd.velocity = safe_cmd.safe_velocity;
        filtered_ros_cmd.timestamp_ns = raw_cmd.timestamp_ns;

        return filtered_ros_cmd;
    }

private:
    std::atomic<ROSCommand> input_cmd_{};
    std::atomic<ROSState> current_state_{};
    std::atomic<bool> has_new_cmd_{false};
};

} // namespace SwarmOS::Bridge

#endif // SWARMOS_ROS2_BRIDGE_HPP
