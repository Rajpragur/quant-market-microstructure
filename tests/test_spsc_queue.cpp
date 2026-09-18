#include <cassert>
#include <iostream>

#include "mdp/spsc_queue.hpp"

int main() {
    mdp::SpscQueue<int, 4> queue;

    int value = 0;
    assert(queue.empty());
    assert(!queue.try_pop(value));

    assert(queue.try_push(10));
    assert(queue.try_push(20));
    assert(queue.try_push(30));
    assert(queue.try_push(40));
    assert(!queue.try_push(50));

    assert(queue.try_pop(value));
    assert(value == 10);
    assert(queue.try_pop(value));
    assert(value == 20);
    assert(queue.try_pop(value));
    assert(value == 30);
    assert(queue.try_pop(value));
    assert(value == 40);
    assert(!queue.try_pop(value));
    assert(queue.empty());

    assert(queue.try_push(60));
    assert(queue.try_pop(value));
    assert(value == 60);

    std::cout << "SPSC queue tests passed\n";
}
