#pragma once

#include <string>
#include <cstddef>
#include <signal.h>

using pipe_t = int;
using pid_t = int;
using signal_t = int;
using SignalHandler_t = void(*)(int);

extern const int ChildDeathSig;
extern const int BrokenPipeSig;
extern const int ChildDoneSig;

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
void* CreateSharedMemory(const char* name, size_t size);
int CloseSharedMemory(void* addr, size_t size);
int UnlinkSharedMemory(const char* name);
void SendSignal(pid_t pid, signal_t sig);