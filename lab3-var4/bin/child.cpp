#include "child.h"
#include <cstdio>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::fprintf(stderr, "Usage: %s <output_file>\n", argv[0]);
        return 1;
    }

    const char* shared_mem_name = "/shm";
    RunChildProcess(argv[1], shared_mem_name);
    return 0;
}