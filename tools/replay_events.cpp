#include <iostream>
#include <string>

#include "mdp/replay.hpp"

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "usage: replay_events FILE\n";
        return 2;
    }
    try {
        const auto events = mdp::read_events(argv[1]);
        std::cout << "events=" << events.size() << '\n';
        if (!events.empty()) {
            std::cout << "first_sequence=" << events.front().sequence << '\n'
                      << "last_sequence=" << events.back().sequence << '\n';
        }
        return 0;
    } catch (const mdp::ReplayError& error) {
        std::cerr << "replay error: " << error.what() << '\n';
        return 1;
    }
}
