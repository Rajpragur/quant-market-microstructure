#include <cassert>
#include <cstdint>
#include <optional>

#include "mdp/order_book.hpp"

int main() {
    mdp::OrderBook book;

    const auto ignored_1 = book.submit_limit({1, mdp::Side::Sell, 101, 5});
    const auto ignored_2 = book.submit_limit({2, mdp::Side::Sell, 101, 3});
    const auto ignored_3 = book.submit_limit({3, mdp::Side::Sell, 102, 8});
    assert(ignored_1.empty() && ignored_2.empty() && ignored_3.empty());

    const auto duplicate = book.submit_limit({1, mdp::Side::Buy, 99, 2});
    assert(duplicate.empty());

    assert(book.best_ask() == std::optional<std::int64_t>{101});
    assert(book.ask_quantity(101) == 8);

    const auto trades = book.submit_limit({4, mdp::Side::Buy, 102, 6});
    assert(trades.size() == 2);
    const mdp::Trade expected_first{4, 1, 101, 5};
    const mdp::Trade expected_second{4, 2, 101, 1};
    assert((trades[0] == expected_first));
    assert((trades[1] == expected_second));
    assert(book.best_ask() == std::optional<std::int64_t>{101});
    assert(book.ask_quantity(101) == 2);

    assert(book.cancel(2));
    assert(!book.cancel(2));
    assert(!book.cancel(999));
    assert(book.best_ask() == std::optional<std::int64_t>{102});
    assert(book.ask_quantity(102) == 8);

    const auto ignored_4 = book.submit_limit({5, mdp::Side::Buy, 99, 4});
    assert(ignored_4.empty());
    assert(book.best_bid() == std::optional<std::int64_t>{99});
    assert(book.bid_quantity(99) == 4);

    mdp::OrderBook modified;
    const auto m1 = modified.submit_limit({10, mdp::Side::Buy, 100, 5});
    const auto m2 = modified.submit_limit({11, mdp::Side::Buy, 100, 5});
    assert(m1.empty() && m2.empty());
    assert(modified.modify(10, 100, 3));
    assert(modified.oldest_order_id(mdp::Side::Buy, 100) == 10);
    assert(modified.modify(10, 100, 8));
    assert(modified.oldest_order_id(mdp::Side::Buy, 100) == 11);
    assert(modified.modify(10, 101, 8));
    assert(modified.best_bid() == 101);
    assert(!modified.modify(999, 100, 1));
}
