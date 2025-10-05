#include "child.h"
#include <cstdio>
#include <cstdlib>

void RunChildProcess(const char* output_file) {
    float num = 0.0;
    int scanned = std::scanf("%f", &num);
    if (scanned != 1) {
        std::fprintf(stderr, "Error: failed to read first number\n");
        std::exit(1);
    }

    float divider = 1.0;
    while ((scanned = std::scanf("%f", &divider)) == 1) {
        if (divider == 0.0) {
            std::fprintf(stderr, "Error: division by zero occurred\n");
            std::exit(1);
        }
        num /= divider;
    }

    if (scanned == 0 && !std::feof(stdin)) {
        std::fprintf(stderr, "Error: invalid input format\n");
        std::exit(1);
    }

    if (std::ferror(stdin)) {
        std::fprintf(stderr, "Error: error on stdin\n");
        std::exit(1);
    }

    FILE* out = std::fopen(output_file, "w");
    if (!out) {
        std::perror("fopen");
        std::exit(1);
    }

    std::fprintf(out, "%.6f\n", num);
    std::fclose(out);
}