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

	if (first_move_ && !checkPiece(pos.x, pos.y + 2 * getTeam()))
		moves.push_back(Position(pos.x, pos.y + 2 * getTeam()));
	if (!checkPiece(pos.x, pos.y + 1 * getTeam()))
		moves.push_back(Position(pos.x, pos.y + 1 * getTeam()));
	checkNormalCapture(&moves);

	// code to check for en-passants

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