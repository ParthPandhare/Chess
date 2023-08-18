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
		first_move_ = true;
		enpassant_able_ = false;
	}
	virtual ~Pawn() {}

	bool isPromotable();
	void checkNormalCapture(std::vector<Position>* moves);
	bool isEnPassantAble() { return enpassant_able_; }
	void setEnPassantAble(bool b) { enpassant_able_ = b; }

	virtual std::vector<Position> getMoves();
	virtual void moveTo(Position* pos) { setPosition(pos); first_move_ = false; }

private:
	bool first_move_;	// true if it's the pawn's first move (for double jump), false otherwise
	bool enpassant_able_;
};

#endif // !PAWN_H
