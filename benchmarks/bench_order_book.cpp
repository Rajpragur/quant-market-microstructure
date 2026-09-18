#include <chrono>
#include <cstdint>
#include <iostream>

#include "mdp/order_book.hpp"

int main() {
    constexpr std::uint64_t iterations = 1'000'000;
    mdp::OrderBook book;
    std::uint64_t trade_count = 0;

    for (std::uint64_t i = 0; i < 10'000; ++i) {
        const auto ignored = book.submit_limit({i, mdp::Side::Sell, 101, 1});
        trade_count += ignored.size();
    }

    const auto start = std::chrono::steady_clock::now();
    for (std::uint64_t i = 10'000; i < 10'000 + iterations; ++i) {
        const mdp::Side side = (i % 2 == 0) ? mdp::Side::Buy : mdp::Side::Sell;
        const std::int64_t price = (side == mdp::Side::Buy) ? 101 : 100;
        const auto trades = book.submit_limit({i, side, price, 1});
        trade_count += trades.size();
    }
    const auto elapsed = std::chrono::duration<double>(
        std::chrono::steady_clock::now() - start).count();

    std::cout << "order_book_benchmark\n"
              << "orders=" << iterations << '\n'
              << "trades=" << trade_count << '\n'
              << "seconds=" << elapsed << '\n'
              << "orders_per_second=" << static_cast<double>(iterations) / elapsed
              << '\n';
}
