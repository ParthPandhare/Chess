#ifndef PIECE_H
#define PIECE_H

#include "constants.h"
#include <vector>

struct Position
{
	Position() { x = 0; y = 0; }
	Position(int x, int y) { this->x = x; this->y = y; }
	int x, y;

	friend bool operator==(const Position& lhs, const Position& rhs)
	{
		if (lhs.x == rhs.x && lhs.y == rhs.y)
			return true;
		return false;
	}
};

class Piece
{
public:
	Piece(int x_pos, int y_pos, int team, int piece_type)
	{
		position_.x = x_pos;
		position_.y = y_pos;
		team_ = team;
		is_active_ = true;
		piece_type_ = piece_type;
	}
	~Piece() {}

	Position getPosition() { return position_; }
	void setPosition(Position* pos) { position_.x = pos->x; position_.y = pos->y; }
	int getTeam() { return team_; }
	bool isActive() { return is_active_; }
	void setInactive() { is_active_ = false; }
	int getPieceType() { return piece_type_; }

	virtual std::vector<Position> getMoves() = 0;
	virtual void moveTo(Position* pos) = 0;			// serparate moveTo function needed for pawns & kings
	virtual bool isEnPassantAble() = 0;				// needed by utils to check en-passant-ability of pawns through piece pointers

private:
	Position position_;		// these are measured from the top left
	int team_;
	bool is_active_;
	int piece_type_;
};

#endif // !PIECE_H
