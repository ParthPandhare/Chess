#ifndef PAWN_H
#define PAWN_H

#include "piece.h"
#include "constants.h"
#include "utils.h"
#include <vector>

class Pawn : public Piece
{
public:
	Pawn(int x_pos, int y_pos, int team)
		: Piece(x_pos, y_pos, team, 8 - team * 3)
	{
		enpassant_able_ = false;
	}
	virtual ~Pawn() {}

	bool isPromotable();
	void checkNormalCapture(std::vector<Position>* moves);

	virtual void setEnpassantAble(bool b) { enpassant_able_ = b; }
	virtual bool isEnPassantAble() { return enpassant_able_; }
	virtual std::vector<Position> getMoves();

private:
	bool enpassant_able_;
};

#endif // !PAWN_H
