#include "monte_carlo_card.h"
#include "deck.h"
#include <iostream>
#include <vector>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <thread>
#include <mutex>

constexpr std::size_t MAX_ROUNDS = 1000000;

int main(int argc, char* argv[]) {
    if (argc != 5) {
        std::cerr << "Wrong arguments, expected './main --rounds [N] --max-threads [M]'\n";
        return 1;
    }

    long long total_rounds;
    int max_threads;

    if (std::strcmp(argv[1], "--rounds") != 0) { std::cerr << "Wrong arguments, expected '--rounds'\n"; return 1; }
    if (argc < 2) { std::cerr << "Wrong number of arguments\n"; return 1; }
    total_rounds = std::atoll(argv[2]);

    if (std::strcmp(argv[3], "--max-threads") != 0) { std::cerr << "Wrong arguments, expected '--max-threads'\n"; return 1; }
    if (argc < 4) { std::cerr << "wrong number of arguments\n"; return 1; }
    max_threads = std::atoll(argv[4]);

    if (total_rounds <= 0 || max_threads <= 0) {
        std::cerr << "Rounds and max-threads number should be positive\n";
        return 1;
    }

    long long rounds_per_thread = total_rounds / max_threads;
    long long remainder = total_rounds % max_threads;

    auto start_time = std::chrono::steady_clock::now();

    std::vector<std::thread> threads;
    long long rounds_for_cur_thread; 
    for (int i = 0; i < max_threads; ++i) {
        rounds_for_cur_thread = rounds_per_thread + (i == 0 ? remainder : 0);
        threads.emplace_back(card_function, rounds_for_cur_thread);
    }

    for (auto& t : threads) {
        t.join();
    }

    auto end_time = std::chrono::steady_clock::now();
    double probability = static_cast<double>(gen_thread.total_succeses_rounds) / gen_thread.total_rounds;

    std::cout << "Total rounds: " << total_rounds << "\n";
    std::cout << "Successful rounds: " << gen_thread.total_succeses_rounds << "\n";
    std::cout << "Probability: " << probability << " or " << probability * 100 << "%\n";
    std::cout << "Duration: " << end_time - start_time << " or " 
              << std::chrono::duration_cast<std::chrono::duration<double>>(end_time - start_time) << "\n";
}