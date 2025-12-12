#include "ClientApp.h"

#include <cstring>
#include <iostream>
#include <unistd.h>

ClientApp::ClientApp() : isRunning(true) {}

void *ClientApp::listenThreadWrapper(void *context) {
  ((ClientApp *)context)->listenLoop();
  return nullptr;
}

void ClientApp::listenLoop() {
  std::string pipePath = CLIENT_PIPE_PREFIX + login;
  NamedPipe myPipe(pipePath);

  myPipe.removePipe();
  if (!myPipe.create() || !myPipe.openPipe(O_RDWR)) {
    std::cerr << "\n[Error] Could not create a personal channel to receive "
                 "messages.\n";
    return;
  }

  Packet pkt;
  while (isRunning) {
    if (myPipe.receive(&pkt, sizeof(Packet))) {
      switch (pkt.type) {
      case S_MSG:
        std::cout << "\n[SERVER]: " << pkt.payload << "\n" << std::flush;
        break;
      case S_INVITE:
        std::cout << "\n[INVITE]: Invite from " << pkt.sender
                  << ". Enter '/accept " << pkt.sender << "'\n"
                  << std::flush;
        break;
      case S_GAME_START:
        std::cout << "\n[GAME]: GAME HAS BEEN STARTED! Opponent: "
                  << pkt.payload
                  << "\n[GAME]: Your ships are automatically spaced."
                  << "\n[GAME]: Enter '/shoot X Y' (0-9)\n"
                  << std::flush;
        break;
      case S_BOARD:
        std::cout << "\n" << pkt.payload << "\n>" << std::flush;
        break;
      case S_SHOT_RESULT:
        std::cout << "\n[RESULT]: " << pkt.payload << " (" << pkt.x << ", "
                  << pkt.y << ")\n>" << std::flush;
        break;
      case S_GAME_OVER:
        std::cout << "\n\n====================================\n";
        std::cout << "               GAME OVER                \n";
        std::cout << "=======================================\n";
        std::cout << pkt.payload << "\n";
        std::cout << "=======================================\n";
        std::cout << "You are in main menu\n";
        std::cout << "Commands:\n";
        std::cout << "  /invite <login>  - Invite player\n";
        std::cout << "  /accept <login>  - Accept invite\n";
        std::cout << "  /shoot <x> <y>   - Shot (0-9)\n";
        std::cout << "  /quit            - Quit\n";
        std::cout << "> " << std::flush;
        break;
      }
    }
  }
  myPipe.closePipe();
  myPipe.removePipe();
}

void ClientApp::sendPacket(Packet &pkt) {
  NamedPipe serverPipe(SERVER_PIPE);
  if (serverPipe.openPipe(O_WRONLY)) {
    serverPipe.send(&pkt, sizeof(Packet));
    serverPipe.closePipe();
  } else {
    std::cout << "[Error] Server not available (not running).\n";
  }
}

void ClientApp::start() {
  std::cout << "=== SEA FIGHT CLIENT ===\n";
  std::cout << "Enter your login: ";
  std::cin >> login;

  if (pthread_create(&listenerThread, NULL, listenThreadWrapper, this) != 0) {
    std::cerr << "Error of creating the thread.\n";
    return;
  }
  sleep(1);

  Packet auth;
  auth.type = LOGIN;
  strcpy(auth.sender, login.c_str());
  sendPacket(auth);

  std::cout << "Commands:\n";
  std::cout << "  /invite <login>  - Invite player\n";
  std::cout << "  /accept <login>  - Accept invite\n";
  std::cout << "  /shoot <x> <y>   - Shot (0-9)\n";
  std::cout << "  /quit            - Quit\n";
  std::cout << "> ";

  std::string cmd;
  while (isRunning) {
    std::cin >> cmd;
    Packet pkt;
    memset(&pkt, 0, sizeof(Packet));
    pkt.type = -1;
    strcpy(pkt.sender, login.c_str());

    if (cmd == "/quit") {
      pkt.type = LOGOUT;
      sendPacket(pkt);
      isRunning = false;
    } else if (cmd == "/invite") {
      std::string target;
      std::cin >> target;
      pkt.type = INVITE;
      strcpy(pkt.target, target.c_str());
      sendPacket(pkt);
    } else if (cmd == "/accept") {
      std::string target;
      std::cin >> target;
      pkt.type = INVITE_RESPONSE;
      strcpy(pkt.target, target.c_str());
      pkt.payload[0] = 'Y';
      sendPacket(pkt);
    } else if (cmd == "/shoot") {
      std::cin >> pkt.x >> pkt.y;
      pkt.type = SHOOT;
      sendPacket(pkt);
    } else {
      std::cout << "Invalid command.\n";
    }
  }

  pthread_cancel(listenerThread);
  pthread_join(listenerThread, NULL);
}