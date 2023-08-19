#ifndef KNIGHT_H
#define KNIGHT_H

#include "piece.h"
#include "constants.h"
#include "utils.h"
#include <vector>

class Knight : public Piece
{
public:
	Knight(int x_pos, int y_pos, int team)
		: Piece(x_pos, y_pos, team, 4 - 3 * team)
	{

	}
	~Knight() {}

	void getMovesInDirection(int x_dir, int y_dir, std::vector<Position>* moves);	// adds all possible moves in 1 direction to moves

	virtual std::vector<Position> getMoves();
	virtual void moveTo(Position* pos) { setPosition(pos); }
	virtual bool isEnPassantAble() { return false; }

private:

};

#endif // !KNIGHT_H
