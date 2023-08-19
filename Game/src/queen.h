#ifndef QUEEN_H
#define QUEEN_H

#include "piece.h"
#include "constants.h"
#include "utils.h"
#include <vector>

class Queen : public Piece
{
public:
	Queen(int x_pos, int y_pos, int team)
		: Piece(x_pos, y_pos, team, 6 - 3 * team)
	{

	}
	~Queen() {}

	void getMovesInDirection(int x_dir, int y_dir, std::vector<Position>* moves);	// adds all possible moves in 1 direction to moves

	virtual std::vector<Position> getMoves();
	virtual bool isEnPassantAble() { return false; }
	virtual void setEnpassantAble(bool b) { return; }

private:

};

#endif // !QUEEN_H
