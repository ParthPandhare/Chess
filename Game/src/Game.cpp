#include "Game.h"

Game* Game::instancePtr = NULL;
Piece* piece_map_[8][8] = { nullptr };

Game::~Game() 
{
	SDL_DestroyWindow(window_);
	SDL_DestroyRenderer(renderer_);
	SDL_Quit();

	for (int x = 0; x < 8; ++x)
	{
		for (int y = 0; y < 8; ++y)
		{
			if (piece_map_[x][y] != nullptr)
				delete piece_map_[x][y];
		}
	}

	std::cout << "Game Cleaned..." << std::endl;
}

void Game::setGameMode(int game_mode)
{
	this->Game::~Game();
	this->Game::Game();
	this->init();
	this->game_mode_ = game_mode;
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
		piece_map_[i][6] = new Pawn(i, 6, WHITE);
		piece_map_[i][1] = new Pawn(i, 1, BLACK);
	}

		// knights, bishops, rooks:
	for (int i = 0; i < 2; ++i)
	{
		// bishops
		piece_map_[2 + i * 3][7] = new Bishop(2 + i * 3, 7, WHITE);
		piece_map_[2 + i * 3][0] = new Bishop(2 + i * 3, 0, BLACK);

		// rooks
		piece_map_[i * 7][7] = new Rook(i * 7, 7, WHITE);
		piece_map_[i * 7][0] = new Rook(i * 7, 0, BLACK);

		// knights
		piece_map_[1 + i * 5][7] = new Knight(1 + i * 5, 7, WHITE);
		piece_map_[1 + i * 5][0] = new Knight(1 + i * 5, 0, BLACK);
	}

		// queens:
	piece_map_[3][7] = new Queen(3, 7, WHITE);
	piece_map_[3][0] = new Queen(3, 0, BLACK);

		// kings:
	w_king_ = new King(4, 7, WHITE);
	piece_map_[4][7] = w_king_;
	b_king_ = new King(4, 0, BLACK);
	piece_map_[4][0] = b_king_;
}

void Game::handleEvents() 
{
	if (game_mode_ == TWO_PLAYER)
		twoPlayerEventHandling();
	else if (game_mode_ == ONE_PLAYER_WHITE)
		onePlayerEventHandling();
	else if (game_mode_ == ONE_PLAYER_BLACK)
		onePlayerEventHandling();
}

void Game::twoPlayerEventHandling()
{
	// checks for mate
	if (result_ == 0)
		setCheckMate();
	else
		return;

	/* checks for user inputs & handles them */
	SDL_Event event;
	SDL_WaitEvent(&event);
	handleUserInput(event);
}

void Game::onePlayerEventHandling()
{
	// checks for mate
	/*if (result_ == 0)
	{
		setCheckMate();
		if (result_ != 0)
			return;
	}
	else
		return;*/

	/* checks for user inputs */
	SDL_Event event;
	SDL_WaitEvent(&event);

	if (event.type == SDL_QUIT)
		isRunning_ = false;

	// if it's the computer's turn:
	if (turn_ == game_mode_ + 4)
		handleMinimaxMove();
	// if it's the user's turn:
	else
		handleUserInput(event);
}

void Game::update() 
{
	// could be cool to have like pieces captured or smthing here
}

