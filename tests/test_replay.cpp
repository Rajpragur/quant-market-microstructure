#include <cassert>
#include <cstdint>
#include <filesystem>
#include <vector>

#include "mdp/market_event.hpp"
#include "mdp/replay.hpp"

int main() {
    const std::filesystem::path path = "/tmp/mdp_event_roundtrip.bin";
    const std::vector<mdp::MarketEvent> input{
        mdp::MarketEvent::add(1, 1000, 42, mdp::Side::Buy, 101, 7),
        mdp::MarketEvent::cancel(2, 1010, 42, mdp::Side::Buy, 101, 7),
        mdp::MarketEvent::add(3, 1020, 43, mdp::Side::Sell, 102, 4),
    };

    assert(mdp::write_events(path, input));
    const auto output = mdp::read_events(path);
    assert(output == input);
    assert(mdp::read_events(path).size() == 3);
    std::filesystem::remove(path);
}
