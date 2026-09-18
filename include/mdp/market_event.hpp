#pragma once

#include <cstdint>
#include <vector>

#include "mdp/order_book.hpp"

namespace mdp {

enum class EventType : std::uint8_t { Add, Cancel, Modify, Trade };

enum class Venue : std::uint8_t { Synthetic, Kalshi };

struct MarketEvent {
    std::uint64_t sequence;
    std::uint64_t timestamp_ns;
    std::uint64_t order_id;
    std::int64_t price_ticks;
    std::uint32_t quantity;
    Side side;
    EventType type;
    Venue venue;

    static MarketEvent add(std::uint64_t sequence, std::uint64_t timestamp_ns,
                           std::uint64_t order_id, Side side,
                           std::int64_t price_ticks, std::uint32_t quantity,
                           Venue venue = Venue::Synthetic) {
        return {sequence, timestamp_ns, order_id, price_ticks, quantity,
                side, EventType::Add, venue};
    }

    static MarketEvent cancel(std::uint64_t sequence, std::uint64_t timestamp_ns,
                              std::uint64_t order_id, Side side,
                              std::int64_t price_ticks, std::uint32_t quantity,
                              Venue venue = Venue::Synthetic) {
        return {sequence, timestamp_ns, order_id, price_ticks, quantity,
                side, EventType::Cancel, venue};
    }

    friend bool operator==(const MarketEvent&, const MarketEvent&) = default;
};

}  // namespace mdp
