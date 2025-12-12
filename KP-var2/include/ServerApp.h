#pragma once

#include "protocol.h"
#include "wrappers.h"

#include <vector>
#include <string>
#include <pthread.h>

class ServerApp {
public:
    ServerApp();
    ~ServerApp();
    void run();

private:
    std::vector<Player> players;
    pthread_mutex_t list_mutex;
    NamedPipe serverPipe;
    bool isRunning;

    Player* findPlayer(const std::string& login);
    void sendToClient(const std::string& login, Packet& pkt);
    void sendBoard(Player* pTarget, GameBoard& boardOwner, bool showShips, const char* title);

    void handleLogin(Packet& pkt);
    void handleInvite(Packet& pkt);
    void handleInviteResponse(Packet& pkt);
    void hanldeShoot(Packet& pkt);
    void handleLogout(Packet& pkt);
};