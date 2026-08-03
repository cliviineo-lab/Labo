#pragma once

#include <array>
#include <cstddef>

namespace SwarmOS::Network {

template <typename TopicType, std::size_t MaxSubscribers = 4, std::size_t QueueSize = 8>
class PubSubBroker {
public:
    struct Invoker {
        void* instance{nullptr};
        void (*invoke)(void*, const TopicType&){nullptr};
    };

    constexpr PubSubBroker() = default;

    // Acepte n'importe quel callable (lambda avec/sans capture, foncteur, etc.)
    template <typename F>
    constexpr bool subscribe(F&& callable) noexcept {
        if (m_sub_count >= MaxSubscribers) return false;
        
        using DecayedF = typename std::decay<F>::type;
        
        // Stocke la référence si c'est une lvalue, ou une copie statique
        m_subscribers[m_sub_count++] = Invoker{
            .instance = const_cast<void*>(static_cast<const void*>(&callable)),
            .invoke = [](void* ptr, const TopicType& val) {
                (*static_cast<DecayedF*>(ptr))(val);
            }
        };
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
