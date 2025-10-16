#pragma once

#include <mutex>

struct GeneralThread {
    long long total_succeses_rounds = 0;
    long long total_rounds = 0;
    std::mutex mutex;
};

extern GeneralThread gen_thread;

void card_function(long long rounds);