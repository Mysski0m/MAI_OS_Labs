#include "ServerApp.h"

#include <ctime>
#include <cstdlib>

int main() {
    std::srand(std::time(nullptr));
    ServerApp server;
    server.run();
    return 0;
}