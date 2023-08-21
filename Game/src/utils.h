#ifndef UTILS_H
#define UTILS_H

#include "piece.h"
#include "Game.h"
#include "pieceMap.h"

bool isValidPosition(Position* pos);
bool isValidPosition(int x, int y);
bool checkEnemyPiece(Position* pos, int team);
bool checkEnemyPiece(int x, int y, int team);
bool checkPiece(Position* pos);
bool checkPiece(int x, int y);
bool checkEnpassantablePawn(Position* pos, int team);
bool checkEnpassantablePawn(int x, int y, int team);

#endif // !UTILS_H
