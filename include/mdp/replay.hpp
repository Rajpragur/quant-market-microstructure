#pragma once

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "mdp/market_event.hpp"

namespace mdp {

inline constexpr char replay_magic[] = "MDPEVT01";
inline constexpr std::uint32_t replay_version = 1;

class ReplayError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

namespace detail {

template <typename T>
void write_value(std::ofstream& file, T value) {
    file.write(reinterpret_cast<const char*>(&value), sizeof(value));
    if (!file) {
        throw ReplayError("failed while writing replay file");
    }
}

template <typename T>
T read_value(std::ifstream& file) {
    T value{};
    file.read(reinterpret_cast<char*>(&value), sizeof(value));
    if (!file) {
        throw ReplayError("truncated replay file");
    }
    return value;
}

inline void write_event(std::ofstream& file, const MarketEvent& event) {
    write_value(file, event.sequence);
    write_value(file, event.timestamp_ns);
    write_value(file, event.order_id);
    write_value(file, event.price_ticks);
    write_value(file, event.quantity);
    write_value(file, static_cast<std::uint8_t>(event.side));
    write_value(file, static_cast<std::uint8_t>(event.type));
    write_value(file, static_cast<std::uint8_t>(event.venue));
}

inline MarketEvent read_event(std::ifstream& file) {
    const auto sequence = read_value<std::uint64_t>(file);
    const auto timestamp_ns = read_value<std::uint64_t>(file);
    const auto order_id = read_value<std::uint64_t>(file);
    const auto price_ticks = read_value<std::int64_t>(file);
    const auto quantity = read_value<std::uint32_t>(file);
    const auto side = static_cast<Side>(read_value<std::uint8_t>(file));
    const auto type = static_cast<EventType>(read_value<std::uint8_t>(file));
    const auto venue = static_cast<Venue>(read_value<std::uint8_t>(file));
    return {sequence, timestamp_ns, order_id, price_ticks, quantity, side, type, venue};
}

}  // namespace detail

inline bool write_events(const std::filesystem::path& path,
                         const std::vector<MarketEvent>& events) {
    std::ofstream file(path, std::ios::binary | std::ios::trunc);
    if (!file) {
        return false;
    }
    file.write(replay_magic, sizeof(replay_magic) - 1);
    detail::write_value(file, replay_version);
    detail::write_value(file, static_cast<std::uint64_t>(events.size()));
    for (const MarketEvent& event : events) {
        detail::write_event(file, event);
    }
    return static_cast<bool>(file);
}

inline std::vector<MarketEvent> read_events(const std::filesystem::path& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        throw ReplayError("could not open replay file");
    }
    char magic[sizeof(replay_magic) - 1]{};
    file.read(magic, sizeof(magic));
    if (!file || std::string(magic, sizeof(magic)) !=
                     std::string(replay_magic, sizeof(magic))) {
        throw ReplayError("invalid replay magic");
    }
    if (detail::read_value<std::uint32_t>(file) != replay_version) {
        throw ReplayError("unsupported replay version");
    }
    const auto count = detail::read_value<std::uint64_t>(file);
    if (count > 100'000'000) {
        throw ReplayError("replay event count is unreasonable");
    }
    std::vector<MarketEvent> events;
    events.reserve(static_cast<std::size_t>(count));
    for (std::uint64_t i = 0; i < count; ++i) {
        events.push_back(detail::read_event(file));
    }
    return events;
}

}  // namespace mdp
