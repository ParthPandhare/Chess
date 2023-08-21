#include "king.h"

std::vector<Position> King::getMoves()
{
	std::vector<Position> moves;

	// bishop directions:
	getMovesInDirection(1, 1, &moves);
	getMovesInDirection(1, -1, &moves);
	getMovesInDirection(-1, 1, &moves);
	getMovesInDirection(-1, -1, &moves);
	// rook directions:
	getMovesInDirection(0, 1, &moves);
	getMovesInDirection(0, -1, &moves);
	getMovesInDirection(1, 0, &moves);
	getMovesInDirection(-1, 0, &moves);

	return moves;
}

void King::getMovesInDirection(int x_dir, int y_dir, std::vector<Position>* moves)
{
	Position pos = getPosition();
	pos.x += x_dir;
	pos.y += y_dir;
	if (isValidPosition(&pos))
	{
		if (checkPiece(&pos))
		{
			if (checkEnemyPiece(&pos, getTeam()))
				moves->push_back(pos);
			return;
		}
		moves->push_back(pos);
	}
}