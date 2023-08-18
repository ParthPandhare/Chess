#ifndef UTILS_H
#define UTILS_H

#include "piece.h"
#include "Game.h"

bool isValidPosition(Position* pos);
bool isValidPosition(int x, int y);
bool checkEnemyPiece(Position* pos, int team);
bool checkEnemyPiece(int x, int y, int team);
bool checkPiece(Position* pos);
bool checkPiece(int x, int y);

#endif // !UTILS_H
