#pragma once

#include <cstdint>
#include <vector>

#include "mdp/order_book.hpp"

namespace mdp {

struct RestingOrder {
    std::uint64_t order_id;
    std::uint64_t accept_time_ns;
    std::uint64_t available_time_ns;
    Side side;
    std::int64_t price_ticks;
    std::uint64_t remaining_quantity;
    std::uint64_t queue_ahead;
};

struct SimulatedFill {
    std::uint64_t order_id;
    Side side;
    std::int64_t price_ticks;
    std::uint64_t quantity;
    std::int64_t fee_ticks;
};

class ExecutionSimulator {
public:
    explicit ExecutionSimulator(std::uint64_t latency_ns,
                                std::int64_t fee_ticks_per_contract)
        : latency_ns_(latency_ns), fee_ticks_per_contract_(fee_ticks_per_contract) {}

    [[nodiscard]] RestingOrder place_limit(std::uint64_t accept_time_ns, Side side,
                                           std::int64_t price_ticks,
                                           std::uint64_t quantity,
                                           std::uint64_t queue_ahead) {
        RestingOrder order{next_order_id_++, accept_time_ns,
                           accept_time_ns + latency_ns_, side, price_ticks,
                           quantity, queue_ahead};
        orders_.push_back(order);
        return order;
    }

    void advance_time(std::uint64_t timestamp_ns) { now_ns_ = timestamp_ns; }

    [[nodiscard]] std::vector<SimulatedFill> on_trade(std::int64_t price_ticks,
                                                       Side aggressor_side,
                                                       std::uint64_t quantity) {
        std::vector<SimulatedFill> fills;
        for (RestingOrder& order : orders_) {
            if (quantity == 0 || order.remaining_quantity == 0 ||
                order.available_time_ns > now_ns_ || order.price_ticks != price_ticks ||
                order.side == aggressor_side) {
                continue;
            }
            if (order.queue_ahead != 0) {
                const auto passed = order.queue_ahead < quantity ? order.queue_ahead : quantity;
                order.queue_ahead -= passed;
                quantity -= passed;
            }
            if (quantity == 0 || order.queue_ahead != 0) {
                continue;
            }
            const auto filled = order.remaining_quantity < quantity
                ? order.remaining_quantity : quantity;
            order.remaining_quantity -= filled;
            quantity -= filled;
            const auto fee = static_cast<std::int64_t>(filled) * fee_ticks_per_contract_;
            fills.push_back({order.order_id, order.side, order.price_ticks, filled, fee});
            inventory_ += order.side == Side::Buy ? static_cast<std::int64_t>(filled)
                                                  : -static_cast<std::int64_t>(filled);
            const auto cash = static_cast<std::int64_t>(filled) * order.price_ticks;
            cash_ticks_ += order.side == Side::Buy ? -cash - fee : cash - fee;
        }
        return fills;
    }

    [[nodiscard]] bool cancel(std::uint64_t order_id) {
        for (auto& order : orders_) {
            if (order.order_id == order_id && order.remaining_quantity != 0) {
                order.remaining_quantity = 0;
                return true;
            }
        }
        return false;
    }

    [[nodiscard]] std::int64_t inventory() const { return inventory_; }
    [[nodiscard]] std::int64_t cash_ticks() const { return cash_ticks_; }

private:
    std::uint64_t latency_ns_;
    std::int64_t fee_ticks_per_contract_;
    std::uint64_t now_ns_{0};
    std::uint64_t next_order_id_{1};
    std::vector<RestingOrder> orders_;
    std::int64_t inventory_{0};
    std::int64_t cash_ticks_{0};
};

}  // namespace mdp
