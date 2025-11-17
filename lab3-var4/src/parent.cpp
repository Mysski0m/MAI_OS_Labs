#include "parent.h"
#include "os.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

const std::size_t KBUFFER = 100;
const std::size_t KSHARED_MEM_SIZE = 1024;
static pid_t child_pid = -1;
static void* shared_mem = nullptr;
static const char* KSHARED_MEM_NAME = "/shm";
static int child_finished = 0;
static int child_exited_with_error = 0;
static int child_ready = 0;

void OnChildExit(signal_t signum) {
    if (signum == SIGCHLD) {
        int status;
        pid_t pid = waitpid(child_pid, &status, WNOHANG);
        if (pid == child_pid) {
            if (WIFEXITED(status)) {
                int exit_code = WEXITSTATUS(status);
                if (exit_code != 0) {
                    child_exited_with_error = 1;
                    std::fprintf(stderr, "Child process exited with error code: %d\n", exit_code);
                    std::exit(-1);
                }
            } else if (WIFSIGNALED(status)) {
                std::fprintf(stderr, "Child process was terminated by signal: %d\n", WTERMSIG(status));
                std::exit(-1);
            }
        }
    }
}

void OnChildKilled(signal_t signum) {
    if (shared_mem) {
        CloseSharedMemory(shared_mem, KSHARED_MEM_SIZE);
    }
    UnlinkSharedMemory(KSHARED_MEM_NAME);
    std::fprintf(stderr, "Child process was killed by signal %d\n", signum);
    std::exit(-1);
}

void OnChildDone(signal_t signum) {
    if (signum == SIGUSR1 && !child_ready) {
        child_ready = 1;
    } else if (signum == SIGUSR1 && child_ready) {
        child_finished = 1;
        std::printf("Child process completed succesfully.\n");
        std::printf("Result written to file.\n");
    }
}

void RunParentProcess() {
    AddSignalHandler(SIGCHLD, OnChildExit);
    AddSignalHandler(SIGUSR1, OnChildDone);

    std::printf("Enter result file name: ");
    char output_file[KBUFFER];
    if (!std::fgets(output_file, KBUFFER, stdin)) {
        std::fprintf(stderr, "Error reading file name from stdin\n");
        std::exit(1);
    }
    output_file[strcspn(output_file, "\n")] = '\0';

    std::printf("Enter numbers (separator - space): ");
    char numbers_line[KBUFFER];
    if (!std::fgets(numbers_line, KBUFFER, stdin)) {
        std::fprintf(stderr, "Error reading numbers from stdin\n");
        std::exit(1);
    }

    shared_mem = CreateSharedMemory(KSHARED_MEM_NAME, KSHARED_MEM_SIZE);
    if (!shared_mem) {
        std::fprintf(stderr, "Failed to create shared memory\n");
        std::exit(-1);
    }

    pid_t pid = CloneProcess();

    if (pid == 0) { // child
        execl("./child", "./child", output_file, nullptr);

        PrintLastError();
        std::exit(1);
    } else if (pid == -1) {
        PrintLastError();
        std::exit(1);
    } else { // parent
        child_pid = pid;

        while (!child_ready) {
            pause();
        }

        std::strncpy(static_cast<char*>(shared_mem), numbers_line, KSHARED_MEM_SIZE - 1);
        static_cast<char*>(shared_mem)[KSHARED_MEM_SIZE - 1] = '\0';

        SendSignal(pid, SIGUSR2);
        
        while (!child_finished && !child_exited_with_error) {
            pause();
        }

        CloseSharedMemory(shared_mem, KSHARED_MEM_SIZE);
        UnlinkSharedMemory(KSHARED_MEM_NAME);
    }
}