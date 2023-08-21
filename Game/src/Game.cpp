#include "Game.h"

Piece* piece_map_[8][8] = { nullptr };

Game::Game() 
{
	isRunning_ = false;
	left_click_pressed_ = false;
	window_ = nullptr;
	renderer_ = nullptr;
	board_image_ = nullptr;
	highlight_image_ = nullptr;
	selected_image_ = nullptr;
	promotion_image_ = nullptr;
	piece_clicked_ = nullptr;
	turn_ = WHITE;
	en_passantable_pawn_ = nullptr;
	w_king_ = nullptr;
	b_king_ = nullptr;
	for (int i = 0; i < 12; ++i) { piece_images_[i] = nullptr; }
	for (int i = 0; i < 32; ++i) { pieces_[i] = nullptr; }
}

Game::~Game() 
{
	SDL_DestroyWindow(window_);
	SDL_DestroyRenderer(renderer_);
	SDL_Quit();

	for (int i = 0; i < 32; ++i)
	{
		if (pieces_[i] != nullptr)
			delete pieces_[i];
	}

	std::cout << "Game Cleaned..." << std::endl;
}

void Game::init() 
{
	if (!SDL_Init(SDL_INIT_EVERYTHING)) 
	{
		std::cout << "Subsystems Initialized..." << std::endl;

		/* creates window */
		window_ = SDL_CreateWindow("Chess", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, constants::SCREEN_WIDTH, constants::SCREEN_HEIGHT, 0);
		assert(window_);		// checks to see if window is created successfully
		std::cout << "Window created..." << std::endl;

		/* creates renderer */
		renderer_ = SDL_CreateRenderer(window_, -1, 0);
		assert(renderer_);		// checks to see if renderer is created successfully
		SDL_SetRenderDrawColor(renderer_, 255, 255, 255, 255);		// background color (in this case white)
		std::cout << "Renderer created..." << std::endl;

		isRunning_ = true;
	}
	else
	{
		isRunning_ = false;
	}

	loadImages();

	// creates instances of all the pieces & adds them to the piece_map_:
		// pawns:
	for (int i = 0; i < 8; ++i)
	{
		pieces_[i] = new Pawn(i, 6, WHITE);
		piece_map_[i][6] = pieces_[i];
		pieces_[i + 8] = new Pawn(i, 1, BLACK);
		piece_map_[i][1] = pieces_[i + 8];
	}

		// knights, bishops, rooks:
	for (int i = 0; i < 2; ++i)
	{
		// bishops
		pieces_[i + 16] = new Bishop(2 + i * 3, 7, WHITE);
		piece_map_[2 + i * 3][7] = pieces_[i + 16];
		pieces_[i + 18] = new Bishop(2 + i * 3, 0, BLACK);
		piece_map_[2 + i * 3][0] = pieces_[i + 18];

		// rooks
		pieces_[i + 20] = new Rook(i * 7, 7, WHITE);
		piece_map_[i * 7][7] = pieces_[i + 20];
		pieces_[i + 22] = new Rook(i * 7, 0, BLACK);
		piece_map_[i * 7][0] = pieces_[i + 22];

		// knights
		pieces_[i + 24] = new Knight(1 + i * 5, 7, WHITE);
		piece_map_[1 + i * 5][7] = pieces_[i + 24];
		pieces_[i + 26] = new Knight(1 + i * 5, 0, BLACK);
		piece_map_[1 + i * 5][0] = pieces_[i + 26];
	}

		// queens:
	pieces_[28] = new Queen(3, 7, WHITE);
	piece_map_[3][7] = pieces_[28];
	pieces_[29] = new Queen(3, 0, BLACK);
	piece_map_[3][0] = pieces_[29];

		// kings:
	pieces_[30] = new King(4, 7, WHITE);
	piece_map_[4][7] = pieces_[30];
	w_king_ = pieces_[30];
	pieces_[31] = new King(4, 0, BLACK);
	piece_map_[4][0] = pieces_[31];
	b_king_ = pieces_[31];
}

