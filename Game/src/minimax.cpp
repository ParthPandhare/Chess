#include "minimax.h"

Minimax* Minimax::instancePtr = NULL;

Move Minimax::getMove(int** boardLayout, int team, std::vector<Move>* movesToPlay, int depth)
{
	std::vector<Move> moves = Game::getInstance()->getPossibleMovesAfterMove(movesToPlay);

	Move best_move = moves[0];
	int best_eval = 0x7FFFFFFF * team;

	for (Move move : moves)
	{
		if (depth == 0)
		{
			int current_eval = getEvaluationAfterMove(boardLayout, &move, team);
			if ((team == WHITE && current_eval > best_eval) || (team == BLACK && current_eval < best_eval))
			{
				best_eval = current_eval;
				best_move = move;
			}
		}
		else // if depth != 0
		{
			int start_square = EMPTY;
			int goal_square = EMPTY;
			int en_passant_square = EMPTY;
			bool castling_kingside = false;
			bool castling_queenside = false;

			start_square = boardLayout[move.start.x][move.start.y];
			goal_square = boardLayout[move.goal.x][move.goal.y];
			boardLayout[move.start.x][move.start.y] = EMPTY;
			boardLayout[move.goal.x][move.goal.y] = start_square;

			// if promoting:
			if ((start_square == W_PAWN && move.goal.y == 0) || (start_square == B_PAWN && move.goal.y == 7))
				boardLayout[move.goal.x][move.goal.y] = 6 - 3 * team;

			// if en passanting:
			if (((start_square == W_PAWN && move.start.y == 3) || (start_square == B_PAWN && move.start.y == 4)) &&
				goal_square == EMPTY && move.start.x != move.goal.x)
			{
				en_passant_square = boardLayout[move.goal.x][move.goal.y - team];
				boardLayout[move.goal.x][move.goal.y - team] = EMPTY;
			}

			// if castling:
			if (start_square == 7 - 3 * team)
			{
				if (move.goal.x - move.start.x == 2)	// if castling kingside
				{
					castling_kingside = true;
					boardLayout[7][move.start.y] = EMPTY;
					boardLayout[5][move.start.y] = 3 - 3 * team;
				}
				else if (move.goal.x - move.start.x == -2)		// if castling queenside
				{
					castling_queenside = true;
					boardLayout[0][move.start.y] = EMPTY;
					boardLayout[3][move.start.y] = 3 - 3 * team;
				}
			}

			movesToPlay->push_back(move);
			Move next_depth_best_move = getMove(boardLayout, -team, movesToPlay, depth - 1);
			movesToPlay->pop_back();

			int current_eval = getEvaluationAfterMove(boardLayout, &next_depth_best_move, -team);
			if ((team == WHITE && current_eval > best_eval) || (team == BLACK && current_eval < best_eval))
			{
				best_eval = current_eval;
				best_move = move;
			}

			boardLayout[move.start.x][move.start.y] = start_square;
			boardLayout[move.goal.x][move.goal.y] = goal_square;

			// resetting en passants & castles
			if (en_passant_square != EMPTY)
			{
				boardLayout[move.goal.x][move.goal.y - team] = en_passant_square;
				en_passant_square = EMPTY;
			}
			if (castling_kingside)
			{
				castling_kingside = false;
				boardLayout[7][move.start.y] = 3 - 3 * team;
				boardLayout[5][move.start.y] = EMPTY;
			}
			else if (castling_queenside)
			{
				castling_queenside = false;
				boardLayout[0][move.start.y] = 3 - 3 * team;
				boardLayout[3][move.start.y] = EMPTY;
			}
		}
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

int Minimax::getEvaluationAfterMove(int** boardLayout, Move* move, int team)
{
	int start_square = EMPTY;
	int goal_square = EMPTY;
	int en_passant_square = EMPTY;
	bool castling_kingside = false;
	bool castling_queenside = false;

	start_square = boardLayout[move->start.x][move->start.y];
	goal_square = boardLayout[move->goal.x][move->goal.y];
	boardLayout[move->start.x][move->start.y] = EMPTY;
	boardLayout[move->goal.x][move->goal.y] = start_square;

	// if promoting:
	if ((start_square == W_PAWN && move->goal.y == 0) || (start_square == B_PAWN && move->goal.y == 7))
		boardLayout[move->goal.x][move->goal.y] = 6 - 3 * team;

	// if en passanting:
	if (((start_square == W_PAWN && move->start.y == 3) || (start_square == B_PAWN && move->start.y == 4)) &&
		goal_square == EMPTY && move->start.x != move->goal.x)
	{
		en_passant_square = boardLayout[move->goal.x][move->goal.y - team];
		boardLayout[move->goal.x][move->goal.y - team] = EMPTY;
	}

	// if castling:
	if (start_square == 7 - 3 * team)
	{
		if (move->goal.x - move->start.x == 2)	// if castling kingside
		{
			castling_kingside = true;
			boardLayout[7][move->start.y] = EMPTY;
			boardLayout[5][move->start.y] = 3 - 3 * team;
		}
		else if (move->goal.x - move->start.x == -2)		// if castling queenside
		{
			castling_queenside = true;
			boardLayout[0][move->start.y] = EMPTY;
			boardLayout[3][move->start.y] = 3 - 3 * team;
		}
	}

	int evaluation = getEvaluation(boardLayout);

	boardLayout[move->start.x][move->start.y] = start_square;
	boardLayout[move->goal.x][move->goal.y] = goal_square;

	// resetting en passants & castles
	if (en_passant_square != EMPTY)
	{
		boardLayout[move->goal.x][move->goal.y - team] = en_passant_square;
		en_passant_square = EMPTY;
	}
	if (castling_kingside)
	{
		castling_kingside = false;
		boardLayout[7][move->start.y] = 3 - 3 * team;
		boardLayout[5][move->start.y] = EMPTY;
	}
	else if (castling_queenside)
	{
		castling_queenside = false;
		boardLayout[0][move->start.y] = 3 - 3 * team;
		boardLayout[3][move->start.y] = EMPTY;
	}

	return evaluation;
}
