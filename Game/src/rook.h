#ifndef ROOK_H
#define ROOK_H

#include "piece.h"
#include "constants.h"
#include "utils.h"
#include <vector>

class Rook : public Piece
{
public:
	Rook(int x_pos, int y_pos, int team)
		: Piece(x_pos, y_pos, team, 3 - 3 * team)
	{

	}
	~Rook() {}

	void getMovesInDirection(int x_dir, int y_dir, std::vector<Position>* moves);	// adds all possible moves in 1 direction to moves

	virtual std::vector<Position> getMoves();
	virtual bool isEnPassantAble() { return false; }
	virtual void setEnpassantAble(bool b) { return; }

private:

};

#endif // !ROOK_H
