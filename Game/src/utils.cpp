#include "utils.h"

bool isValidPosition(Position* pos)
{
	if (pos->x > 7 || pos->x < 0 || pos->y > 7 || pos->y < 0)
		return false;
	return true;
}

bool isValidPosition(int x, int y)
{
	Position pos(x, y);
	return isValidPosition(&pos);
}

/*
	this will check whether there's an enemy piece at a given position
	if the position is invalid, it will return false
*/
bool checkEnemyPiece(int x, int y, int team)
{
	if (!isValidPosition(x, y))
		return false;

	// code to actually check whether or not there's an enemy piece
	if (piece_map_[x][y] == nullptr)
		return false;
	else if (piece_map_[x][y]->getTeam() == team)
		return false;
	return true;
}

// overloaded checkEnemyPiece
bool checkEnemyPiece(Position* pos, int team) { return checkEnemyPiece(pos->x, pos->y, team); }

/*
	same thing as checkEnemyPiece but checks for all pieces
*/
bool checkPiece(Position* pos)
{
	if (!isValidPosition(pos))
		return false;

	// code to check for a piece
	if (piece_map_[pos->x][pos->y] == nullptr)
		return false;
	return true;
}

// overloaded checkPiece
bool checkPiece(int x, int y) { Position pos(x, y); return checkPiece(&pos); }

/*
	checks whether there's a pawn at the position given of the opposite team
	if there is it checks whether the pawn can be captured using en-passant
*/
bool checkEnpassantablePawn(int x, int y, int team)
{
	if (!isValidPosition(x, y))
		return false;

	if (piece_map_[x][y] == nullptr)
		return false;

	if (piece_map_[x][y]->getTeam() == (-1 * team) && piece_map_[x][y]->getPieceType() == 8 + 3 * team)
	{
		if (piece_map_[x][y]->isEnPassantAble())
			return true;
	}
	return false;
}

// overloaded checkEnpassantablePawn
bool checkEnpassantablePawn(Position* pos, int team) { return checkEnpassantablePawn(pos->x, pos->y, team); }