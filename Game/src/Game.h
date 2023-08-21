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

class Game 
{
public:
	Game();
	~Game();

	void init();

	void handleEvents();
	void update();
	void render();

	bool running() { return isRunning_; }

	void loadImages();
	void renderTexture(SDL_Texture* texture, int x_pos, int y_pos);		// for rendering a single texture of any kind
	void renderTexture(SDL_Texture* texture, Position pos) { renderTexture(texture, pos.x, pos.y); };
	void renderBoard();
	void renderMultiple(SDL_Texture* texture, std::vector<Position> positions);		// renders the given texture to all given squares; useful for highlights
	void renderPieces(Piece** pieces);	// given an array of pointers to pieces, it'll render all the pieces
	void getLegalMoves(std::vector<Position>* moves, Piece* piece);
	void deletePiece(Piece* piece);
	void resetEnPassants();
	void promotePiece(Piece* piece);
	bool isCheck(Piece* target_piece);
	bool isLegal(Piece* piece, Position pos);

private:
	bool isRunning_, left_click_pressed_;
	SDL_Window* window_;
	SDL_Renderer* renderer_;
	SDL_Texture* board_image_;
	SDL_Texture* highlight_image_;
	SDL_Texture* selected_image_;
	SDL_Texture* promotion_image_;
	SDL_Texture* piece_images_[12];
	Piece* pieces_[32];
	Piece* piece_clicked_;
	int turn_;
	Piece* en_passantable_pawn_;
	Piece* w_king_;
	Piece* b_king_;
};

#endif