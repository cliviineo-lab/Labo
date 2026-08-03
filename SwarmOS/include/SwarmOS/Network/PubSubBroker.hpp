#pragma once

#include <array>
#include <cstdint>
#include <cstddef>

namespace SwarmOS::Network {

// Structure générique pour un message du bus
template <typename T, std::size_t Capacity = 16>
class RingBuffer {
public:
    constexpr RingBuffer() noexcept = default;

    constexpr bool push(const T& item) noexcept {
        std::size_t next = (head_ + 1) % Capacity;
        if (next == tail_) {
            return false; // Buffer plein (garantie zéro-overflow)
        }
        buffer_[head_] = item;
        head_ = next;
        return true;
    }

    constexpr bool pop(T& item) noexcept {
        if (head_ == tail_) {
            return false; // Buffer vide
        }
        item = buffer_[tail_];
        tail_ = (tail_ + 1) % Capacity;
        return true;
    }

    [[nodiscard]] constexpr bool empty() const noexcept { return head_ == tail_; }

private:
    std::array<T, Capacity> buffer_{};
    size_t head_{0};
    size_t tail_{0};
};

// Broker de publication / abonnement statique
template <typename TopicType, std::size_t MaxSubscribers = 4, std::size_t QueueSize = 8>
class PubSubBroker {
public:
    using Callback = void(*)(const TopicType&);

    constexpr bool subscribe(Callback cb) noexcept {
        if (subscriber_count_ >= MaxSubscribers) {
            return false; // Limite atteinte
        }
        subscribers_[subscriber_count_++] = cb;
        return true;
    }

    void publish(const TopicType& msg) noexcept {
        // Envoie synchrone et immédiat aux abonnés (Temps Réel)
        for (std::size_t i = 0; i < subscriber_count_; ++i) {
            if (subscribers_[i] != nullptr) {
                subscribers_[i](msg);
            }
        }
    }

private:
    std::array<Callback, MaxSubscribers> subscribers_{};
    std::size_t subscriber_count_{0};
};

} // namespace SwarmOS::Network
