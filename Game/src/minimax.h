#ifndef MINIMAX_H
#define MINIMAX_H

#include "constants.h"
#include "piece.h"
#include <iostream>
#include <vector>
#include <cassert>
#include "Game.h"

struct Move
{
	Move() {}
	Move(Position st, Position go)
	{
		this->start = st;
		this->goal = go;
	}
	Position start;
	Position goal;
};

class Minimax
{
public:
	// deleting copy constructor
	Minimax(const Minimax& obj)
		= delete;

	// returns the single instance of this class
	static Minimax* getInstance()
	{
		if (instancePtr == NULL)
		{
			instancePtr = new Minimax();
			return instancePtr;
		}
		else
		{
			return instancePtr;
		}
	}

	Move getMove(int** boardLayout, int team, std::vector<Move>* movesToPlay, int depth);

private:
	static Minimax* instancePtr;
	Minimax() {}

	int getEvaluation(int** boardLayout);
	int getEvaluationAfterMove(int** boardLayout, Move* move, int team);
};

#endif // !MINIMAX_H
