#pragma once

#include <array>
#include <cstddef>
#include <type_traits>
#include <utility>

namespace SwarmOS::Network {

template <typename TopicType, std::size_t MaxSubscribers = 4, std::size_t QueueSize = 8>
class PubSubBroker {
public:
    struct Invoker {
        void* instance{nullptr};
        void (*invoke)(void*, const TopicType&){nullptr};
    };

    constexpr PubSubBroker() = default;

    template <typename F>
    constexpr bool subscribe(F&& callable) noexcept {
        if (m_sub_count >= MaxSubscribers) return false;

        using DecayedF = typename std::decay<F>::type;

        if constexpr (std::is_is_function_v<typename std::remove_pointer<DecayedF>::type>) {
            // Si c'est un pointeur de fonction brut
            m_subscribers[m_sub_count++] = Invoker{
                .instance = reinterpret_cast<void*>(+callable),
                .invoke = [](void* ptr, const TopicType& val) {
                    auto fn = reinterpret_cast<void(*)(const TopicType&)>(ptr);
                    fn(val);
                }
            };
        } else {
            // Si c'est un objet callable / lambda (avec ou sans capture)
            m_subscribers[m_sub_count++] = Invoker{
                .instance = const_cast<void*>(static_cast<const void*>(&callable)),
                .invoke = [](void* ptr, const TopicType& val) {
                    (*static_cast<DecayedF*>(ptr))(val);
                }
            };
        }

        return true;
    }

    constexpr bool publish(const TopicType& message) noexcept {
        if (m_queue_count >= QueueSize) return false;

        for (std::size_t i = 0; i < m_sub_count; ++i) {
            if (m_subscribers[i].invoke) {
                m_subscribers[i].invoke(m_subscribers[i].instance, message);
            }
        }
        return true;
    }

private:
    std::array<Invoker, MaxSubscribers> m_subscribers{};
    std::size_t m_sub_count{0};
    std::size_t m_queue_count{0};
};

} // namespace SwarmOS::Network
