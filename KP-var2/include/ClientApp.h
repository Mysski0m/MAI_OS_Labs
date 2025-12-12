#pragma once

#include "protocol.h"
#include "wrappers.h"

#include <pthread.h>
#include <string>

class ClientApp {
public:
  ClientApp();
  ~ClientApp() = default;
  void start();

private:
  std::string login;
  bool isRunning;
  pthread_t listenerThread;

  static void *listenThreadWrapper(void *context);
  void listenLoop();

  void sendPacket(Packet &pkt);
};