void Game::handleEvents() 
{
	/* checks for user inputs */
	SDL_Event event;
	SDL_PollEvent(&event);

	if (event.type == SDL_QUIT)
		isRunning_ = false;
	else if (event.type == SDL_MOUSEBUTTONDOWN)
	{
		if (event.button.button == SDL_BUTTON_LEFT)
		{
			std::cout << "Left click pressed!" << std::endl;
			SDL_SetWindowMouseGrab(window_, SDL_TRUE);
			Position pos(event.button.x / constants::SQUARE_DIMENSION, event.button.y / constants::SQUARE_DIMENSION);

			if (left_click_pressed_)
			{
				// checks to see if the user wants to move the piece based on where they click
				std::vector<Position> moves = piece_clicked_->getMoves();
				std::vector<Position>::iterator itr = std::find(moves.begin(), moves.end(), pos);
				if (itr != moves.end())
				{
					Position old_position = piece_clicked_->getPosition();
					piece_map_[old_position.x][old_position.y] = nullptr;
					int piece_type = piece_clicked_->getPieceType();

					// DELETE PIECE IF CAPTURED
					if (checkPiece(&pos))
						deletePiece(piece_map_[pos.x][pos.y]);
					// delete piece if captured using en passant
					else if (piece_type == W_PAWN && piece_clicked_->getPosition().y == 3 && checkEnpassantablePawn(pos.x, pos.y + 1, WHITE))
						deletePiece(piece_map_[pos.x][pos.y + 1]);
					else if (piece_type == B_PAWN && piece_clicked_->getPosition().y == 4 && checkEnpassantablePawn(pos.x, pos.y - 1, BLACK))
						deletePiece(piece_map_[pos.x][pos.y - 1]);

					// sets enpassants after resetting the previous ones
					resetEnPassants();
					if ((piece_type == W_PAWN || piece_type == B_PAWN) &&
						(pos.y - piece_clicked_->getPosition().y) * piece_clicked_->getTeam() == 2)
						piece_clicked_->setEnpassantAble(true);

					// actually move the piece
					assert(!piece_map_[pos.x][pos.y]);	// makes sure the square you're trying to move to is empty (the piece there should be deleted first)
					piece_map_[pos.x][pos.y] = piece_clicked_;
					piece_clicked_->moveTo(&pos);

					// check for promotion
					if ((pos.y == 0 && piece_type == W_PAWN) || (pos.y == 7 && piece_type == B_PAWN))
						promotePiece(piece_clicked_);

					// change turn
					turn_ *= -1;
				}

				left_click_pressed_ = false;
				piece_clicked_ = nullptr;
			}
			else if (piece_map_[pos.x][pos.y] != nullptr && piece_map_[pos.x][pos.y]->getTeam() == turn_)	// only allow people to move on their turn
			{
				left_click_pressed_ = true;
				piece_clicked_ = piece_map_[pos.x][pos.y];
			}
		}
		else if (event.button.button == SDL_BUTTON_RIGHT)
		{
			std::cout << "Right click pressed!" << std::endl;
		}
	}
	else if (event.type == SDL_MOUSEBUTTONUP)
	{
		std::cout << "Mouse button released!" << std::endl;
		SDL_SetWindowMouseGrab(window_, SDL_FALSE);
	}
	else if (event.type == SDL_MOUSEMOTION)
	{
		std::cout << event.motion.x << ", " << event.motion.y << std::endl;
	}
}

void Game::update() 
{
	// could be cool to have like pieces captured or smthing here
}

void Game::render() 
{
	// if user input, then do all of this stuff. until then do nothing:
	SDL_RenderClear(renderer_);

	renderBoard();
	renderPieces(pieces_);
	if (piece_clicked_)
	{
		renderMultiple(highlight_image_, piece_clicked_->getMoves());
		renderTexture(selected_image_, piece_clicked_->getPosition());
	}

	// render checks
	if (turn_ == WHITE && isCheck(w_king_))
		renderTexture(highlight_image_, w_king_->getPosition());
	else if (isCheck(b_king_))
		renderTexture(highlight_image_, b_king_->getPosition());
	
	SDL_RenderPresent(renderer_);
}

void Game::loadImages()
{
	SDL_Surface* tmp[16];
	tmp[0] = IMG_Load("assets/board.png");
	tmp[1] = IMG_Load("assets/black/rook.png");
	tmp[2] = IMG_Load("assets/black/knight.png");
	tmp[3] = IMG_Load("assets/black/bishop.png");
	tmp[4] = IMG_Load("assets/black/queen.png");
	tmp[5] = IMG_Load("assets/black/king.png");
	tmp[6] = IMG_Load("assets/black/pawn.png");
	tmp[7] = IMG_Load("assets/white/rook.png");
	tmp[8] = IMG_Load("assets/white/knight.png");
	tmp[9] = IMG_Load("assets/white/bishop.png");
	tmp[10] = IMG_Load("assets/white/queen.png");
	tmp[11] = IMG_Load("assets/white/king.png");
	tmp[12] = IMG_Load("assets/white/pawn.png");
	tmp[13] = IMG_Load("assets/highlight.png");
	tmp[14] = IMG_Load("assets/selected.png");
	tmp[15] = IMG_Load("assets/promotion.png");

	board_image_ = SDL_CreateTextureFromSurface(renderer_, tmp[0]);
	highlight_image_ = SDL_CreateTextureFromSurface(renderer_, tmp[13]);
	selected_image_ = SDL_CreateTextureFromSurface(renderer_, tmp[14]);
	promotion_image_ = SDL_CreateTextureFromSurface(renderer_, tmp[15]);
	for (int i = 0; i < 12; ++i)
	{
		piece_images_[i] = SDL_CreateTextureFromSurface(renderer_, tmp[i + 1]);
	}

	for (int i = 0; i < 16; ++i)
	{
		SDL_FreeSurface(tmp[i]);
	}
}

void Game::renderTexture(SDL_Texture* texture, int x_pos, int y_pos)
{
	SDL_Rect dest_Rect;
	dest_Rect.x = x_pos * constants::SQUARE_DIMENSION;
	dest_Rect.y = y_pos * constants::SQUARE_DIMENSION;
	dest_Rect.h = constants::SQUARE_DIMENSION;
	dest_Rect.w = constants::SQUARE_DIMENSION;
	SDL_RenderCopy(renderer_, texture, NULL, &dest_Rect);
}

