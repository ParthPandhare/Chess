#include "minimax.h"

Minimax* Minimax::instancePtr = NULL;

Move Minimax::getMove(int** boardLayout, std::vector<Move> moves, int team)
{
	Move best_move = moves[0];
	int best_eval = 0x7FFFFFFF * team;
	int start_square = EMPTY;
	int goal_square = EMPTY;

	for (Move move : moves)
	{
		start_square = boardLayout[move.start.x][move.start.y];
		goal_square = boardLayout[move.goal.x][move.goal.y];
		boardLayout[move.start.x][move.start.y] = EMPTY;
		boardLayout[move.goal.x][move.goal.y] = start_square;
		int current_eval = getEvaluation(boardLayout);
		if ((team == WHITE && current_eval > best_eval) || (team == BLACK && current_eval < best_eval))
		{
			best_eval = current_eval;
			best_move = move;
		}
		boardLayout[move.start.x][move.start.y] = start_square;
		boardLayout[move.goal.x][move.goal.y] = goal_square;
	}

	return best_move;
}

int Minimax::getEvaluation(int** boardLayout)
{
	int evaluation = 0;
	for (int x = 0; x < 8; ++x)
	{
		for (int y = 0; y < 8; ++y)
		{
			switch (boardLayout[x][y])
			{
			case EMPTY:
				break;
			case W_PAWN:
				evaluation += weights::PAWN_W;
				break;
			case B_PAWN:
				evaluation += weights::PAWN_B;
				break;
			case W_BISHOP:
				evaluation += weights::BISHOP_W;
				break;
			case B_BISHOP:
				evaluation += weights::BISHOP_B;
				break;
			case W_KNIGHT:
				evaluation += weights::KNIGHT_W;
				break;
			case B_KNIGHT:
				evaluation += weights::KNIGHT_B;
				break;
			case W_ROOK:
				evaluation += weights::ROOK_W;
				break;
			case B_ROOK:
				evaluation += weights::ROOK_B;
				break;
			case W_QUEEN:
				evaluation += weights::QUEEN_W;
				break;
			case B_QUEEN:
				evaluation += weights::QUEEN_B;
				break;
			case W_KING:
				evaluation += weights::KING_W;
				break;
			case B_KING:
				evaluation += weights::KING_B;
				break;
			default:
				std::cout << "ERROR: INCORRECT BOARDLAYOUT GIVEN" << std::endl;
				assert(false);
			}
		}
	}
	return evaluation;
}