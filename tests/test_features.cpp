#include <cassert>
#include <cmath>
#include <optional>

#include "mdp/features.hpp"

int main() {
    assert(!mdp::compute_features({}));
    const mdp::BookTop top{100, 5, 102, 15};
    const auto features = mdp::compute_features(top);
    assert(features);
    assert(features->spread_ticks == 2);
    assert(std::abs(features->mid_price - 101.0) < 1e-12);
    assert(std::abs(features->microprice - 100.5) < 1e-12);
    assert(std::abs(features->imbalance - (-0.5)) < 1e-12);
}
