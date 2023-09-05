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
		eval = 0;
	}
	Position start;
	Position goal;
	int eval;	// only used by the minimax methods to persist actual evaluation after multiple moves
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
	Move getMove(int** boardLayout, int team, std::vector<Move>* movesToPlay, int depth, int original_evaluation, bool top_loop);

private:
	static Minimax* instancePtr;
	Minimax() {}

	int getEvaluation(int** boardLayout);
	int getEvaluationAfterMove(int** boardLayout, Move* move, int team);
};

#endif // !MINIMAX_H
