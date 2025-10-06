#include "parent.h"
#include "os.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <unistd.h>

const std::size_t kBuffer = 100;
static pipe_t err_pipe_in;

void PrintErrorFromChild(pipe_t pipe) {
    char buffer[kBuffer];
    int bytes = ReadPipe(pipe, buffer, kBuffer);
    if (bytes > 0) {
        std::fprintf(stderr, "Error from child: ");
    }
    while (bytes > 0) {
        std::fwrite(buffer, sizeof(char), bytes, stderr);
        bytes = ReadPipe(pipe, buffer, kBuffer);
    }
}

void OnChildKilled(signal_t signum) {
    PrintErrorFromChild(err_pipe_in);
    std::exit(-1);
}

void RunParentProcess() {
    AddSignalHandler(ChildDeathSig, OnChildKilled);

    std::printf("Enter result file name: ");
    char output_file[kBuffer];
    if (!std::fgets(output_file, kBuffer, stdin)) {
        PrintLastError();
        std::exit(1);
    }
    output_file[strcspn(output_file, "\n")] = '\0';

    std::printf("Enter numbers (separator - space): ");
    char numbers_line[kBuffer];
    if (!std::fgets(numbers_line, kBuffer, stdin)) {
        PrintLastError();
        std::exit(1);
    }

    pipe_t input_pipe[2];
    pipe_t err_pipe[2];
    if (CreatePipe(input_pipe) == -1 || CreatePipe(err_pipe) == -1) {
        PrintLastError();
        std::exit(1);
    }

    err_pipe_in = err_pipe[0];
    pid_t child_id = CloneProcess();

    if (child_id == 0) { // child
        if (LinkStdinWithPipe(input_pipe[0]) == -1 ||
            LinkStderrWithPipe(err_pipe[1]) == -1) {
            PrintLastError();
            std::exit(1);
        }
        ClosePipe(input_pipe[1]);
        ClosePipe(err_pipe[0]);

        const char* child_argv[] = {"./child", output_file, nullptr};
        execl("./child", "./child", output_file, nullptr);

        PrintLastError();
        std::exit(1);
    } else if (child_id == -1) {
        PrintLastError();
        std::exit(1);
    } else { // parent
        ClosePipe(input_pipe[0]);
        ClosePipe(err_pipe[1]);

        WritePipe(input_pipe[1], numbers_line, std::strlen(numbers_line));
        ClosePipe(input_pipe[1]);

        PrintErrorFromChild(err_pipe[0]);
        ClosePipe(err_pipe[0]);

        WaitForChild();
    }
}