#pragma once
#include "os.h"

void RunParentProcess();
void OnChildKilled(signal_t signum);
void OnChildDone(signal_t signum);