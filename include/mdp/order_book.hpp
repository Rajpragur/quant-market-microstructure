#pragma once

#include <cstdint>
#include <deque>
#include <map>
#include <optional>
#include <unordered_map>
#include <vector>

namespace mdp {

enum class Side { Buy, Sell };

struct Order {
    std::uint64_t order_id;
    Side side;
    std::int64_t price_ticks;
    std::uint64_t quantity;
};

struct Trade {
    std::uint64_t aggressor_order_id;
    std::uint64_t resting_order_id;
    std::int64_t price_ticks;
    std::uint64_t quantity;

    friend bool operator==(const Trade&, const Trade&) = default;
};

struct OrderLocation {
    std::int64_t price_ticks;
    Side side;
};

class OrderBook {
public:
    [[nodiscard]] std::vector<Trade> submit_limit(Order incoming) {
        std::vector<Trade> trades;
        if (incoming.quantity == 0 || locations_.contains(incoming.order_id)) {
            return trades;
        }
        if (incoming.side == Side::Buy) {
            match_buy(incoming, trades);
        } else {
            match_sell(incoming, trades);
        }
        if (incoming.quantity != 0) {
            resting(incoming).push_back(incoming);
            locations_.emplace(incoming.order_id,
                               OrderLocation{incoming.price_ticks, incoming.side});
        }
        return trades;
    }

    [[nodiscard]] bool cancel(std::uint64_t order_id) {
        const auto location = locations_.find(order_id);
        if (location == locations_.end()) {
            return false;
        }
        Levels& levels = location->second.side == Side::Buy ? bids_ : asks_;
        auto level = levels.find(location->second.price_ticks);
        if (level == levels.end()) {
            return false;
        }
        for (auto order = level->second.begin(); order != level->second.end(); ++order) {
            if (order->order_id == order_id) {
                level->second.erase(order);
                if (level->second.empty()) {
                    levels.erase(level);
                }
                locations_.erase(location);
                return true;
            }
        }
        return false;
    }

    [[nodiscard]] std::optional<std::int64_t> best_bid() const {
        if (bids_.empty()) {
            return std::nullopt;
        }
        return bids_.rbegin()->first;
    }

    [[nodiscard]] std::optional<std::int64_t> best_ask() const {
        if (asks_.empty()) {
            return std::nullopt;
        }
        return asks_.begin()->first;
    }

    [[nodiscard]] std::uint64_t bid_quantity(std::int64_t price_ticks) const {
        return level_quantity(bids_, price_ticks);
    }

    [[nodiscard]] std::uint64_t ask_quantity(std::int64_t price_ticks) const {
        return level_quantity(asks_, price_ticks);
    }

private:
    using Levels = std::map<std::int64_t, std::deque<Order>>;

    Levels bids_;
    Levels asks_;
    std::unordered_map<std::uint64_t, OrderLocation> locations_;

    [[nodiscard]] std::deque<Order>& resting(const Order& order) {
        return order.side == Side::Buy ? bids_[order.price_ticks]
                                       : asks_[order.price_ticks];
    }

    void match_buy(Order& incoming, std::vector<Trade>& trades) {
        while (incoming.quantity != 0 && !asks_.empty()) {
            auto level = asks_.begin();
            if (level->first > incoming.price_ticks) {
                break;
            }
            auto& orders = level->second;
            while (incoming.quantity != 0 && !orders.empty()) {
                Order& resting_order = orders.front();
                const std::uint64_t executed = incoming.quantity < resting_order.quantity
                    ? incoming.quantity
                    : resting_order.quantity;
                trades.push_back({incoming.order_id, resting_order.order_id,
                                  resting_order.price_ticks, executed});
                incoming.quantity -= executed;
                resting_order.quantity -= executed;
                if (resting_order.quantity == 0) {
                    locations_.erase(resting_order.order_id);
                    orders.pop_front();
                }
            }
            if (orders.empty()) {
                asks_.erase(level);
            }
        }
    }

    void match_sell(Order& incoming, std::vector<Trade>& trades) {
        while (incoming.quantity != 0 && !bids_.empty()) {
            auto level = std::prev(bids_.end());
            if (level->first < incoming.price_ticks) {
                break;
            }
            auto& orders = level->second;
            while (incoming.quantity != 0 && !orders.empty()) {
                Order& resting_order = orders.front();
                const std::uint64_t executed = incoming.quantity < resting_order.quantity
                    ? incoming.quantity
                    : resting_order.quantity;
                trades.push_back({incoming.order_id, resting_order.order_id,
                                  resting_order.price_ticks, executed});
                incoming.quantity -= executed;
                resting_order.quantity -= executed;
                if (resting_order.quantity == 0) {
                    locations_.erase(resting_order.order_id);
                    orders.pop_front();
                }
            }
            if (orders.empty()) {
                bids_.erase(level->first);
            }
        }
    }

    static bool cancel_from(Levels& levels, std::uint64_t order_id) {
        for (auto level = levels.begin(); level != levels.end(); ++level) {
            auto& orders = level->second;
            for (auto order = orders.begin(); order != orders.end(); ++order) {
                if (order->order_id == order_id) {
                    orders.erase(order);
                    if (orders.empty()) {
                        levels.erase(level);
                    }
                    return true;
                }
            }
        }
        return false;
    }

    static std::uint64_t level_quantity(const Levels& levels,
                                        std::int64_t price_ticks) {
        const auto level = levels.find(price_ticks);
        if (level == levels.end()) {
            return 0;
        }
        std::uint64_t total = 0;
        for (const Order& order : level->second) {
            total += order.quantity;
        }
        return total;
    }
};

}  // namespace mdp
