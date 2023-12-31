#ifndef GAME_H
#define GAME_H

#include "SDL.h"
#include "SDL_image.h"
#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include "constants.h"
#include "pawn.h"
#include "bishop.h"
#include "knight.h"
#include "rook.h"
#include "queen.h"
#include "king.h"
#include "pieceMap.h"
#include "minimax.h"

struct Move;

class Game		// SINGLETON CLASS
{
public:
	// deleting copy constructor
	Game(const Game& obj)
		= delete;

	// returns the single instance of this class
	static Game* getInstance()
	{
		if (instancePtr == NULL)
		{
			instancePtr = new Game();
			return instancePtr;
		}
		else
		{
			return instancePtr;
		}
	}

	~Game();

	void init();
	void handleEvents();
	void update();
	void render();
	bool running() { return isRunning_; }
	void setGameMode(int game_mode);

	int** getBoardLayout();
	std::vector<Move> getPossibleMoves(int team);
	std::vector<Move> getPossibleMovesAfterMove(std::vector<Move>* moves_to_play);
	

private:
	static Game* instancePtr;
	Game() 
	{
		this->isRunning_ = false;
		this->left_click_pressed_ = false;
		this->board_changed_ = true;
		this->w_castle_king_ = true;
		this->b_castle_king_ = true;
		this->w_castle_queen_ = true;
		this->b_castle_queen_ = true;
		this->window_ = nullptr;
		this->renderer_ = nullptr;
		this->board_image_ = nullptr;
		this->highlight_image_ = nullptr;
		this->selected_image_ = nullptr;
		this->promotion_image_ = nullptr;
		this->piece_clicked_ = nullptr;
		this->turn_ = WHITE;
		this->en_passantable_pawn_ = nullptr;
		this->w_king_ = nullptr;
		this->b_king_ = nullptr;
		for (int i = 0; i < 12; ++i) { this->piece_images_[i] = nullptr; }
		this->result_ = 0;
		this->game_mode_ = TWO_PLAYER;
	}

	void twoPlayerEventHandling();
	void onePlayerEventHandling();

	void loadImages();
	void renderTexture(SDL_Texture* texture, int x_pos, int y_pos);		// for rendering a single texture of any kind
	void renderTexture(SDL_Texture* texture, Position pos) { renderTexture(texture, pos.x, pos.y); };
	void renderBoard();
	void renderMultiple(SDL_Texture* texture, std::vector<Position> positions);		// renders the given texture to all given squares; useful for highlights
	void renderPieces();
	void getLegalMoves(std::vector<Position>* moves, Piece* piece);
	void deletePiece(Piece* piece);
	void resetEnPassants();
	void promotePiece(Piece* piece);
	bool isCheck(Piece* target_piece);
	bool isCheck(Position pos, int team);
	void canCastle(Piece* piece, std::vector<Position>* moves);
	bool isLegal(Piece* piece, Position pos);
	bool isMate(Piece* piece);
	void move(Move* move);
	void moveWithoutDeleting(Move* move);
	void moveWithPromotion(Move* move, int promoted_piece);
	void setCheckMate();
	void handleUserInput(SDL_Event event);
	void handleMinimaxMove();

	bool isRunning_, left_click_pressed_, board_changed_, w_castle_king_, w_castle_queen_, b_castle_king_, b_castle_queen_;
	int turn_, result_, game_mode_;	// for result_: 0 by default, 1 if black won, -1 if white won
	SDL_Window* window_;
	SDL_Renderer* renderer_;
	SDL_Texture* board_image_;
	SDL_Texture* highlight_image_;
	SDL_Texture* selected_image_;
	SDL_Texture* promotion_image_;
	SDL_Texture* piece_images_[12];
	Piece* piece_clicked_;
	Piece* en_passantable_pawn_;
	Piece* w_king_;
	Piece* b_king_;
};

#endif