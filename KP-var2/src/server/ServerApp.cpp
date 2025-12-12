#include "ServerApp.h"

#include <iostream>
#include <cstring>
#include <algorithm>

ServerApp::ServerApp() : serverPipe(SERVER_PIPE), isRunning(true) {
    list_mutex = PTHREAD_MUTEX_INITIALIZER;
}

ServerApp::~ServerApp() {
    pthread_mutex_destroy(&list_mutex);
}

void ServerApp::sendToClient(const std::string& login, Packet& pkt) {
    std::string pipePath = CLIENT_PIPE_PREFIX + login;
    NamedPipe pipe(pipePath);
    if (pipe.openPipe(O_WRONLY)) {
        pipe.send(&pkt, sizeof(Packet));
        pipe.closePipe();
    } else {
        std::cerr << "[Error] Failed to send message to player " << login << " (pipe is not available)\n";
    }
}

void ServerApp::sendBoard(Player* pTarget, GameBoard& boardOwner, bool showShips, const char* title) {
    Packet pkt;
    pkt.type = S_BOARD;
    strcpy(pkt.sender, "SERVER");

    char boardStr[400];
    boardOwner.getBoardString(boardStr, showShips);
    
    sprintf(pkt.payload, "%s\n%s", title, boardStr);
    sendToClient(pTarget->login, pkt);
}

Player* ServerApp::findPlayer(const std::string& login) {
    for (auto& p : players) {
        if (p.login == login) return &p;
    }
    return nullptr;
}

void ServerApp::handleLogin(Packet &pkt) {
    if (findPlayer(pkt.sender)) {
        std::cout << "[Login] Reject: " << pkt.sender << " is already online." << std::endl;
        return;
    }
    players.push_back({pkt.sender, false, "", GameBoard()});
    std::cout << "[Login] New player: " << pkt.sender << std::endl;

    Packet resp;
    resp.type = S_MSG;
    strcpy(resp.payload, "Welcome to the Sea Fight server!");
    sendToClient(pkt.sender, resp);
}

void ServerApp::handleInvite(Packet& pkt) {
    Player* target = findPlayer(pkt.target);
    if (target && target->login != pkt.sender && !target->inGame) {
        Packet invitePkt;
        invitePkt.type = S_INVITE;
        strcpy(invitePkt.sender, pkt.sender);
        sendToClient(pkt.target, invitePkt);

        Packet confirmPkt;
        confirmPkt.type = S_MSG;
        char buffer[128];
        sprintf(buffer, "Invite received to player %s! Wait for answer...", pkt.target);
        strcpy(confirmPkt.payload, buffer);
        sendToClient(pkt.sender, confirmPkt);

        std::cout << "[Invite] " << pkt.sender << " -> " << pkt.target << std::endl;
    } else {
        Packet err;
        err.type = S_MSG;
        strcpy(err.payload, "Player not found or already playing.");
        sendToClient(pkt.sender, err);
    }
}

void ServerApp::handleInviteResponse(Packet& pkt) {
    if (pkt.payload[0] == 'Y' || pkt.payload[0] == 'y') {
        Player* sender = findPlayer(pkt.sender);
        Player* opponent = findPlayer(pkt.target);

        if (sender && opponent) {
            sender->inGame = true;
            sender->opponent = opponent->login;
            sender->board.placeShipsRandomly();
            sender->isTurn = false;

            opponent->inGame = true;
            opponent->opponent = sender->login;
            opponent->board.placeShipsRandomly();
            opponent->isTurn = true;

            Packet start;
            start.type = S_GAME_START;

            strcpy(start.payload, opponent->login.c_str());
            strcat(start.payload, " (Wait for opponent turn)");
            sendToClient(sender->login, start);

            strcpy(start.payload, sender->login.c_str());
            strcat(start.payload, " (YOUR TURN)");
            sendToClient(opponent->login, start);

            sendBoard(sender, sender->board, true, "YOUR BOARD:");
            sendBoard(opponent, opponent->board, true, "YOUR BOARD:");

            std::cout << "[Game Start] " << sender->login << " vs " << opponent->login << std::endl;
        }
    }
}

