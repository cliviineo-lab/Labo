#ifndef SWARMOS_ROS2_BRIDGE_HPP
#define SWARMOS_ROS2_BRIDGE_HPP

#include <array>
#include <mutex>
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
        std::lock_guard<std::mutex> lock(cmd_mutex_);
        input_cmd_ = cmd;
        has_new_cmd_ = true;
    }

    void push_ros_state(const ROSState& st) noexcept {
        std::lock_guard<std::mutex> lock(state_mutex_);
        current_state_ = st;
    }

    [[nodiscard]] ROSCommand step_realtime_loop(
        SwarmOS::Safety::SafetyCBF<MaxObstacles>& cbf,
        const std::array<ROSState, MaxObstacles>& obstacles,
        size_t obstacle_count) noexcept 
    {
        ROSCommand raw_cmd;
        {
            std::lock_guard<std::mutex> lock(cmd_mutex_);
            raw_cmd = input_cmd_;
        }

        ROSState current_st;
        {
            std::lock_guard<std::mutex> lock(state_mutex_);
            current_st = current_state_;
        }

        typename SwarmOS::Safety::SafetyCBF<MaxObstacles>::State state{};
        state.position = current_st.position;
        state.velocity = current_st.velocity;

        typename SwarmOS::Safety::SafetyCBF<MaxObstacles>::Command cmd{};
        cmd.desired_velocity = raw_cmd.velocity;

        auto safe_cmd = cbf.filter(
            state, 
            cmd, 
            reinterpret_cast<const std::array<typename SwarmOS::Safety::SafetyCBF<MaxObstacles>::State, MaxObstacles>&>(obstacles), 
            obstacle_count
        );

        ROSCommand filtered_ros_cmd;
        filtered_ros_cmd.velocity = safe_cmd.safe_velocity;
        filtered_ros_cmd.timestamp_ns = raw_cmd.timestamp_ns;

        return filtered_ros_cmd;
    }

private:
    mutable std::mutex cmd_mutex_;
    ROSCommand input_cmd_{};

    mutable std::mutex state_mutex_;
    ROSState current_state_{};

    bool has_new_cmd_{false};
};

} // namespace SwarmOS::Bridge

#endif // SWARMOS_ROS2_BRIDGE_HPP
