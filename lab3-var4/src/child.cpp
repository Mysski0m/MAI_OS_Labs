#include "child.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>

static void* shared_mem = nullptr;
static pid_t parent_pid = -1;
const size_t SHARED_MEM_BUFFER = 1024;
static const char* g_output_file = nullptr;

static void OnParentSignal(int sig) {
    if (sig == SIGUSR2) {
        char* input = static_cast<char*>(shared_mem);
        float num = 0.0;
        int scanned = std::sscanf(input, "%f", &num);
        if (scanned != 1) {
            std::fprintf(stderr, "Error: failed to read first number\n");
            std::exit(1);
        }

        const char* p = input;
        while (*p && *p != ' ') p++;
        if (*p) p++;
        float divider = 0.0;
        while (std::sscanf(p, "%f", &divider) == 1) {
            if (divider == 0.0) {
                std::fprintf(stderr, "Error: division by zero occurred\n");
                std::exit(1);
            }
            num /= divider;
            while (*p && *p != ' ') p++;
            if (*p) p++;
        }

        FILE* out = std::fopen(g_output_file, "w");
        if (!out) {
            std::fprintf(stderr, "Error: failed to open output file '%s'\n", g_output_file);
            std::exit(1);
        }

        std::fprintf(out, "%.6f\n", num);
        std::fclose(out);

        kill(parent_pid, SIGUSR1);
        
        std::exit(0);
    }
}

void RunChildProcess(const char* output_file, const char* shared_mem_name) {
    g_output_file = output_file;
    parent_pid = getppid();

    int fd = shm_open(shared_mem_name, O_RDWR, 0666);
    if (fd == -1) {
        std::perror("child can't open shared memory");
        std::exit(-1);
    }

    shared_mem = mmap(nullptr, SHARED_MEM_BUFFER, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);

    if (shared_mem == MAP_FAILED) {
        std::perror("child: mmap");
        std::exit(1);
    }

    signal(SIGUSR2, OnParentSignal);

    kill(parent_pid, SIGUSR1);

    pause();
}