void Game::render() 
{
	// if it's checkmate
	if (result_ != 0)
	{
		SDL_Event event;
		SDL_WaitEvent(&event);
		if (event.type == SDL_QUIT)
		{
			isRunning_ = false;
		}
		return;
	}

	// if the board is the same, nothing needs to be done
	if (!board_changed_)
		return;

	// if user input, then do all of this stuff. until then do nothing:
	SDL_RenderClear(renderer_);

	renderBoard();
	renderPieces();
	if (piece_clicked_)
	{
		std::vector<Position> moves = piece_clicked_->getMoves();
		if (piece_clicked_ == w_king_)
			canCastle(w_king_, &moves);
		else if (piece_clicked_ == b_king_)
			canCastle(b_king_, &moves);
		getLegalMoves(&moves, piece_clicked_);

		/*for (int i = 0; i < moves.size(); ++i)
			std::cout << moves[i].x << ", " << moves[i].y << std::endl;*/

		renderMultiple(highlight_image_, moves);
		renderTexture(selected_image_, piece_clicked_->getPosition());
	}

	// render checks
	if (turn_ == WHITE && isCheck(w_king_))
		renderTexture(highlight_image_, w_king_->getPosition());
	else if (isCheck(b_king_))
		renderTexture(highlight_image_, b_king_->getPosition());
	
	SDL_RenderPresent(renderer_);
	board_changed_ = false;
}

int** Game::getBoardLayout()
{
	int** board = new int* [8];
	for (int x = 0; x < 8; ++x)
	{
		board[x] = new int[8];
		for (int y = 0; y < 8; ++y)
		{
			if (piece_map_[x][y] == nullptr)
				board[x][y] = EMPTY;
			else
				board[x][y] = piece_map_[x][y]->getPieceType();
		}
	}
	return board;
}

std::vector<Move> Game::getPossibleMoves(int team)
{
	std::vector<Move> moves;
	for (int x = 0; x < 8; ++x)
	{
		for (int y = 0; y < 8; ++y)
		{
			if (piece_map_[x][y] != nullptr && piece_map_[x][y]->getTeam() == team)
			{
				std::vector<Position> goal_positions = piece_map_[x][y]->getMoves();
				// adds castling moves if possible:
				if (piece_map_[x][y] == w_king_)
					canCastle(w_king_, &goal_positions);
				else if (piece_map_[x][y] == b_king_)
					canCastle(b_king_, &goal_positions);

				//getLegalMoves(&goal_positions, piece_map_[x][y]);
				for (Position pos : goal_positions)
					moves.push_back(Move(Position(x, y), pos));
			}
		}
	}
	return moves;
}

std::vector<Move> Game::getPossibleMovesAfterMove(std::vector<Move>* moves_to_play)
{
	if (moves_to_play->empty())
		return getPossibleMoves(turn_);

	// stores old variables:
	bool first_move[8][8] = { false };
	int old_w_kingside = w_castle_king_;
	int old_w_queenside = w_castle_queen_;
	int old_b_kingside = b_castle_king_;
	int old_b_queenside = b_castle_queen_;
	int old_turn = turn_;
	Piece* old_piece_map[8][8] = { nullptr };
	for (int x = 0; x < 8; ++x)
	{
		for (int y = 0; y < 8; ++y)
		{
			if (piece_map_[x][y] != nullptr)
			{
				old_piece_map[x][y] = piece_map_[x][y];
				if (piece_map_[x][y]->isFirstMove())
					first_move[x][y] = true;
			}
		}
	}
	
	for (int i = 0; i < moves_to_play->size(); ++i)
		moveWithoutDeleting(&(*moves_to_play)[i]);

	std::vector<Move> moves = getPossibleMoves(turn_);

	// restores old variables
	for (int x = 0; x < 8; ++x)
	{
		for (int y = 0; y < 8; ++y)
		{
			if (piece_map_[x][y] != old_piece_map[x][y])
			{
				piece_map_[x][y] = old_piece_map[x][y];
				if (piece_map_[x][y] != nullptr)
				{
					Position pos(x, y);
					piece_map_[x][y]->moveTo(&pos);
					if (first_move[x][y])
						piece_map_[x][y]->resetFirstMove();
				}
			}
		}
	}
	w_castle_king_ = old_w_kingside;
	w_castle_queen_ = old_w_queenside;
	b_castle_king_ = old_b_kingside;
	b_castle_queen_ = old_b_queenside;
	turn_ = old_turn;

	return moves;
}


//~~~ HELPER FUNCTIONS ~~~//

