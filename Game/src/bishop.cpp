#include "bishop.h"

std::vector<Position> Bishop::getMoves()
{
	std::vector<Position> moves;

	getMovesInDirection(1, 1, &moves);
	getMovesInDirection(1, -1, &moves);
	getMovesInDirection(-1, 1, &moves);
	getMovesInDirection(-1, -1, &moves);

	return moves;
}

void Bishop::getMovesInDirection(int x_dir, int y_dir, std::vector<Position>* moves)
{
	Position pos = getPosition();
	pos.x += x_dir;
	pos.y += y_dir;
	while (isValidPosition(&pos))
	{
		if (checkPiece(&pos))
		{
			if (checkEnemyPiece(&pos, getTeam()))
				moves->push_back(pos);
			break;
		}
		moves->push_back(pos);
		pos.x += x_dir;
		pos.y += y_dir;
	}
}