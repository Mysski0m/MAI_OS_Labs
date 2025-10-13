#include "monte_carlo_card.h"
#include "deck.h"
#include <random>
#include <algorithm>
#include <iostream>

GeneralThread gen_thread;

void* card_function(void* arg) {
    long long rounds = *static_cast<long long*>(arg);
    delete static_cast<long long*>(arg);

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

    pthread_mutex_lock(&gen_thread.mutex);
    gen_thread.total_succeses_rounds += success_shuffles;
    gen_thread.total_rounds += rounds;
    pthread_mutex_unlock(&gen_thread.mutex);

    return nullptr;
}