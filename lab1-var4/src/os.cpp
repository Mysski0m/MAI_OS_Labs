#include "os.h"
#include "exceptions.h"
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>
#include <csignal>
#include <cstdio>

const int ChildDeathSig = SIGCHLD;
const int BrokenPipeSig = SIGPIPE;

int CreatePipe(pipe_t fd[2]) {
    return pipe(fd);
}

pid_t CloneProcess() {
    return static_cast<pid_t>(fork());
}

int Exec(const char* path) {
    return execl(path, path, nullptr);
}

void PrintLastError() {
    perror("OS Error");
}

int WaitForChild() {
    return wait(nullptr);
}

int ClosePipe(pipe_t pipe) {
    return close(pipe);
}

int WritePipe(pipe_t pipe, void* buffer, std::size_t bytes) {
    return static_cast<int>(write(pipe, buffer, bytes));
}

int ReadPipe(pipe_t pipe, void* buffer, std::size_t bytes) {
    return static_cast<int>(read(pipe, buffer, bytes));
}

int LinkStdinWithPipe(pipe_t pipe) {
    return dup2(pipe, STDIN_FILENO);
}

int LinkStderrWithPipe(pipe_t pipe) {
    return dup2(pipe, STDERR_FILENO);
}

void AddSignalHandler(signal_t sig, SignalHandler_t handler) {
    signal(sig, handler);
}

int ReadFromStdin(char* buffer, std::size_t size) {
    return static_cast<int>(read(STDIN_FILENO, buffer, size));
}