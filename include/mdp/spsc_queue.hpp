#pragma once

#include <array>
#include <atomic>
#include <cstddef>
#include <utility>

namespace mdp {

template <typename T, std::size_t Capacity>
class SpscQueue {
    static_assert(Capacity > 0, "Capacity must be greater than zero");

public:
    [[nodiscard]] bool try_push(const T& value) {
        const std::size_t tail = tail_.load(std::memory_order_relaxed);
        const std::size_t head = head_.load(std::memory_order_acquire);

        if (tail - head == Capacity) {
            return false;
        }

        buffer_[tail % Capacity] = value;
        tail_.store(tail + 1, std::memory_order_release);
        return true;
    }

    [[nodiscard]] bool try_push(T&& value) {
        const std::size_t tail = tail_.load(std::memory_order_relaxed);
        const std::size_t head = head_.load(std::memory_order_acquire);

        if (tail - head == Capacity) {
            return false;
        }

        buffer_[tail % Capacity] = std::move(value);
        tail_.store(tail + 1, std::memory_order_release);
        return true;
    }

    [[nodiscard]] bool try_pop(T& value) {
        const std::size_t head = head_.load(std::memory_order_relaxed);
        const std::size_t tail = tail_.load(std::memory_order_acquire);

        if (head == tail) {
            return false;
        }

        value = std::move(buffer_[head % Capacity]);
        head_.store(head + 1, std::memory_order_release);
        return true;
    }

    [[nodiscard]] bool empty() const {
        return head_.load(std::memory_order_acquire) ==
               tail_.load(std::memory_order_acquire);
    }

    [[nodiscard]] constexpr std::size_t capacity() const noexcept {
        return Capacity;
    }

private:
    std::array<T, Capacity> buffer_{};
    std::atomic<std::size_t> head_{0};
    std::atomic<std::size_t> tail_{0};
};

}  // namespace mdp