void Game::move(Move* move)
{
	Position old_position = move->start;
	piece_map_[old_position.x][old_position.y] = nullptr;
	int piece_type = piece_clicked_->getPieceType();
	Position pos = move->goal;

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

	// move rook if castled & set castling rights (if rook or king moves, set castling rights to false for that side)
	if (turn_ == WHITE && (w_castle_king_ || w_castle_queen_))
	{
		if (piece_clicked_ == w_king_)
		{
			if (pos.x - old_position.x == 2)	// if-elses move rook if needed
			{
				Position temp(pos.x - 1, pos.y);
				piece_map_[7][7]->moveTo(&temp);
				piece_map_[temp.x][temp.y] = piece_map_[7][7];
				piece_map_[7][7] = nullptr;
			}
			else if (pos.x - old_position.x == -2)
			{
				Position temp(pos.x + 1, pos.y);
				piece_map_[0][7]->moveTo(&temp);
				piece_map_[temp.x][temp.y] = piece_map_[0][7];
				piece_map_[0][7] = nullptr;
			}
			w_castle_king_ = false;
			w_castle_queen_ = false;
		}
		else if (w_castle_king_ && piece_map_[7][7] != nullptr && (piece_clicked_ == piece_map_[7][7] || piece_map_[7][7]->getPieceType() != W_ROOK))
			w_castle_king_ = false;
		else if (w_castle_king_ && piece_map_[0][7] != nullptr && (piece_clicked_ == piece_map_[0][7] || piece_map_[0][7]->getPieceType() != W_ROOK))
			w_castle_queen_ = false;
	}
	else if (turn_ == BLACK && (b_castle_king_ || b_castle_queen_))
	{
		if (piece_clicked_ == b_king_)
		{
			if (pos.x - old_position.x == 2)	// if it castled kingside
			{
				Position temp(pos.x - 1, pos.y);
				piece_map_[7][0]->moveTo(&temp);
				piece_map_[temp.x][temp.y] = piece_map_[7][0];
				piece_map_[7][0] = nullptr;
			}
			else if (pos.x - old_position.x == -2)
			{
				Position temp(pos.x + 1, pos.y);
				piece_map_[0][0]->moveTo(&temp);
				piece_map_[temp.x][temp.y] = piece_map_[0][0];
				piece_map_[0][0] = nullptr;
			}
			b_castle_king_ = false;
			b_castle_queen_ = false;
		}
		else if (b_castle_king_ && piece_map_[7][0] != nullptr && (piece_clicked_ == piece_map_[7][0] || piece_map_[7][0]->getPieceType() != B_ROOK))
			b_castle_king_ = false;
		else if (b_castle_king_ && piece_map_[0][0] != nullptr && (piece_clicked_ == piece_map_[0][0] || piece_map_[0][0]->getPieceType() != B_ROOK))
			b_castle_queen_ = false;
	}

	// check for promotion
	if ((pos.y == 0 && piece_type == W_PAWN) || (pos.y == 7 && piece_type == B_PAWN))
		promotePiece(piece_clicked_);

	// change turn
	turn_ *= -1;
}

