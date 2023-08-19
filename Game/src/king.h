#ifndef KING_H
#define KING_H

#include "piece.h"
#include "constants.h"
#include "utils.h"
#include <vector>

class King : public Piece
{
public:
	King(int x_pos, int y_pos, int team)
		: Piece(x_pos, y_pos, team, 7 - 3 * team)
	{

	}
	~King() {}

	void getMovesInDirection(int x_dir, int y_dir, std::vector<Position>* moves);	// adds all possible moves in 1 direction to moves

	virtual std::vector<Position> getMoves();
	virtual bool isEnPassantAble() { return false; }
	virtual void setEnpassantAble(bool b) { return; }

private:

};

#endif // !KING_H
