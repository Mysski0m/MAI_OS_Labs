#pragma once

#include <pthread.h>

struct GeneralThread {
    long long total_succeses_rounds = 0;
    long long total_rounds = 0;
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
};

extern GeneralThread gen_thread;

void* card_function(void* arg);