void Game::moveWithoutDeleting(Move* move)
{
	Position old_position = move->start;
	Piece* piece_selected = piece_map_[old_position.x][old_position.y];
	piece_map_[old_position.x][old_position.y] = nullptr;
	int piece_type = piece_selected->getPieceType();
	Position pos = move->goal;

	// sets enpassants after resetting the previous ones
	resetEnPassants();
	if ((piece_type == W_PAWN || piece_type == B_PAWN) &&
		(pos.y - piece_selected->getPosition().y) * piece_selected->getTeam() == 2)
		piece_selected->setEnpassantAble(true);

	// actually move the piece
	piece_map_[pos.x][pos.y] = piece_selected;
	piece_selected->moveTo(&pos);

	// move rook if castled & set castling rights (if rook or king moves, set castling rights to false for that side)
	if (turn_ == WHITE && (w_castle_king_ || w_castle_queen_))
	{
		if (piece_clicked_ == w_king_)
		{
			if (pos.x - old_position.x == 2)	// if-elses move rook if needed
			{
				Position temp(pos.x - 1, pos.y);
				piece_map_[7][7]->moveTo(&temp);
				piece_map_[temp.x][temp.y] = piece_map_[7][7];
				piece_map_[7][7] = nullptr;
			}
			else if (pos.x - old_position.x == -2)
			{
				Position temp(pos.x + 1, pos.y);
				piece_map_[0][7]->moveTo(&temp);
				piece_map_[temp.x][temp.y] = piece_map_[0][7];
				piece_map_[0][7] = nullptr;
			}
			w_castle_king_ = false;
			w_castle_queen_ = false;
		}
		else if (w_castle_king_ && piece_map_[7][7] != nullptr && (piece_clicked_ == piece_map_[7][7] || piece_map_[7][7]->getPieceType() != W_ROOK))
			w_castle_king_ = false;
		else if (w_castle_king_ && piece_map_[0][7] != nullptr && (piece_clicked_ == piece_map_[0][7] || piece_map_[0][7]->getPieceType() != W_ROOK))
			w_castle_queen_ = false;
	}
	else if (turn_ == BLACK && (b_castle_king_ || b_castle_queen_))
	{
		if (piece_clicked_ == b_king_)
		{
			if (pos.x - old_position.x == 2)	// if it castled kingside
			{
				Position temp(pos.x - 1, pos.y);
				piece_map_[7][0]->moveTo(&temp);
				piece_map_[temp.x][temp.y] = piece_map_[7][0];
				piece_map_[7][0] = nullptr;
			}
			else if (pos.x - old_position.x == -2)
			{
				Position temp(pos.x + 1, pos.y);
				piece_map_[0][0]->moveTo(&temp);
				piece_map_[temp.x][temp.y] = piece_map_[0][0];
				piece_map_[0][0] = nullptr;
			}
			b_castle_king_ = false;
			b_castle_queen_ = false;
		}
		else if (b_castle_king_ && piece_map_[7][0] != nullptr && (piece_clicked_ == piece_map_[7][0] || piece_map_[7][0]->getPieceType() != B_ROOK))
			b_castle_king_ = false;
		else if (b_castle_king_ && piece_map_[0][0] != nullptr && (piece_clicked_ == piece_map_[0][0] || piece_map_[0][0]->getPieceType() != B_ROOK))
			b_castle_queen_ = false;
	}

	// check for promotion
	if ((pos.y == 0 && piece_type == W_PAWN) || (pos.y == 7 && piece_type == B_PAWN))
		promotePiece(piece_selected);

	// change turn
	turn_ *= -1;
}

void Game::handleUserInput(SDL_Event event)
{
	if (event.type == SDL_QUIT)
		isRunning_ = false;
	else if (event.type == SDL_MOUSEBUTTONDOWN)
	{
		if (event.button.button == SDL_BUTTON_LEFT)
		{
			//std::cout << "Left click pressed!" << std::endl;
			//SDL_SetWindowMouseGrab(window_, SDL_TRUE);
			Position pos(event.button.x / constants::SQUARE_DIMENSION, event.button.y / constants::SQUARE_DIMENSION);

			if (left_click_pressed_)
			{
				// checks to see if the user wants to move the piece based on where they click & gets rid of illegal moves
				std::vector<Position> moves = piece_clicked_->getMoves();
				getLegalMoves(&moves, piece_clicked_);

				// adds castling moves if possible:
				if (piece_clicked_ == w_king_)
					canCastle(w_king_, &moves);
				else if (piece_clicked_ == b_king_)
					canCastle(b_king_, &moves);

				std::vector<Position>::iterator itr = std::find(moves.begin(), moves.end(), pos);
				if (itr != moves.end())
				{
					Position start = piece_clicked_->getPosition();
					Move mv(start, pos);
					this->move(&mv);
				}
				board_changed_ = true;
				left_click_pressed_ = false;
				piece_clicked_ = nullptr;
			}
			else if (piece_map_[pos.x][pos.y] != nullptr && piece_map_[pos.x][pos.y]->getTeam() == turn_)	// only allow people to move on their turn
			{
				board_changed_ = true;
				left_click_pressed_ = true;
				piece_clicked_ = piece_map_[pos.x][pos.y];
			}
		}
		else if (event.button.button == SDL_BUTTON_RIGHT)
		{
			//std::cout << "Right click pressed!" << std::endl;
		}
	}
	else if (event.type == SDL_MOUSEBUTTONUP)
	{
		//std::cout << "Mouse button released!" << std::endl;
		//SDL_SetWindowMouseGrab(window_, SDL_FALSE);
	}
	else if (event.type == SDL_MOUSEMOTION)
	{
		//std::cout << event.motion.x << ", " << event.motion.y << std::endl;
	}
}

