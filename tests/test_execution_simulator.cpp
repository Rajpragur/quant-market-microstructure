#include <cassert>
#include <cstdint>

#include "mdp/execution_simulator.hpp"

int main() {
    mdp::ExecutionSimulator simulator(2, 1);
    const auto order = simulator.place_limit(7, mdp::Side::Buy, 100, 10, 5);
    assert(order.accept_time_ns == 7);
    assert(order.available_time_ns == 9);
    assert(simulator.on_trade(100, mdp::Side::Sell, 3).empty());
    simulator.advance_time(9);
    assert(simulator.on_trade(100, mdp::Side::Sell, 3).empty());
    const auto fills = simulator.on_trade(100, mdp::Side::Sell, 8);
    assert(fills.size() == 1);
    assert(fills[0].quantity == 6);
    assert(fills[0].price_ticks == 100);
    assert(fills[0].fee_ticks == 6);
    assert(simulator.inventory() == 6);
    assert(simulator.cash_ticks() == -606);
    assert(simulator.cancel(order.order_id));
    assert(!simulator.cancel(order.order_id));
}
