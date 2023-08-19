#ifndef BISHOP_H
#define BISHOP_H

#include "piece.h"
#include "constants.h"
#include "utils.h"
#include <vector>

class Bishop : public Piece
{
public:
	Bishop(int x_pos, int y_pos, int team)
		: Piece(x_pos, y_pos, team, 5 - 3 * team)
	{

	}
	~Bishop() {}

	void getMovesInDirection(int x_dir, int y_dir, std::vector<Position>* moves);	// adds all possible moves in 1 direction to moves

	virtual std::vector<Position> getMoves();
	virtual void moveTo(Position* pos) { setPosition(pos); }
	virtual bool isEnPassantAble() { return false; }

private:

};

#endif // ! BISHOP_H