void Game::renderBoard()
{
	SDL_Rect dest_Rect;
	dest_Rect.x = 0;
	dest_Rect.y = 0;
	dest_Rect.h = constants::BOARD_DIMENSION;
	dest_Rect.w = constants::BOARD_DIMENSION;
	SDL_RenderCopy(renderer_, board_image_, NULL, &dest_Rect);
}

void Game::renderMultiple(SDL_Texture* texture, std::vector<Position> positions)
{
	SDL_Rect dest_Rect;
	dest_Rect.h = constants::SQUARE_DIMENSION;
	dest_Rect.w = constants::SQUARE_DIMENSION;
	for (int i = 0; i < positions.size(); ++i)
	{
		dest_Rect.x = positions[i].x * constants::SQUARE_DIMENSION;
		dest_Rect.y = positions[i].y * constants::SQUARE_DIMENSION;
		SDL_RenderCopy(renderer_, texture, NULL, &dest_Rect);
	}
}

void Game::renderPieces(Piece** pieces)
{
	for (int i = 0; i < 32; i++)
	{
		if (pieces[i] == nullptr)	// if there's no pieces in the array (ex: if it was captured)
			continue;
		renderTexture(piece_images_[pieces[i]->getPieceType()], pieces[i]->getPosition());
	}
}

void Game::deletePiece(Piece* piece)
{
	int i = 0;
	while (pieces_[i] != piece) { ++i; }
	piece_map_[piece->getPosition().x][piece->getPosition().y] = nullptr;
	delete pieces_[i];
	pieces_[i] = nullptr;
}

void Game::resetEnPassants()
{
	for (int i = 0; i < 32; ++i)
	{
		if (pieces_[i] != nullptr && pieces_[i]->isEnPassantAble())
		{
			pieces_[i]->setEnpassantAble(false);
			return;
		}
	}
}

void Game::promotePiece(Piece* piece)
{
	std::cout << "Promoting piece" << std::endl;
	Position pos = piece->getPosition();
	int piece_type = piece->getPieceType();

	// renders options:
	for (int i = 0; i < 4; ++i)
	{
		renderTexture(selected_image_, pos.x, pos.y - i * piece->getTeam());
		renderTexture(piece_images_[6 - 3 * piece->getTeam() - i], pos.x, pos.y - i * piece->getTeam());
	}

	SDL_RenderPresent(renderer_);

	// waits for input & promotes accordingly:
	for (; ; )
	{
		SDL_Event event;
		SDL_PollEvent(&event);
		if (event.type == SDL_QUIT)
		{
			isRunning_ = false;
			return;
		}
		else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
		{
			Position clicked(event.button.x / constants::SQUARE_DIMENSION, event.button.y / constants::SQUARE_DIMENSION);
			if (pos == clicked)		// queen selected
			{
				int i = 0;
				while (pieces_[i] != piece) { ++i; }
				deletePiece(pieces_[i]);
				pieces_[i] = new Queen(pos.x, pos.y, turn_);
				piece_map_[pos.x][pos.y] = pieces_[i];
				break;
			}
			else if (pos.x == clicked.x)
			{
				if (clicked.y == pos.y - 1 * piece->getTeam())
				{
					int i = 0;
					while (pieces_[i] != piece) { ++i; }
					deletePiece(pieces_[i]);
					pieces_[i] = new Bishop(pos.x, pos.y, turn_);
					piece_map_[pos.x][pos.y] = pieces_[i];
					break;
				}
				else if (clicked.y == pos.y - 2 * piece->getTeam())
				{
					int i = 0;
					while (pieces_[i] != piece) { ++i; }
					deletePiece(pieces_[i]);
					pieces_[i] = new Knight(pos.x, pos.y, turn_);
					piece_map_[pos.x][pos.y] = pieces_[i];
					break;
				}
				else if (clicked.y == pos.y - 3 * piece->getTeam())
				{
					int i = 0;
					while (pieces_[i] != piece) { ++i; }
					deletePiece(pieces_[i]);
					pieces_[i] = new Rook(pos.x, pos.y, turn_);
					piece_map_[pos.x][pos.y] = pieces_[i];
					break;
				}
			}
		}
	}
}

bool Game::isCheck(Piece* target_piece)
{
	int team = target_piece->getTeam();
	Position pos = target_piece->getPosition();
	
	// iterate through piece_map_, find enemy pieces, check if enemy pieces attack target_piece
	for (int x = 0; x < 8; ++x)
	{
		for (int y = 0; y < 8; ++y)
		{
			if (piece_map_[x][y] != nullptr && piece_map_[x][y]->getTeam() != team)
			{
				std::vector<Position> enemy_moves = piece_map_[x][y]->getMoves();
				if (std::find(enemy_moves.begin(), enemy_moves.end(), pos) != enemy_moves.end())
					return true;
			}
		}
	}
	return false;
}