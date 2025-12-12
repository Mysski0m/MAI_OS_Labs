#pragma once

#include "GameLogic.h"

#include <string>

#define SERVER_PIPE "/tmp/battleship_server_pipe"
#define CLIENT_PIPE_PREFIX "/tmp/client_"

enum MsgType {
  LOGIN,
  INVITE,
  INVITE_RESPONSE,
  SHOOT,
  LOGOUT,
  S_MSG,
  S_INVITE,
  S_GAME_START,
  S_SHOT_RESULT,
  S_GAME_OVER,
  S_BOARD
};

struct Packet {
  int type;
  char sender[32];
  char target[32];
  char payload[512];
  int x;
  int y;
  int shotResult;
};

struct Player {
  std::string login;
  bool inGame;
  std::string opponent;
  GameBoard board;
  bool isTurn;
};