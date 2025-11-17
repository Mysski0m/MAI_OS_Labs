#include "os.h"
#include "exceptions.h"
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>
#include <csignal>
#include <cstdio>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>

const int ChildDeathSig = SIGCHLD;
const int ChildDoneSig = SIGUSR1;
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
    // std::fprintf(stderr, "DEBUG: errno = %d\n", errno);
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

void* CreateSharedMemory(const char* name, size_t size) {
    int fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        PrintLastError();
        return nullptr;
    }

    if (ftruncate(fd, size) == -1) {
        PrintLastError();
        close(fd);
        return nullptr;
    }

    void* addr = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);

    if (addr == MAP_FAILED) {
        PrintLastError();
        return nullptr;
    }

    return addr;
}

int CloseSharedMemory(void* addr, size_t size) {
    return munmap(addr, size);
}

int UnlinkSharedMemory(const char* name) {
    return shm_unlink(name);
}

void SendSignal(pid_t pid, signal_t sig) {
    kill(pid, sig);
}