void ServerApp::hanldeShoot(Packet& pkt) {
    Player* shooter = findPlayer(pkt.sender);

    if (!shooter) { std::cout << "Shooter not found: " << pkt.sender << "\n"; return; }
    if (!shooter->inGame) { std::cout << "Shooter not in game: " << pkt.sender << "\n"; return; }
    if (shooter->opponent.empty()) { std::cout << "Opponent string empty for " << pkt.sender << "\n"; return; }
    // if (!shooter || !shooter->inGame) return;

    if (!shooter->isTurn) {
        Packet err;
        err.type = S_MSG;
        strcpy(err.payload, "Now is NOT your turn. Wait for opponent.");
        sendToClient(shooter->login, err);
        return;
    }

    Player* victim = findPlayer(shooter->opponent);
    if (!victim) { std::cout << "Victim not found: " << shooter->opponent << "\n"; return; }
    // if (!victim) return;

    ShotResult res = victim->board.processShot(pkt.x, pkt.y);
    
    if (res == RES_REPEAT) {
        Packet err;
        err.type = S_MSG;
        strcpy(err.payload, "You have already shot here or the coordinates are wrong. Repeat.");
        sendToClient(shooter->login, err);
        return;
    }

    std::cout << "[Shoot] " << shooter->login << " at (" << pkt.x << ", " << pkt.y << ")" << std::endl;

    if (res == RES_LOSE) {
        Packet pktWin;
        pktWin.type = S_GAME_OVER;
        strcpy(pktWin.payload, "WIN! You destroy all opponent's ships\n");
        sendToClient(shooter->login, pktWin);

        Packet pktLose;
        pktLose.type = S_GAME_OVER;
        strcpy(pktLose.payload, "LOSE! Your ships destroyed\n");
        sendToClient(victim->login, pktLose);
        
        shooter->inGame = false;
        shooter->opponent = "";
        shooter->isTurn = false;

        victim->inGame = false;
        victim->opponent = "";
        victim->isTurn = false;

        std::cout << "[Game Over] Winner:" << shooter->login << "\n";
        return;
    }

    Packet respShooter;
    respShooter.type = S_SHOT_RESULT;
    respShooter.x = pkt.x;
    respShooter.y = pkt.y;
    respShooter.shotResult = (int)res;

    if (res == RES_HIT) {
        strcpy(respShooter.payload, "HIT! Shoot again!");
    } else {
        strcpy(respShooter.payload, "MISS. Change turn...");
    }
    
    sendToClient(shooter->login, respShooter);

    sendBoard(shooter, victim->board, false, "Opponent's board (Radar):");

    Packet respVictim = respShooter;
    if (res == RES_HIT) {
        strcpy(respVictim.payload, "Your ship has been HIT! Opponent's turn...");
    } else {
        strcpy(respVictim.payload, "Opponent MISS! Your turn!");
    }

    sendToClient(victim->login, respVictim);

    sendBoard(victim, victim->board, true, "Your board:");

    if (res == RES_MISS) {
        shooter->isTurn = false;
        victim->isTurn = true;
    }
}

void ServerApp::handleLogout(Packet& pkt) {
    Player* quittingPlayer = findPlayer(pkt.sender);

    if (quittingPlayer && quittingPlayer->inGame) {
        Player* opponent = findPlayer(quittingPlayer->opponent);
        if (opponent) {
            Packet winPkt;
            winPkt.type = S_GAME_OVER;
            strcpy(winPkt.payload, "Opponent left the game.\n YOU WON!");
            sendToClient(opponent->login, winPkt);

            opponent->inGame = false;
            opponent->opponent = "";
            opponent->isTurn = false;
        }
    }

    auto it = std::remove_if(players.begin(), players.end(),
        [&](const Player& p) { return p.login == pkt.sender; });

    if (it != players.end()) {
        players.erase(it, players.end());
        std::cout << "[Logout] Player " << pkt.sender << " removed from server's list.\n";
    }
}

void ServerApp::run() {
    serverPipe.removePipe();
    if (!serverPipe.create()) {
        std::cerr << "Fatal: Unable to create server pipe. Check access rights." << std::endl;
        return;
    }

    if (!serverPipe.openPipe(O_RDWR)) {
        std::cerr << "Fatal: Unable to open pipe." << std::endl;
        return;
    }

    std::cout << "Server running. Waiting..." << std::endl;

    Packet pkt;
    while (serverPipe.receive(&pkt, sizeof(Packet))) {
        pthread_mutex_lock(&list_mutex);

        switch (pkt.type) {
            case LOGIN: handleLogin(pkt); break;
            case INVITE: handleInvite(pkt); break;
            case INVITE_RESPONSE: handleInviteResponse(pkt); break;
            case SHOOT: hanldeShoot(pkt); break;
            case LOGOUT: handleLogout(pkt); break;
        }

        pthread_mutex_unlock(&list_mutex);
    }

    serverPipe.closePipe();
    serverPipe.removePipe();
}