void Game::handleMinimaxMove()
{
	std::vector<Move> moves_to_play;
	Move computer_move = Minimax::getInstance()->getMove(getBoardLayout(), game_mode_ + 4, &moves_to_play, 3);
	std::cout << computer_move.start.x << ", " << computer_move.start.y << " --> " << computer_move.goal.x << ", " << computer_move.goal.y << std::endl;
	piece_clicked_ = piece_map_[computer_move.start.x][computer_move.start.y];

	// for promotions:
	if ((piece_clicked_->getPieceType() == W_PAWN && computer_move.goal.y == 0) || (piece_clicked_->getPieceType() == B_PAWN && computer_move.goal.y == 7))
		moveWithPromotion(&computer_move, W_QUEEN);
	else
		this->move(&computer_move);
	piece_clicked_ = nullptr;
	board_changed_ = true;
}

void Game::setCheckMate()
{
	if (turn_ == WHITE && isMate(w_king_))
	{
		result_ = BLACK;
		std::cout << "~~~ GAME OVER ~~~" << std::endl;
		std::cout << "~~~ BLACK WINS ~~~" << std::endl;
		return;
	}
	else if (turn_ == BLACK && isMate(b_king_))
	{
		result_ = WHITE;
		std::cout << "~~~ GAME OVER ~~~" << std::endl;
		std::cout << "~~~ WHITE WINS ~~~" << std::endl;
		return;
	}
}

