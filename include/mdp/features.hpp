#pragma once

#include <cstdint>
#include <optional>

namespace mdp {

struct BookTop {
    std::int64_t bid_price_ticks;
    std::uint64_t bid_quantity;
    std::int64_t ask_price_ticks;
    std::uint64_t ask_quantity;
};

struct MicrostructureFeatures {
    std::int64_t spread_ticks;
    double mid_price;
    double microprice;
    double imbalance;
};

[[nodiscard]] inline std::optional<MicrostructureFeatures>
compute_features(const std::optional<BookTop>& top) {
    if (!top || top->bid_quantity == 0 || top->ask_quantity == 0 ||
        top->ask_price_ticks < top->bid_price_ticks) {
        return std::nullopt;
    }
    const auto total_quantity = top->bid_quantity + top->ask_quantity;
    const double bid = static_cast<double>(top->bid_price_ticks);
    const double ask = static_cast<double>(top->ask_price_ticks);
    const double bid_quantity = static_cast<double>(top->bid_quantity);
    const double ask_quantity = static_cast<double>(top->ask_quantity);
    return MicrostructureFeatures{
        top->ask_price_ticks - top->bid_price_ticks,
        (bid + ask) / 2.0,
        (ask * bid_quantity + bid * ask_quantity) /
            static_cast<double>(total_quantity),
        (bid_quantity - ask_quantity) / static_cast<double>(total_quantity),
    };
}

}  // namespace mdp
