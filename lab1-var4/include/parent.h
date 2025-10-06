#pragma once
#include "os.h"

void RunParentProcess();
void PrintErrorFromChild(pipe_t pipe);
void OnChildKilled(signal_t signum);