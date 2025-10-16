#include "monte_carlo_card.h"
#include "deck.h"
#include <random>
#include <algorithm>
#include <iostream>
#include <mutex>

GeneralThread gen_thread;

void card_function(long long rounds) {
    std::vector<Card> deck = CreateDeck();
    
    std::random_device rd;
    std::mt19937 random_gen(rd());
    
    long long success_shuffles = 0;

    for (long long i = 0; i < rounds; ++i) {
        std::shuffle(deck.begin(), deck.end(), random_gen);

        if (deck[0].denomination == deck[1].denomination) {
            ++success_shuffles;
        }
    }

    std::lock_guard<std::mutex> lock(gen_thread.mutex);
    gen_thread.total_succeses_rounds += success_shuffles;
    gen_thread.total_rounds += rounds;
}