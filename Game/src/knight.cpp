#include "knight.h"

std::vector<Position> Knight::getMoves()
{
	std::vector<Position> moves;

	getMovesInDirection(1, 1, &moves);
	getMovesInDirection(1, -1, &moves);
	getMovesInDirection(-1, 1, &moves);
	getMovesInDirection(-1, -1, &moves);

	return moves;
}

void Knight::getMovesInDirection(int x_dir, int y_dir, std::vector<Position>* moves)
{
	Position pos = getPosition();
	pos.x += x_dir;
	pos.y += 2 * y_dir;
	if (isValidPosition(&pos))
	{
		if (checkEnemyPiece(&pos, getTeam()))
			moves->push_back(pos);
		else if (!checkPiece(&pos))
			moves->push_back(pos);
	}

	pos = getPosition();
	pos.x += 2 * x_dir;
	pos.y += y_dir;
	if (isValidPosition(&pos))
	{
		if (checkEnemyPiece(&pos, getTeam()))
			moves->push_back(pos);
		else if (!checkPiece(&pos))
			moves->push_back(pos);
	}
}