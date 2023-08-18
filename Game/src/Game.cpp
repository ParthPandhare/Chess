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
	piece_clicked_ = nullptr;
	for (int i = 0; i < 12; ++i) { piece_images_[i] = nullptr; }
	for (int i = 0; i < 32; ++i) { pieces_[i] = nullptr; }
}

Game::~Game() 
{
	SDL_DestroyWindow(window_);
	SDL_DestroyRenderer(renderer_);
	SDL_Quit();
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
		// kings & queens:
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
			
			if (left_click_pressed_)
			{
				// checks to see if the user wants to move the piece based on where they click
				Position pos(event.button.x / constants::SQUARE_DIMENSION, event.button.y / constants::SQUARE_DIMENSION);
				std::vector<Position> moves = piece_clicked_->getMoves();
				std::vector<Position>::iterator itr = std::find(moves.begin(), moves.end(), pos);
				if (itr != moves.end())
				{
					Position old_position = piece_clicked_->getPosition();
					piece_map_[old_position.x][old_position.y] = nullptr;
					piece_map_[pos.x][pos.y] = piece_clicked_;
					piece_clicked_->moveTo(&pos);
				}

				left_click_pressed_ = false;
				piece_clicked_ = nullptr;
			}
			else
			{
				left_click_pressed_ = true;
				piece_clicked_ = piece_map_[event.button.x / constants::SQUARE_DIMENSION][event.button.y / constants::SQUARE_DIMENSION];
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
	
	SDL_RenderPresent(renderer_);
}

void Game::loadImages()
{
	SDL_Surface* tmp[15];
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

	board_image_ = SDL_CreateTextureFromSurface(renderer_, tmp[0]);
	highlight_image_ = SDL_CreateTextureFromSurface(renderer_, tmp[13]);
	selected_image_ = SDL_CreateTextureFromSurface(renderer_, tmp[14]);
	for (int i = 0; i < 12; ++i)
	{
		piece_images_[i] = SDL_CreateTextureFromSurface(renderer_, tmp[i + 1]);
	}

	for (int i = 0; i < 15; ++i)
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