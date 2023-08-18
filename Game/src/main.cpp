#include "Game.h"


Game* game = nullptr;

int main(int argc, char *argv[]) 
{
	game = new Game();
	game->init();

	while (game->running())
	{
		game->handleEvents();
		game->update();
		game->render();
	}

	delete game;
	return 0;
}