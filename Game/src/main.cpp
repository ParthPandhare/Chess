#include "Game.h"


static Game* g = nullptr;

int main(int argc, char *argv[]) 
{
	g = Game::getInstance();
	g->init();
	g->setGameMode(ONE_PLAYER_WHITE);

	while (g->running())
	{
		g->handleEvents();
		// game->update();
		g->render();
	}

	return 0;
}