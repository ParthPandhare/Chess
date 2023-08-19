#include "pawn.h"


bool Pawn::isPromotable()
{
	if (getTeam() == WHITE && getPosition().y == 0)
		return true;
	if (getTeam() == BLACK && getPosition().y == 7)
		return true;
	return false;
}

std::vector<Position> Pawn::getMoves()
{
	std::vector<Position> moves;
	Position pos = getPosition();

	// normal moves (first move double jump, moving forward)
	if (!checkPiece(pos.x, pos.y + 1 * getTeam()))
	{
		moves.push_back(Position(pos.x, pos.y + 1 * getTeam()));

		if (first_move_ && !checkPiece(pos.x, pos.y + 2 * getTeam()))
			moves.push_back(Position(pos.x, pos.y + 2 * getTeam()));
	}
	checkNormalCapture(&moves);

	// code to check for en-passants
	if (getTeam() == WHITE && pos.y == 3)
	{
		if (checkEnpassantablePawn(pos.x + 1, pos.y, WHITE))
			moves.push_back(Position(pos.x + 1, pos.y - 1));
		else if (checkEnpassantablePawn(pos.x - 1, pos.y, WHITE))
			moves.push_back(Position(pos.x - 1, pos.y - 1));
	}
	else if (getTeam() == BLACK && pos.y == 4)
	{
		if (checkEnpassantablePawn(pos.x + 1, pos.y, BLACK))
			moves.push_back(Position(pos.x + 1, pos.y + 1));
		else if (checkEnpassantablePawn(pos.x - 1, pos.y, BLACK))
			moves.push_back(Position(pos.x - 1, pos.y + 1));
	}

	return moves;
}

/* checks diagonal left & right; adds the positions to moves if there's a piece there */
void Pawn::checkNormalCapture(std::vector<Position>* moves)
{
	Position pos_left(getPosition().x - 1, getPosition().y + 1 * getTeam());
	Position pos_right(getPosition().x + 1, getPosition().y + 1 * getTeam());

	if (checkEnemyPiece(&pos_left, getTeam()))
		moves->push_back(pos_left);
	if (checkEnemyPiece(&pos_right, getTeam()))
		moves->push_back(pos_right);
}
