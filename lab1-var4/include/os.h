#pragma once

#include <string>
#include <cstddef>

using pipe_t = int;
using pid_t = int;
using signal_t = int;
using SignalHandler_t = void(*)(int);

extern const int ChildDeathSig;
extern const int BrokenPipeSig;

int CreatePipe(pipe_t fd[2]);
pid_t CloneProcess();
int Exec(const char* path);
void PrintLastError();
int WaitForChild();
int ClosePipe(pipe_t pipe);
int WritePipe(pipe_t pipe, void* buffer, std::size_t bytes);
int ReadPipe(pipe_t pipe, void* buffer, std::size_t bytes);
int LinkStdinWithPipe(pipe_t pipe);
int LinkStderrWithPipe(pipe_t pipe);
void AddSignalHandler(signal_t sig, SignalHandler_t handler);
int ReadFromStdin(char* buffer, std::size_t size);