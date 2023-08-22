#include "Game.h"


static Game* g = nullptr;

int main(int argc, char *argv[]) 
{
	g = Game::getInstance();
	g->init();

	while (g->running())
	{
		g->handleEvents();
		// game->update();
		g->render();
	}

	return 0;
}