// specifically for the computer:
void Game::moveWithPromotion(Move* move, int promoted_piece)
{
	Position old_position = move->start;
	piece_map_[old_position.x][old_position.y] = nullptr;
	int piece_type = piece_clicked_->getPieceType();
	Position pos = move->goal;

	// DELETE PIECE IF CAPTURED
	if (checkPiece(&pos))
		deletePiece(piece_map_[pos.x][pos.y]);

	// resets en passants
	resetEnPassants();

	// actually move the piece
	assert(!piece_map_[pos.x][pos.y]);	// makes sure the square you're trying to move to is empty (the piece there should be deleted first)
	piece_map_[pos.x][pos.y] = piece_clicked_;
	piece_clicked_->moveTo(&pos);

	// move rook if castled & set castling rights (if rook or king moves, set castling rights to false for that side)
	if (turn_ == WHITE && (w_castle_king_ || w_castle_queen_))
	{
		if (piece_clicked_ == w_king_)
		{
			if (pos.x - old_position.x == 2)	// if-elses move rook if needed
			{
				Position temp(pos.x - 1, pos.y);
				piece_map_[7][7]->moveTo(&temp);
			}
			else if (pos.x - old_position.x == -2)
			{
				Position temp(pos.x + 1, pos.y);
				piece_map_[0][7]->moveTo(&temp);
			}
			w_castle_king_ = false;
			w_castle_queen_ = false;
		}
		else if (w_castle_king_ && piece_map_[7][7] != nullptr && (piece_clicked_ == piece_map_[7][7] || piece_map_[7][7]->getPieceType() != W_ROOK))
			w_castle_king_ = false;
		else if (w_castle_king_ && piece_map_[0][7] != nullptr && (piece_clicked_ == piece_map_[0][7] || piece_map_[0][7]->getPieceType() != W_ROOK))
			w_castle_queen_ = false;
	}
	else if (turn_ == BLACK && (b_castle_king_ || b_castle_queen_))
	{
		if (piece_clicked_ == b_king_)
		{
			if (pos.x - old_position.x == 2)	// if it castled kingside
			{
				Position temp(pos.x - 1, pos.y);
				piece_map_[7][0]->moveTo(&temp);
			}
			else if (pos.x - old_position.x == -2)
			{
				Position temp(pos.x + 1, pos.y);
				piece_map_[0][0]->moveTo(&temp);
			}
			b_castle_king_ = false;
			b_castle_queen_ = false;
		}
		else if (b_castle_king_ && piece_map_[7][0] != nullptr && (piece_clicked_ == piece_map_[7][0] || piece_map_[7][0]->getPieceType() != B_ROOK))
			b_castle_king_ = false;
		else if (b_castle_king_ && piece_map_[0][0] != nullptr && (piece_clicked_ == piece_map_[0][0] || piece_map_[0][0]->getPieceType() != B_ROOK))
			b_castle_queen_ = false;
	}

	// promote the piece
	switch (promoted_piece)
	{
	case B_QUEEN:
	case W_QUEEN:
		delete piece_map_[pos.x][pos.y];
		piece_map_[pos.x][pos.y] = new Queen(pos.x, pos.y, turn_);
		break;
	case B_BISHOP:
	case W_BISHOP:
		delete piece_map_[pos.x][pos.y];
		piece_map_[pos.x][pos.y] = new Bishop(pos.x, pos.y, turn_);
		break;
	case B_KNIGHT:
	case W_KNIGHT:
		delete piece_map_[pos.x][pos.y];
		piece_map_[pos.x][pos.y] = new Knight(pos.x, pos.y, turn_);
		break;
	case B_ROOK:
	case W_ROOK:
		delete piece_map_[pos.x][pos.y];
		piece_map_[pos.x][pos.y] = new Rook(pos.x, pos.y, turn_);
		break;
	default:
		std::cout << "ERROR WITH PROMOTION IN FUNCTION moveWithPromotion" << std::endl;
		assert(false);
	}

	// change turn
	turn_ *= -1;
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

void Game::renderPieces()
{
	for (int x = 0; x < 8; ++x)
	{
		for (int y = 0; y < 8; ++y)
		{
			if (piece_map_[x][y] == nullptr)
				continue;
			renderTexture(piece_images_[piece_map_[x][y]->getPieceType()], piece_map_[x][y]->getPosition());
		}
	}
}

void Game::getLegalMoves(std::vector<Position>* moves, Piece* piece)
{
	for (int i = 0; i < moves->size(); )
	{
		if (!isLegal(piece, (*moves)[i]))
		{
			moves->erase(moves->begin() + i);
			continue;
		}
		else
			++i;
	}
}

void Game::deletePiece(Piece* piece)
{
	Position pos = piece->getPosition();
	delete piece_map_[pos.x][pos.y];
	piece_map_[pos.x][pos.y] = nullptr;
}

void Game::resetEnPassants()
{
	for (int x = 0; x < 8; ++x)
	{
		for (int y = 0; y < 8; ++y)
		{
			if (piece_map_[x][y] != nullptr && piece_map_[x][y]->isEnPassantAble())
			{
				piece_map_[x][y]->setEnpassantAble(false);
				return;
			}
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
		SDL_WaitEvent(&event);
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
				delete piece_map_[pos.x][pos.y];
				piece_map_[pos.x][pos.y] = new Queen(pos.x, pos.y, turn_);
				break;
			}
			else if (pos.x == clicked.x)
			{
				if (clicked.y == pos.y - 1 * piece->getTeam())
				{
					delete piece_map_[pos.x][pos.y];
					piece_map_[pos.x][pos.y] = new Bishop(pos.x, pos.y, turn_);
					break;
				}
				else if (clicked.y == pos.y - 2 * piece->getTeam())
				{
					delete piece_map_[pos.x][pos.y];
					piece_map_[pos.x][pos.y] = new Knight(pos.x, pos.y, turn_);
					break;
				}
				else if (clicked.y == pos.y - 3 * piece->getTeam())
				{
					delete piece_map_[pos.x][pos.y];
					piece_map_[pos.x][pos.y] = new Rook(pos.x, pos.y, turn_);
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

bool Game::isCheck(Position pos, int team)
{
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

void Game::canCastle(Piece* piece, std::vector<Position>* moves)
{
	Position pos = piece->getPosition();
	
	if (((piece->getTeam() == WHITE && w_castle_king_) || (piece->getTeam() == BLACK && b_castle_king_)) &&					// if can castle kingside
		(piece_map_[pos.x + 1][pos.y] == nullptr && piece_map_[pos.x + 2][pos.y] == nullptr) &&								// if squares empty
		(!isCheck(Position(pos.x + 1, pos.y), piece->getTeam()) && !isCheck(Position(pos.x + 2, pos.y), piece->getTeam())) && // not check
		!isCheck(piece))	// if king not in check
		moves->push_back(Position(pos.x + 2, pos.y));
	if (((piece->getTeam() == WHITE && w_castle_queen_) || (piece->getTeam() == BLACK && b_castle_queen_)) && 
		(piece_map_[pos.x - 1][pos.y] == nullptr && piece_map_[pos.x - 2][pos.y] == nullptr) &&
		(!isCheck(Position(pos.x - 1, pos.y), piece->getTeam()) && !isCheck(Position(pos.x - 2, pos.y), piece->getTeam())) && 
		!isCheck(piece))
		moves->push_back(Position(pos.x - 2, pos.y));
}

bool Game::isLegal(Piece* piece, Position pos)
{
	bool first_move = piece->isFirstMove();

	// create temporary captured piece if the move is a capture
	Piece* captured_piece = nullptr;
	if (checkPiece(&pos))
		captured_piece = piece_map_[pos.x][pos.y];

	// save old position & change the moving piece's position
	Position old_position = piece->getPosition();
	piece->moveTo(&pos);
	piece_map_[pos.x][pos.y] = piece;
	piece_map_[old_position.x][old_position.y] = nullptr;

	// check if the king is in check, false if it is, true if it isn't
	bool is_legal = true;
	if ((piece->getTeam() == WHITE && isCheck(w_king_)) || (piece->getTeam() == BLACK && isCheck(b_king_)))
		is_legal = false;

	// reset all pieces & the piece_map_
	piece->moveTo(&old_position);
	if (first_move)
		piece->resetFirstMove();
	piece_map_[old_position.x][old_position.y] = piece;
	if (captured_piece != nullptr)
		piece_map_[pos.x][pos.y] = captured_piece;
	else
		piece_map_[pos.x][pos.y] = nullptr;

	return is_legal;
}

bool Game::isMate(Piece* piece)
{
	int team = piece->getTeam();
	if (isCheck(piece))
	{
		for (int x = 0; x < 8; ++x)
		{
			for (int y = 0; y < 8; ++y)
			{
				if (piece_map_[x][y] != nullptr && piece_map_[x][y]->getTeam() == team)
				{
					std::vector<Position> moves = piece_map_[x][y]->getMoves();
					getLegalMoves(&moves, piece_map_[x][y]);
					if (!moves.empty())
						return false;
				}
			}
		}
		return true;
	}
	return false;
}